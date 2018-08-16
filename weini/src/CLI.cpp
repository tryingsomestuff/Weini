#include "CLI.h"

#include "sort.h"
#include "line.h"
#include "UtilPosition.h"
#include "stats.h"

#include <sstream>
#include <fstream>
#include <iterator>
#include <thread>
#include <chrono>

void GeneratorWrapperLoop(std::vector<Stats::NodeCount> & nodesV,
                          std::vector<NodeStatType> & perft,
                          Position & p,
                          const FastContainer<Move> & moves,
                          DepthType currentDepth,
                          bool display,
                          bool pseudolegal){

   int g = 0;
   for (auto it = moves.begin() ; it != moves.end() ; ++it){
      if ( display ) {
         LOG(logINFO) <<  perft.size()-2-currentDepth << "         " <<  (*it).Show();
      }
      Position p2(p);
      // No check on move !
      p2.ApplyMove(*it);
      FastContainer<Move> moves2;
      //ScopePoolObject<FastContainer<Move>, _default_block_size> moves_;
      //FastContainer<Move> & moves2 = moves_.Get();
      MoveGenerator(!pseudolegal,false,true).Generator(p2,moves2,&nodesV[perft.size()-1-currentDepth]);
      g += (int)moves2.size();
      if ( currentDepth > 0 ) GeneratorWrapperLoop(nodesV,perft,p2,moves2,currentDepth-1,display,pseudolegal);
   }

   perft[perft.size()-1-currentDepth] += g;
}

std::vector<NodeStatType> GeneratorWrapper(const std::string & fen,
                                            DepthType depth,
                                            bool display,
                                            bool pseudolegal){
  Position p(fen);

  if (display) p.Display();

  std::vector<NodeStatType> ret(depth+1,0);
  std::vector<Stats::NodeCount> nodesV(depth+1,Stats::NodeCount());
  FastContainer<Move> moves;
  MoveGenerator(!pseudolegal, false, true).Generator(p,moves,&nodesV[0]);
  ret[0] = moves.size();
  if ( depth > 0){
     GeneratorWrapperLoop(nodesV,ret,p,moves,depth-1,display,pseudolegal);
  }

  int k = 0;
  for(auto it = ret.begin() ; it != ret.end() ; ++it,++k){
      LOG(logINFO) << "=== Generation " << k  << " " << *it;
      nodesV[k].Print();
  }

  return ret;

}

bool TestGenerator(const std::string & fen,
                   std::vector<NodeStatType> expected,
                   DepthType depth,
                   bool display,
                   bool pseudolegal){

  LOG(logINFO) << "Testing generator for " << fen << " depth " << (int)depth;

  MoveGenerator::stats.Probe();

  std::vector<NodeStatType> l = GeneratorWrapper(fen,depth,display,pseudolegal);

  MoveGenerator::stats.Probe();

  bool ok = true;

  for(DepthType k = 0 ; k < depth && k < (DepthType)expected.size(); ++ k){
    if ( expected[k] != l[k]){
      LOG(logFAIL) << "Error for generator, expected " << expected[k] << " got " << l[k];
      ok = false;
    }
  }

  if ( ok && !expected.empty()){
    LOG(logINFO) << "=> OK";
  }

  if ( expected.empty() ){
    LOG(logWARNING) << "No expected given";
  }

  return ok;

}

bool TestThreats(const std::string & fen,
                 const std::string & square,
                 std::vector<Square> expected,
                 bool display){
    Square sq(square);
    Position p(fen);

    LOG(logINFO) << "Testing threats for " << fen << " on square " << square;

    if ( display ) p.Display();
    FastContainer<Square::LightSquare> threats;
    UtilMove::GetThreads(p,sq.index(),false,&threats);
    //if ( display ){
      if ( threats.empty()){
         LOG(logINFO) << "No threats for " << sq.position() << " from " << (p.WhiteToPlay()? "black":"white");
      }
      else{
         LOG(logINFO) << "Threats for " << sq.position() << " from " << (p.WhiteToPlay()? "black":"white");
         for(auto it = threats.begin() ; it != threats.end() ; ++it){
             LOG(logINFO) << Square(*it).position();
         }
      }
    //}

    bool ok = true;

    if ( expected.size() != threats.size() ){
      ok = false;
      LOG(logFAIL) << "Number of threats differ : " << expected.size() << " != " << threats.size();
    }

    for ( size_t k = 0 ; k < expected.size() ; ++k){
        if ( std::find(threats.begin(), threats.end(), expected[k].index()) == threats.end()){
           ok = false;
        }
    }

    if ( ok){
      LOG(logINFO) << "=> OK";
    }
    else{
      LOG(logFAIL) << "=> BAD";
    }

    return ok;
}

bool TestMoveHash(){
    Position p("start");
    {
        std::cout << "**********************" << std::endl;
        Move m("g8 h8", p.Turn());
        std::cout << m.Show() << std::endl;
        std::cout << "from " << (int)m.From() << std::endl;
        std::cout << "to   " << (int)m.To() << std::endl;
        std::cout << "type " << (int)m.Type() << std::endl;
        Move::HashType h = m.ZHash();
        std::cout << h << std::endl;
        std::cout << std::hex << (m.From() << 10) << std::endl;
        std::cout << std::hex << (m.To() << 4) << std::endl;
        std::cout << std::hex << (m.Type()) << std::endl;
        std::cout << std::hex << h << std::endl;
        MiniMove mini = Move::HashToMini(h);
        std::cout << "h from " << (int)Move::Hash2From(h) << std::endl;
        std::cout << "h to   " << (int)Move::Hash2To(h) << std::endl;
        std::cout << "h type " << (int)Move::Hash2Type(h) << std::endl;
        Move m2(mini);
        std::cout << m2.Show() << std::endl;
        if (m2.Show() != m.Show()) {
            return false;
        }
    }
    {
        std::cout << "**********************" << std::endl;
        Move m("0-0-0", p.Turn());
        std::cout << m.Show() << std::endl;
        std::cout << "from " << (int)m.From() << std::endl;
        std::cout << "to   " << (int)m.To() << std::endl;
        std::cout << "type " << (int)m.Type() << std::endl;
        Move::HashType h = m.ZHash();
        std::cout << h << std::endl;
        std::cout << std::hex << (m.From() << 10) << std::endl;
        std::cout << std::hex << (m.To() << 4) << std::endl;
        std::cout << std::hex << (m.Type()) << std::endl;
        std::cout << std::hex << h << std::endl;
        MiniMove mini = Move::HashToMini(h);
        std::cout << "h from " << (int)Move::Hash2From(h) << std::endl;
        std::cout << "h to   " << (int)Move::Hash2To(h) << std::endl;
        std::cout << "h type " << (int)Move::Hash2Type(h) << std::endl;
        Move m2(mini);
        std::cout << m2.Show() << std::endl;
        if (m2.Show() != m.Show()) {
            return false;
        }
    }
    return true;
}

ScoreType TestStaticAnalysis(const std::string & fen){
    Position pAnal(fen);
    pAnal.Display();
    Analyse::ActivateAnalysisDisplay(true);
    return Analyse().Run(pAnal,1.f);
}

SearcherBase::SearchedMove TestAnalysis(Position & pAnal, DepthType depth, bool display){
    //if ( display) pAnal.Display();
    SearcherBase::Data _data(pAnal,depth,true,display,Definitions::debugConfig.modedebug);
    TimeMan::Instance().Tic();
    LOG(logINFO) << "Next search time " << Definitions::timeControl.currentMoveMs;
    SearcherBase::SearchedMove best = ThreadPool<Searcher>::Instance().SearchSync(_data);
    Line pv = ThreadPool<Searcher>::Instance().Main().GetData().pv;
    //if ( display ) pAnal.Display();
    LOG(logINFO) << "Best move is " << SearcherBase::GetMove(best).Show()
                 << " with score " << (pAnal.Turn() == NSColor::c_white ? +1 : -1 ) * SearcherBase::GetScore(best)
                 << " (depth " << (int)SearcherBase::GetDepth(best) << "/" << (int)depth << ")";
    if ( pv.n > 0 ){
       LOG(logINFO) << "PV:     " << pv.GetPV(pAnal,true);
    }
    // Getting the PV using TT requieres games hashes to check for draw
    if ( display ) Stats::CutCount();
    return best;

}

SearcherBase::SearchedMove TestAnalysis(const std::string & fen, DepthType depth, bool display){
    Position pAnal(fen);
    pAnal.Display();
    return TestAnalysis(pAnal,depth,display);
}

void TestMoveOrdering(const std::string & fen){
   Position p(fen);

   p.Display();

   FastContainer<Move> moves;
   MoveGenerator().Generator(p,moves);
   UtilMove::DisplayList(moves,"Unsorted move list");
   Searcher s;
   Sort::SortMoves(moves,p,s,Sort::SP_all);
   UtilMove::DisplayList(moves,"Sorted move list");

}

void TestSEE(const std::string & fen, const std::string & square, ScoreType threshold){

    Position p(fen);

    p.Display();

    Square sq(square);
    LOG(logINFO) << "SSE at square " << sq.position();
    LOG(logINFO) << "Threshold value " << threshold;
    p.SwitchPlayer();
    FastContainer<Square::LightSquare> threats;
    UtilMove::GetThreads(p, sq.index(), false, &threats);
    p.SwitchPlayer();
    Move m(threats[0], sq.index());
    m.Validate(p, true, true);
    LOG(logINFO) << "SSE " << (Searcher::SEE(p,m,threshold)?"true":"false");
}

#define GETHASH \
h1 = p.GetZHash(); //\
//h1w = p.GetZHash(NSColor::c_white); \
//h1b = p.GetZHash(NSColor::c_black);

#define COMPAREHASH \
h2 = p.GetZHash(); \
if (h1  != h2 ) { LOG(logFAIL) << "bad hash  " << h1  << " " << h2  << " " << p.GetFEN(); }
//h2w = p.GetZHash(NSColor::c_white); \
//h2b = p.GetZHash(NSColor::c_black); \
//if (h1w != h2w) { LOG(logFAIL) << "bad whash " << h1w << " " << h2w << " " << p.GetFEN(); } \
//if (h1b != h2b) { LOG(logFAIL) << "bad bhash " << h1b << " " << h2b << " " << p.GetFEN(); }



void TestGameZHash(){

  Position p("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");

  /*
  LOG(logINFO) << Util::Count(p) ;

  LOG(logINFO) << (p.WhiteToPlay()?"White":"Black") << " to play" ;

  LOG(logINFO) << p.Info().white.CastlingRight() ;
  LOG(logINFO) << p.Info().black.CastlingRight() ;

  Square ep(p.Info().enPassant);
  if ( ep.IsValid()){
    LOG(logINFO) << ep.position();
    LOG(logINFO) << (int)ep.file();
    LOG(logINFO) << ep.fileStr();
    LOG(logINFO) << (int)ep.rank() ;
  }
  else{
    LOG(logINFO) << "No current en passant" ;
  }
  */

  p.Display();

  Util::Zobrist::HashType h1, h2;
  //Util::Zobrist::HashType h1w, h1b, h2w, h2b;

  // should fail "not your turn"
  Move("d5 d4", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // test capture with pawn
  Move("e4 e5", p.Turn()).ValidateAndApplyInPlace(p).Display();

  GETHASH;

  // bishop forward
  Move("c8 d7", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop forward
  Move("f1 e2", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop backward
  Move("d7 c8", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop backward
  Move("e2 f1", p.Turn()).ValidateAndApplyInPlace(p).Display();

  COMPAREHASH;

  p = Position("r3kbnr/pppqpppp/2npb3/8/8/2NBPN2/PPPP1PPP/R1BQK2R w KQkq - 6 5");

  Move("0-0"  , p.Turn()).ValidateAndApplyInPlace(p).Display();
  Move("0-0-0", p.Turn()).ValidateAndApplyInPlace(p).Display();

  GETHASH;

}

void TestGamePlay(){

  Position p("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d5 0 2");

  LOG(logINFO) << Util::Count(p,1) ;

  LOG(logINFO) << (p.WhiteToPlay()?"White":"Black") << " to play" ;

  LOG(logINFO) << p.Info().white.CastlingRight() ;
  LOG(logINFO) << p.Info().black.CastlingRight() ;

  Square ep(p.Info().enPassant);
  if ( ep.IsValid()){
    LOG(logINFO) << ep.position();
    LOG(logINFO) << (int)ep.file();
    LOG(logINFO) << ep.fileStr();
    LOG(logINFO) << (int)ep.rank() ;
  }
  else{
    LOG(logINFO) << "No current en passant" ;
  }

  p.Display();

  // should fail "not your turn"
  Move m0("d5 e4",p.Turn());
  p = m0.ValidateAndApply(p);
  p.Display();

  // test capture with pawn
  Move m1("e4 d5",p.Turn());
  p = m1.ValidateAndApply(p);
  p.Display();

  // generate possible en passant
  Move m2("e7 e5",p.Turn());
  p = m2.ValidateAndApply(p);
  p.Display();

  // capture en passant
  Move m3("d5 e6",p.Turn());
  p = m3.ValidateAndApply(p);
  p.Display();

  // should fail : invalid piece in the way
  Move m4("d8 a5",p.Turn());
  p = m4.ValidateAndApply(p);
  p.Display();

  // should fail : invalid wrong direction
  Move m5("d8 h6",p.Turn());
  p = m5.ValidateAndApply(p);
  p.Display();

  // capture with bishop
  Move m6("c8 e6",p.Turn());
  p = m6.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m7("g1 f3",p.Turn());
  p = m7.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m8("g8 f6",p.Turn());
  p = m8.ValidateAndApply(p);
  p.Display();

  // move bishop
  Move m9("f1 d3",p.Turn());
  p = m9.ValidateAndApply(p);
  p.Display();

  // should fail : piece in the way
  Move m10("f8 h6",p.Turn());
  p = m10.ValidateAndApply(p);
  p.Display();

  // move bishop
  Move m11("f8 d6",p.Turn());
  p = m11.ValidateAndApply(p);
  p.Display();

  // white castle king side
  Move m12("0-0",p.Turn());
  p = m12.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m13("b8 d7",p.Turn());
  p = m13.ValidateAndApply(p);
  p.Display();

  // pawn
  Move m14("b2 b3",p.Turn());
  p = m14.ValidateAndApply(p);
  p.Display();

  // black queen
  Move m15("d8 e7",p.Turn());
  p = m15.ValidateAndApply(p);
  p.Display();

  // white fianquetto
  Move m16("c1 b2",p.Turn());
  p = m16.ValidateAndApply(p);
  p.Display();

  // black castle queen side
  Move m17("0-0-0",p.Turn());
  p = m17.ValidateAndApply(p);
  p.Display();

  // knight
  Move m18("b1 c3",p.Turn());
  p = m18.ValidateAndApply(p);
  p.Display();

  // attack with bishop : check
  Move m19("d6 h2",p.Turn());
  p = m19.ValidateAndApply(p);
  p.Display();

  // should fail, does not remove check
  Move m20("a2 a3",p.Turn());
  p = m20.ValidateAndApply(p);
  p.Display();

  // remove check, take bishop with king
  Move m21("g1 h2",p.Turn());
  p = m21.ValidateAndApply(p);
  p.Display();

  // check with queen now
  Move m22("e7 d6",p.Turn());
  p = m22.ValidateAndApply(p);
  p.Display();

  // remove check with pawn
  Move m23("g2 g3",p.Turn());
  p = m23.ValidateAndApply(p);
  p.Display();
}

std::deque<SearcherBase::SearchedMove> Play(const std::string & fen, DepthType depth, bool AIVsAI){
    Position p(fen);
    //p.Display();

    std::deque<SearcherBase::SearchedMove> game;
    std::string strMove;
    FastContainer<Move> moves;

    Game gamehash;
    gamehash.SetHash(p);
    Analyse::EvalCache c;

    while(true){

       // already mate or stalemate ?
       MoveGenerator(true,false,false).Generator(p,moves);
       if ( moves.empty() ){
           if ( p.IsInCheck()){
              LOG(logINFO) << "Result :: Checkmate " << (p.WhiteToPlay()?"0-1":"1-0");
           }
           else{
              LOG(logINFO) << "Result :: Stalemate " << "1/2-1/2";
           }
           break;
       }
       // is it a draw ?
       if ( Searcher::IsForcedDraw(gamehash,p,c,3,true) ){
           LOG(logINFO) << "Result :: Can be a draw because of " << (p.WhiteToPlay()?"white":"black") << " 1/2-1/2";
           return game;
       }
       /*
       if ( AIVsAI && fabs(Util::Count(p)) > 2000){
           LOG(logINFO) << "Play :: One player is lost";
           return game;
       }
       */
       p.Display();
       // first player
       if ( AIVsAI ){
           LOG(logINFO) << "Play :: AI1 turn";
           bool moveOK = false;
           SearcherBase::SearchedMove best = TestAnalysis(p,depth,true);
           Move & m = SearcherBase::GetMove(best);
           LOG(logINFO) << "Play :: AI1 plays " << m.ShowAlgAbr(p);
           p = m.ValidateAndApply(p,&moveOK);
           gamehash.SetHash(p,-1,m.IsCapture());
           if ( ! moveOK ){
               LOG(logERROR) << "Play :: Bad computer move ! " << m.Show() ;
               break;
           }
           game.push_back(best);
           Util::DisplayPGN(fen,game);
       }
       else{
           LOG(logINFO) << "Play :: Your move";
           bool moveOK = false;
           while ( ! moveOK ){
              std::getline(std::cin, strMove);
              Move m(strMove,p.Turn());
              p = m.ValidateAndApply(p,&moveOK);
              if ( moveOK){
                  gamehash.SetHash(p,-1,m.IsCapture());
                  game.push_back(SearcherBase::SearchedMove(m,-9999,-2));
                  LOG(logINFO) << "Play :: you play " << m.Show();
              }
           }
       }

       // write receipt
       if ( false ){
           std::ofstream str;
           str.open("js/test.json");
           str << Util::JsonPGN(Position(Position::startPosition).GetFEN(),game);
           str.close();
       }

       // already mate or stalemate ?
       MoveGenerator(true,false,false).Generator(p,moves);
       if ( moves.empty() ){
           if ( p.IsInCheck()){
              LOG(logINFO) << "Result :: Checkmate " << (p.WhiteToPlay()?"0-1":"1-0");;
           }
           else{
              LOG(logINFO) << "Result :: Stalemate " << "1/2-1/2";
           }
           break;
       }
       // is it a draw ?
       if ( Searcher::IsForcedDraw(gamehash,p,c,3,true) ){
          LOG(logINFO) << "Result :: Can be a draw because of " << (p.WhiteToPlay()?"white":"black") << " 1/2-1/2";
          return game;
       }
       /*
       if ( AIVsAI && fabs(Util::Count(p)) > 2000){
           LOG(logINFO) << "One player is lost";
           return game;
       }
       */
       // second player
       LOG(logINFO) << "Play :: AI2 turn";
       bool moveOK = false;
       p.Display();
       SearcherBase::SearchedMove best = TestAnalysis(p,depth,true);
       Move & m = SearcherBase::GetMove(best);
       LOG(logINFO) << "Play :: AI2 plays " << m.ShowAlgAbr(p);
       p = m.ValidateAndApply(p,&moveOK);
       gamehash.SetHash(p,-1,m.IsCapture());
       if ( ! moveOK ){
           LOG(logERROR) << "Play :: Bad computer move ! " << m.Show() ;
           break;
       }
       game.push_back(best);
       Util::DisplayPGN(fen,game);

       // write receipt
       if ( false ){
           std::ofstream str;
           str.open("js/test.json");
           str << Util::JsonPGN(Position(Position::startPosition).GetFEN(),game);
           str.close();
       }

    }

    return game;
}
