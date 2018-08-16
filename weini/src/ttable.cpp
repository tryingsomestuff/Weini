#include "ttable.h"
#include "definitions.h"
#include "position.h"
#include "moveGenerator.h"
#include "search.h"

namespace{
   Bucket* ttable  = 0;
   Bucket* ttableQ = 0;

   std::mutex mutexTT;
   std::mutex mutexTTQ;
}


void Transposition::InitTT() {

    LOG(logINFO) << "Init TT";

    if (ttable == 0) {
        //std::lock_guard<std::mutex> lock(mutexTT);
        //x modulo y = (x & (y − 1)) if y is a power of 2
        unsigned long long ttsizeNb = TTMbToSize<Bucket>(Definitions::ttConfig.ttSize);
        Util::NextPowerOf2(ttsizeNb);
        Definitions::ttConfig.ttSize = ttsizeNb; // convert Mb ttSize to number of bucket ttSize
        LOG(logINFO) << "Adapted TT size " << Definitions::ttConfig.ttSize;
        LOG(logINFO) << "Size of TT " << int(Definitions::ttConfig.ttSize * sizeof(Bucket) / 1024. / 1024.) << "Mo";
        ttable = new Bucket[(int)Definitions::ttConfig.ttSize];
    }
}

void Transposition::InitTTQ() {

    LOG(logINFO) << "Init TTQ";

    if (ttableQ == 0) {
        //std::lock_guard<std::mutex> lock(mutexTTQ);
        //x modulo y = (x & (y − 1)) if y is a power of 2
        unsigned long long ttsizeNb = TTMbToSize<Bucket>(Definitions::ttConfig.ttQSize);
        Util::NextPowerOf2(ttsizeNb);
        Definitions::ttConfig.ttQSize = ttsizeNb; // convert Mb ttSize to number of bucket ttSize
        LOG(logINFO) << "Adapted QTT size " << Definitions::ttConfig.ttQSize;
        ttableQ = new Bucket[(int)Definitions::ttConfig.ttQSize];
        LOG(logINFO) << "Size of QTT " << int(Definitions::ttConfig.ttQSize * sizeof(Bucket) / 1024. / 1024.) << "Mo";
    }
}

void Transposition::InsertTT(const TTHASHTYPE &     zhash,
                             ScoreType              score,
                             DepthType              depth,
                             Transposition::TT_Type t_type,
                             const TTMOVETYPE &     m
#ifdef DEBUG_TT_SORT_FAIL
                            ,const std::string &    fen
#endif
                             ) {

    if ( m == 0 ){
        LOG(logFATAL) << "Trying to insert invalid move inside TT";
    }

    if (depth >= MAX_SEARCH_DEPTH) {
        LOG(logFATAL) << "depth >= MAX_SEARCH_DEPTH " << depth;
    }

    if (zhash == 0) {
        LOG(logFATAL) << "Zero hash";
    }

    //InitTT();

    std::lock_guard<std::mutex> lock(mutexTT);

    Bucket & bucket = ttable[zhash&(Definitions::ttConfig.ttSize-1)];
    Transposition & tReplaceAlways = bucket.t[0];
    Transposition & tReplaceDepth  = bucket.t[1];
    if ( tReplaceAlways.hash != 0 ){
        tReplaceAlways.depth  = depth;
        tReplaceAlways.score  = score;
        tReplaceAlways.t_type = t_type;
        tReplaceAlways.move   = m;
        tReplaceAlways.hash   = zhash;
#ifdef DEBUG_TT_SORT_FAIL
        tReplaceAlways.fen    = fen;
#endif
        tReplaceAlways.age    = ta_new;

        if ( tReplaceDepth.depth <= depth       // update if depth is bigger
             || tReplaceDepth.hash != zhash     // or if hash collision
             || tReplaceDepth.age == ta_elder){ // or if old entry
            tReplaceDepth = tReplaceAlways;
        }
    }
    else{ // add in TT
        tReplaceAlways.depth  = depth;
        tReplaceAlways.score  = score;
        tReplaceAlways.t_type = t_type;
        tReplaceAlways.move   = m;
        tReplaceAlways.hash   = zhash;
#ifdef DEBUG_TT_SORT_FAIL
        tReplaceAlways.fen    = fen;
#endif
        tReplaceAlways.age    = ta_new;

        tReplaceDepth = tReplaceAlways;
    }


}

// I think using the TT may change the result because move sorting shall differ.
bool Transposition::GetTT(const TTHASHTYPE & zhash,
                          DepthType          mindepth,
                          ScoreType          alpha,
                          ScoreType          beta,
                          Transposition &    tt) {

    //InitTT();

    std::lock_guard<std::mutex> lock(mutexTT);

    if (zhash == 0) {
        LOG(logFATAL) << "Zero hash";
    }

    Bucket & bucket = ttable[zhash&(Definitions::ttConfig.ttSize-1)];

    for(unsigned char bid = 0 ; bid < 2 ; ++bid){

        const Transposition & t = bucket.t[bid];

        if ( t.hash == 0 ){
            // we can return immediatly
            // because if bid==0, replace always with 0 hash has never been initialized
            // and if bid==1, no more bucket (yet) ...
            return false;
        }

        if ( t.hash != zhash ){
            bid==0?++Stats::ttARCollision:++Stats::ttCollision;
            //LOG(logWARNING) << "TT collision " << t.hash << " " << zhash << " " << (t.hash&(Definitions::ttSize - 1)) << " " << (zhash&(Definitions::ttSize - 1));
            //return false;
            continue; // goto next bucket
        }

        // does avoiding old entries helps avoiding real collisions ?
        /*
        if ( t.age == ta_elder ){
            //return false;
            continue; // goto next bucket
        }*/

        // assign move in all cases (for move sorting)
        tt = t;

        if ( t.move == 0 || tt.move == 0){
           LOG(logFATAL) << "A 0 hash move is present in the TT " << t.hash << " " << t.depth << " " << t.score << " " << t.move << " " << tt.move;
        }

        if ( tt.depth >= mindepth){
            ++Stats::ttAllHit;
            switch(tt.t_type){
            case Transposition::tt_exact:
                bid==0?++Stats::ttARHitExact:++Stats::ttHitExact;
                if ( std::fabs(tt.score) > Definitions::scores.checkMateScore - MAX_GAME_PLY ){
                    ///@todo return a corrected checkmate score !!!
                    return false;
                }
                return true;
            case Transposition::tt_alpha:
                if ( tt.score <= alpha){
                    bid==0?++Stats::ttARHitAlpha:++Stats::ttHitAlpha;
                    tt.score = alpha;
                    return true;
                }
                else{
                    bid==0?++Stats::ttARHitAlphaOver:++Stats::ttHitAlphaOver;
                    return false;
                }
            case Transposition::tt_beta:
                if ( tt.score >= beta){
                    bid==0?++Stats::ttARHitBeta:++Stats::ttHitBeta;
                    tt.score = beta;
                    return true;
                }
                else{
                    bid==0?++Stats::ttARHitBetaOver:++Stats::ttHitBetaOver;
                    return false;
                }
            }
        }
    }

    return false;
}

void Transposition::InsertTTQ(const TTHASHTYPE &     zhash,
                              ScoreType              score,
                              DepthType              depth,
                              Transposition::TT_Type t_type,
                              const TTMOVETYPE &     m
#ifdef DEBUG_TT_SORT_FAIL
                             ,const std::string &    fen
#endif
                             ) {

    if ( m == 0 ){
        LOG(logFATAL) << "Trying to insert invalid move inside TT";
    }

    if (depth >= MAX_SEARCH_DEPTH + Definitions::selectivity.quies_max) {
        LOG(logFATAL) << "depth >= MAX_SEARCH_DEPTH " << depth;
    }

    if (zhash == 0) {
        LOG(logFATAL) << "Zero hash";
    }

    std::lock_guard<std::mutex> lock(mutexTTQ);

    //InitTTQ();

    Bucket & bucket = ttableQ[zhash&(Definitions::ttConfig.ttQSize-1)];
    Transposition & tReplaceAlways = bucket.t[0];
    Transposition & tReplaceDepth  = bucket.t[1];
    if ( tReplaceAlways.hash != 0 ){
        tReplaceAlways.depth  = depth;
        tReplaceAlways.score  = score;
        tReplaceAlways.t_type = t_type;
        tReplaceAlways.move   = m;
        tReplaceAlways.hash   = zhash;
#ifdef DEBUG_TT_SORT_FAIL
        tReplaceAlways.fen    = fen;
#endif
        tReplaceAlways.age    = ta_new;

        if ( tReplaceDepth.depth <= depth       // update if depth is bigger
             || tReplaceDepth.hash != zhash     // or if hash collision
             || tReplaceDepth.age == ta_elder){ // or if old entry
            tReplaceDepth = tReplaceAlways;
        }
    }
    else{ // add in TT
        tReplaceAlways.depth  = depth;
        tReplaceAlways.score  = score;
        tReplaceAlways.t_type = t_type;
        tReplaceAlways.move   = m;
        tReplaceAlways.hash   = zhash;
#ifdef DEBUG_TT_SORT_FAIL
        tReplaceAlways.fen    = fen;
#endif
        tReplaceAlways.age    = ta_new;

        tReplaceDepth = tReplaceAlways;
    }
}

// I think using the TT may change the result because move sorting shall differ.
bool Transposition::GetTTQ(const TTHASHTYPE & zhash,
                           DepthType          mindepth,
                           ScoreType          alpha,
                           ScoreType          beta,
                           Transposition &    tt) {

    std::lock_guard<std::mutex> lock(mutexTTQ);

    //InitTTQ();

    if (zhash == 0) {
        LOG(logFATAL) << "Zero hash";
    }

    Bucket & bucket = ttableQ[zhash&(Definitions::ttConfig.ttQSize-1)];

    for(unsigned char bid = 0 ; bid < 2 ; ++bid){

        const Transposition & t = bucket.t[bid];

        if ( t.hash == 0 ){
            // we can return immediatly
            // because if bid==0, replace always with 0 hash has never been initialized
            // and if bid==1, no more bucket (yet) ...
            return false;
        }
        if ( t.hash != zhash ){
            bid==0?++Stats::ttARQCollision:++Stats::ttQCollision;
            //return false;
            continue; // goto next bucket
        }

        // assign move in all cases (for move sorting)
        tt = t;

        if ( tt.depth >= mindepth){
            ++Stats::ttQAllHit;
            switch(tt.t_type){
            case Transposition::tt_exact:
                bid==0?++Stats::ttARQHitExact:++Stats::ttQHitExact;
                if ( std::fabs(tt.score) > Definitions::scores.checkMateScore - MAX_GAME_PLY ){
                    ///@todo return a corrected checkmate score !!!
                    return false;
                }
                return true;
            case Transposition::tt_alpha:
                if ( tt.score <= alpha){
                    bid==0?++Stats::ttARQHitAlpha:++Stats::ttQHitAlpha;
                    tt.score = alpha;
                    return true;
                }
                else{
                    bid==0?++Stats::ttARQHitAlphaOver:++Stats::ttQHitAlphaOver;
                    return false;
                }
            case Transposition::tt_beta:
                if ( tt.score >= beta){
                    bid==0?++Stats::ttARQHitBeta:++Stats::ttQHitBeta;
                    tt.score = beta;
                    return true;
                }
                else{
                    bid==0?++Stats::ttARQHitBetaOver:++Stats::ttQHitBetaOver;
                    return false;
                }
            }
        }
    }
    return false;
}


bool Transposition::Sort(const Transposition & ttt, FastContainer<Move> & moves, const Position & p) {
    if ( moves.empty()){
        LOG(logFATAL) << "Move list is empty ... cannot sort ...";
    }
    Move ttmove(Move::HashToMini(ttt.move));
    auto it = std::find(moves.begin(), moves.end(), ttmove);
    if (it != moves.end() ) {
        moves.moveToFront(it - moves.begin());
        moves.front().SetSortScore(moves.front().SortScore() + Definitions::scores.ttSortScore);
        return true;
    }
    LOG(logERROR) << "TT sort failed : " << p.GetFEN() << " " << ttt.move << " " << ttmove.Show() << " " << ttmove.Type();
    UtilMove::DisplayList(moves, "Move list");
#ifdef DEBUG_TT_SORT_FAIL
    LOG(logINFO) << "Inserted for position " << ttt.fen;
#endif
    // this move is not good (collision)
    return false;
}

void UpdateAge_(Bucket * e, unsigned long long int n){
    if ( ! e ) return;
    for(unsigned long long int k = 0 ; k < n ; ++k){
        // update age only for second entry, not for always replace entry
        e[k].t[1].age = (Transposition::TT_Aging)(std::min(e[k].t[1].age + 1,int(Transposition::ta_max)));
    }
}

void Clear_(Bucket * e, unsigned long long int n){
    if ( ! e ) return;
    /*
    for( unsigned long long int k = 0 ; k < n ; ++k){
        e[k].t[0].hash = 0;
        e[k].t[1].hash = 0;
    }
    */
    memset(e,0,sizeof(Bucket)*n);
}

std::string Transposition::GetPV(const Position & pp, int depth, bool abr){
    Position p(pp);
    std::stringstream ss;
    Transposition ttt;
    Game game;
    game.SetHash(p);

    int count = 0;

    while( true ){
        //LOG(logINFO) << "PV TT " << p.GetFEN();

        static Analyse::EvalCache c;

        if (Searcher::IsForcedDraw(game, p, c, 3, false)) {
            ss << "(tt draw)";
            break;
        }

        ttt.hash = 0;
        bool ttok = Transposition::GetTT(PHASH(p),1,Definitions::scores.infScore,-Definitions::scores.infScore,ttt);
        //LOG(logINFO) << "PV TT " << ttt.move;
        if ( ttt.hash == 0 ){
            // no more moves inside TT
            //ss << "(tt no move)";
            break;
        }

        /*
        if ( !ttok ){
            //ss << "(tt depth)";
            break;
        }
        */

        FastContainer<Move> moves;
        MoveGenerator(true).Generator(p,moves);
        if ( moves.empty() ){
            // no more moves ... (checkmate or stalemate)
            //ss << "(tt checkmate or stalemate)";
            break;
        }
        // find the move
        bool found = false;
        Move & m = moves[0];
        for(size_t k = 0 ; k < moves.size() && !found; ++k){
            if ( moves[k].ZHash() == ttt.move){
               m = moves[k];
               found = true;
            }
        }
        if ( ! found ){
            LOG(logFATAL) << "TT move not found ! " << p.GetFEN() << " " << Move(Move::HashToMini(ttt.move)).Show();
            break;
        }

        //LOG(logINFO) << "PV TT " << m.ShowAlgAbr(p);

        ss << (abr?m.ShowAlgAbr(p):m.ShowAlg(p,false)) << " ";

#ifdef DEBUG_PV_TT
#ifdef DEBUG_PV_TT_A_LOT
        LOG(logINFO) << k << " ==== " << p.GetFEN() << " " << m.Show();
#endif
        bool ok;
        m.ApplyInPlace(p,&ok);
        game.SetHash(p,-1,m.IsCapture());
        if ( !ok ){
            LOG(logERROR) << "Error in PV TT! (maybe a real hash collision ...)" << p.GetFEN() << " " << m.Show();
        }
#else
        p.ApplyMove(m,true);
        game.SetHash(p,-1,m.IsCapture());
#endif

        ++count;

        if ( count >= MAX_SEARCH_DEPTH){
            //ss << "(max ply)";
            break;
        }

        if ( count > depth){
            //ss << "(max depth)";
            break;
        }

    }

    if (count == 0) {
        LOG(logWARNING) << "Sorry first move in PV is not in the TT anymore";
    }

    // debug
    //searcher.game.Display();

    return ss.str();
}

void Transposition::UpdateAge() {
    std::lock_guard<std::mutex> lock(mutexTT);
    UpdateAge_(ttable, Definitions::ttConfig.ttSize );
}

void Transposition::UpdateAgeQ(){
    std::lock_guard<std::mutex> lock(mutexTTQ);
    ++Stats::ttClear;
    UpdateAge_(ttableQ,Definitions::ttConfig.ttQSize);
}

void Transposition::ClearTT(){
    std::lock_guard<std::mutex> lock(mutexTT);
    ++Stats::ttQClear;
    Clear_(ttable, Definitions::ttConfig.ttSize );
}

void Transposition::ClearTTQ(){
    std::lock_guard<std::mutex> lock(mutexTT);
    Clear_(ttableQ, Definitions::ttConfig.ttQSize );
}
