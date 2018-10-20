#include "com.h"
#include "search.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "ttable.h"
#include "book.h"
#include "line.h"
#include "sort.h"
#include "stats.h"

#include <iterator>
#include <iomanip>

volatile std::atomic_bool Searcher::stopFlag;

volatile Searcher::eVerbosity Searcher::verbosity = Searcher::eVerb_standard;

DepthType Searcher::lmr_reduction[MAX_REDUCTION_DEPTH] = { 0,1,2,3,4,5,6,7,8,9 };

namespace {
    void ManageExtensionRoot(DepthType & depth, const Position & previousP, const Position & currentP,
                             const Move & move, const FastContainer<Move> & moves, bool isPV, Searcher & searcher) {

        unsigned short int nbExtension = 0;

        Square::LightSquare lastCaptureSquare = searcher.game.Info(currentP.CurrentPly()).lastMoveWasCapture() ?
                                                             searcher.game.Info(currentP.CurrentPly()).lastMove.to
                                                           : Square::Invalide;

        if (Definitions::extensionReductionConfig.do_EndGameExtensionSearch
            && previousP.IsEndGame()) {
            ++depth;
            OPTIM_STATS(++Stats::countEndGameExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_VeryEndGameExtensionSearch
            && previousP.IsVeryEndGame()) {
            ++depth;
            OPTIM_STATS(++Stats::countVeryEndGameExtensionSearch;)
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_CheckExtensionSearch
            && move.IsCheck()) {
            ++depth;
            OPTIM_STATS(++Stats::countCheckExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_PVExtensionSearch
            && isPV) {
            ++depth;
            OPTIM_STATS(++Stats::countPVExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_SingleReplyExtensionSearch
            && moves.size() == 1) {
            ++depth;
            OPTIM_STATS(++Stats::countSingleReplyExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_ReCaptureExtensionSearch
            && isPV
            && Square::IsValid(lastCaptureSquare)
            && move.To() == lastCaptureSquare) {
            ++depth;
            OPTIM_STATS(++Stats::countReCaptureExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_CheckExtensionRoot
            && previousP.IsInCheck()) {
            ++depth;
            OPTIM_STATS(++Stats::countCheckExtensionRoot;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_NearPromotionExtensionSearch
            && Move::IsNearPromotion(previousP, searcher.game.Info(previousP.CurrentPly()).lastMove.to)) {
            ++depth;
            OPTIM_STATS(++Stats::countNearPromotionExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
        if (Definitions::extensionReductionConfig.do_NearPromotionExtensionSearch
            && Move::IsNearPromotion(currentP, searcher.game.Info(previousP.CurrentPly()).lastMove.to)) {
            ++depth;
            OPTIM_STATS(++Stats::countNearPromotionExtensionSearch;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionRootMax) return;
        }
    }

    void DisplayPV(const Position & p, Line * pv, SearcherBase::SearchedMove best, DepthType iterativeDeepeningDepth, const std::string & mes = ""){
        std::string pvstr = SearcherBase::GetMove(best).ShowAlg(p,false);
        if (pv /*&& ! (Definitions::ttConfig.do_transpositionTableSortAlphaBeta || Definitions::ttConfig.do_transpositionTableAlphaBeta)*/) {
           std::string pvstr2 = pv->GetPV(p);
           pvstr = pvstr2.empty() ? pvstr : pvstr2;
        }
        else if (Definitions::ttConfig.do_transpositionTableSortAlphaBeta || Definitions::ttConfig.do_transpositionTableAlphaBeta){
           std::string pvstr2 = Transposition::GetPV(p,SearcherBase::GetDepth(best),false);
           pvstr = pvstr2.empty() ? pvstr : pvstr2;
        }
        float sec = TimeMan::Instance().ElapsedTic();
        std::string str = Com::DisplayInfo(SearcherBase::GetDepth(best) + 1,
                                           Stats::seldepth[iterativeDeepeningDepth],
                                           SearcherBase::GetScore(best),
                                           sec,
                                           Stats::currentNodesCount + Stats::currentqNodesCount,
                                           Stats::ttHitExact + Stats::ttHitAlpha + Stats::ttHitBeta,
                                           pvstr);
        SearcherBase::TryDisplay(SearcherBase::GetDepth(best), str + " " + mes, false);
    }

    bool ThreadSkipDepth(const Searcher & searcher, const Position & p, DepthType iterativeDeepeningDepth){
        // =====================
        // Sizes and phases of the skip-blocks,
        // used for distributing search depths across the threads
        // =====================
        ///@todo currently limited to 20 threads, use Definitions::threads and assert something here
        ///@todo json configurable
        static const DepthType skipSize[]  = { 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };
        static const DepthType skipPhase[] = { 0, 1, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7 };
        // Distribute search depths across the threads
        if (! searcher.IsMainThread() ){
            int i = (searcher.Id() - 1) % 20;
            if (((iterativeDeepeningDepth + p.CurrentPly() + skipPhase[i]) / skipSize[i]) % 2) {
                //LOG(logINFO) << "Thead " << Id() << " skipping depth " << iterativeDeepeningDepth;
                return true;
            }
            else{
                //LOG(logINFO) << "Thead " << Id() << " entering depth " << iterativeDeepeningDepth;
                return false;
            }
        }
        else{
            //LOG(logINFO) << "Thead " << Id() << " entering depth " << iterativeDeepeningDepth;
            return false;
        }
        return false;
    }
}


#ifdef TT_DEBUG_HASH_COL
// some globals to debug "real" hash collisions ...
std::map<std::string, TTHASHTYPE> SearchAlgo::fenToZ;
std::map<TTHASHTYPE, std::string> SearchAlgo::ZToFen;
#endif

void Searcher::Search() {
    if (verbosity>=eVerb_verbose) LOG(logINFO) << "Call to Searcher Search";
    if ( IsMainThread() ){
        Reset();
        ThreadPool<Searcher>::Instance().StartOthers();
    }

    verbosity    = GetData().verbosity;
    GetData().best = Search(GetData().p, GetData().depth, 0, &(GetData().pv));
    GetData().searchDone = true;
    if (verbosity >= eVerb_verbose) {
        if (SearcherBase::GetMove(GetData().best).IsValid()) {
            LOG(logINFO) << "Searcher return move " << SearcherBase::GetMove(GetData().best)
                << " with score " << SearcherBase::GetScore(GetData().best)
                << " at depth " << (int)SearcherBase::GetDepth(GetData().best) << (IsASync() ? "(async)" : "");
        }
        else {
            LOG(logERROR) << "Searcher return invalid move";
        }
    }
}

void Searcher::Reset(){
    if (verbosity >= eVerb_verbose) LOG(logINFO) << "Call to Searcher Reset";

    stopFlag = false;

    // reset stat ///@todo stat by thread ?
    Stats::currentEveryNodesCount  = 0;
    Stats::currentPseudoNodesCount = 0;
    Stats::currentNodesCount       = 0;
    Stats::currentqNodesCount      = 0;
    Stats::currentMaxDepth         = 0;
    Stats::currentLeafNodesCount   = 0;
}

void Searcher::ResetKiller(){
   for(int k = 0 ; k < 2 ; ++k){
       for(int p = 0 ; p < MAX_GAME_PLY ; ++p){
           killerMovesWhite[k][p] = 0;
           killerMovesBlack[k][p] = 0;
       }
   }
}

void Searcher::ResetHistory(){
    for(int k = 0 ; k < 15 ; ++k){
        for(int p = 0 ; p < BOARD_SIZE ; ++p){
            searchHistory[k][p] = 0;
        }
    }
}

void Searcher::ResetCounter() {
    for (int k = 0; k < BOARD_SIZE; ++k) {
        for (int p = 0; p < BOARD_SIZE; ++p) {
            counterMove[k][p] = 0;
        }
    }
}

///@todo dynamic contempt :
/*
// Adjust contempt based on root move's previousScore (dynamic contempt)
int dct = ct + 88 * previousScore / (abs(previousScore) + 200);
// in endgame *0.5
*/

// stallmate due to 50 moves rules or 3 repetitions rule, or insuficent material
// a game structure is needed for the 3Rep verification
///@todo score the draw !
bool Searcher::IsForcedDraw(const Game & g, const Position & p, Analyse::EvalCache & cache, DepthType nbRep, bool displayDebug){

  // consider very long games as always draw ...
  if ( p.CurrentPly() >= MAX_GAME_PLY-MAX_SEARCH_DEPTH-1 ){
      return true; ///@todo this is not pretty at all ...
  }

  // 50 moves
  if ( p.HalfMoves50Rule() >= 100){
    if (displayDebug){
      LOG(logINFO) << "Draw, 50 moves rule !";
    }
    return true;
  }

  // material draw
  bool isDraw = false;

  cache.InitMaterialCache(p); // only if not yet initialized

  const int nbWp = cache.pieceNumber.nbWp;
  const int nbWk = cache.pieceNumber.nbWk;
  const int nbWb = cache.pieceNumber.nbWb;
  const int nbWr = cache.pieceNumber.nbWr;
  const int nbWq = cache.pieceNumber.nbWq;
  const int nbBp = cache.pieceNumber.nbBp;
  const int nbBk = cache.pieceNumber.nbBk;
  const int nbBb = cache.pieceNumber.nbBb;
  const int nbBr = cache.pieceNumber.nbBr;
  const int nbBq = cache.pieceNumber.nbBq;

  const int nbW  = cache.pieceNumber.nbW;
  const int nbB  = cache.pieceNumber.nbB;

  const int nbWm = cache.pieceNumber.nbWm;
  const int nbBm = cache.pieceNumber.nbBm;

  const int nbWM = nbW-nbWm;
  const int nbBM = nbB-nbBm;

  /*
  std::cout << "nbWp " << nbWp << std::endl;
  std::cout << "nbWk " << nbWk << std::endl;
  std::cout << "nbWb " << nbWb << std::endl;
  std::cout << "nbWr " << nbWr << std::endl;
  std::cout << "nbWq " << nbWq << std::endl;
  std::cout << "nbBp " << nbBp << std::endl;
  std::cout << "nbBk " << nbBk << std::endl;
  std::cout << "nbBb " << nbBb << std::endl;
  std::cout << "nbBr " << nbBr << std::endl;
  std::cout << "nbBq " << nbBq << std::endl;
  std::cout << "nbW  " << nbW  << std::endl;
  std::cout << "nbB  " << nbB  << std::endl;
  std::cout << "nbWm " << nbWm << std::endl;
  std::cout << "nbBm " << nbBm << std::endl;
  */

  if ( nbWp == 0 && nbBp == 0 ){

      // real FIDE draw
      // only kings
      if ( nbW +nbB == 0 ){
          isDraw = true;
      }
      // only one minor white
      else if ( nbWm == 1 && nbWM == 0 && nbB == 0 ){
          isDraw = true;
      }
      // only one minor black
      else if ( nbBm == 1 && nbBM == 0 && nbW == 0 ){
          isDraw = true;
      }
      // only two knights white
      else if ( nbWk == 2 && nbWb ==0 && nbWM == 0 && nbB == 0){
          isDraw = true;
      }
      // only two knights black
      else if ( nbBk == 2 && nbBb ==0 && nbBM == 0 && nbW == 0){
          isDraw = true;
      }
      // NOT real FIDE draw
      // only one minor each
      else if ( nbW == 1 && nbWm == 1 && nbB == 1 && nbBm == 1){
          isDraw = true;
      }
      // two minor (not bishop pair) versus one white
      else if ( nbWM == 0 && nbWm == 2 && nbWb != 2 && nbBM == 0 && nbBm == 1 ){
          isDraw = true;
      }
      // two minor (not bishop pair) versus one black
      else if ( nbBM == 0 && nbBm == 2 && nbBb != 2 && nbWM == 0 && nbWm == 1 ){
          isDraw = true;
      }
      ///@todo others ...
  }
  else{ // some pawn are present
      ///@todo ... KPK
  }

  if ( isDraw ){
     if (displayDebug ){
        LOG(logINFO) << "Draw, insuficent material !";
     }
     return true;
  }

  // 3 repetitions
  if ( p.CurrentPly() == 0){
    return false;
  }

  return g.Is3RepDraw(p, nbRep, displayDebug);
  return true;

}

ScoreType Searcher::Analyze(Position & p, bool lazy, bool fromPV) {

    if (Searcher::IsForcedDraw(game, p, analyze.MaterialCache(), fromPV?3:1, Definitions::debugConfig.modedebug)) {
        //LOG(logWARNING) << "Analysis found a draw, phase is " << (int)p.GamePhase();
        return p.IsEndGame()?0:-Definitions::scores.contempt;
    }

    const float taperedCoeff = Definitions::evalConfig.evalWithTapered ? (p.GamePhasePercent() - Position::endGameGamePhasePercent) / (100.f - Position::endGameGamePhasePercent) : 1.f;
    const bool whiteToMove = p.WhiteToPlay();
    return (whiteToMove ? +1 : -1 ) * ( lazy ? analyze.RunLazy(p) : analyze.Run(p,taperedCoeff));
}

void Searcher::ApplyMove(Position & p, const Move & m, bool verifyKingCapture) {
    p.ApplyMove(m, &game, verifyKingCapture);
    game.SetHash(p,-1,m.IsCapture());
}

Position Searcher::ApplyMoveCopyPosition(Position & p, const Move & m, bool verifyKingCapture) {
#ifdef WITH_MEMORY_POOL_BUILDER
    ScopeClone<Position, _default_block_size> psc(p);
    Position & p2 = psc.Get();
#else
    Position p2(p);
#endif
    ApplyMove(p2, m, verifyKingCapture);
    return p2; // hoping for RVO
}

#define HANDLE_TC_SEARCH(b) \
    if ( !stopFlag) { \
       int msec = TimeMan::Instance().ElapsedTicMS(); \
       if ( msec >= _allowedThinkTime) { \
          LOG(logINFO) << ">> TC endeed in Search"; \
          stopFlag = true; \
       } \
    } \
    if (stopFlag) { \
      OPTIM_STATS(++Stats::tcForcedAbort;) \
      if (iterativeDeepeningDepth == 0) { \
        LOG(logWARNING) << "Returning an hasardous move at depth 0, without time ..."; \
        best = current; \
      } \
      else{ \
         if ( !b || std::abs(SearcherBase::GetScore(current)) == Definitions::scores.stopFlagScore ){ \
            break; \
         } \
         stopDeepening = true; \
      } \
    }

SearcherBase::SearchedMove Searcher::Search(Position & p, const DepthType depth, const PlyType ply, Line * pv){

  // =====================
  // reset some thread dependant data
  // like killer, history, ...
  // =====================
  ResetKiller();
  ResetHistory();
  ResetCounter();

  // force current hash
  game.SetHash(p);

  ///@todo Definitions::do_useRecursivePVSort
  // for now, we are desactivating all pv saving stuff as soon as a TT is used inside the tree
  if ( Definitions::ttConfig.do_transpositionTableSortAlphaBeta || Definitions::ttConfig.do_transpositionTableAlphaBeta ){
     pv = 0;
  }

  // =====================
  // reset think time
  // =====================
  // currentMoveMs has been set by TimeMan or directly from json config
  _allowedThinkTime = Definitions::timeControl.currentMoveMs;

  // =====================
  // Look for a book move
  // =====================
  if ( p.CurrentMoveCount() < Definitions::bookConfig.maxBookMoves){ // let's avoid hash collision risk and win some time ...
     const Move * bookMove = Book::Get(p.GetZHash());
     if ( bookMove != NULL ){
       //LOG(logDEBUG) << "Using book move " << bookMove->Show();
       OPTIM_STATS(++Stats::openingBookHits;)
       // book score is high, this way the first not-book move will take more time
       Stats::previousScore[p.Turn()] = Definitions::scores.bookScore;
       return SearcherBase::SearchedMove(*bookMove,Definitions::scores.bookScore,-3);
     }
  }

  // =====================
  // TT aging or auto-clear
  // =====================
  ///@todo why is this so slow ???
  if ( IsMainThread() ){
      if (Definitions::ttConfig.do_ttAging ){
          Transposition::UpdateAge();
          Transposition::UpdateAgeQ();
      }
      if (Definitions::ttConfig.do_ttClearing ){
          LOG(logINFO) << "Clearing TT";
          Transposition::ClearTT(); // reset ttable
          Transposition::ClearTTQ(); // reset ttable
          Analyse::ClearTT(); // reset ttable
          Analyse::ClearTTL(); // reset ttable
          Analyse::ClearTTP(); // reset ttable
      }
  }

  // =====================
  // Some usefull variables ...
  // =====================
  ScoreType bestScorePrevious = -Definitions::scores.infScore;
  SearcherBase::SearchedMove best;

  // =====================
  // who is playing
  // =====================
  //const NSColor::eColor color = p.Turn();
  //const bool whiteToMove = p.WhiteToPlay();
  //const bool isMax = p.Turn() == color;
  //const bool maxIsWhite = color == NSColor::c_white; // here same as whiteToMove !

  // =====================
  // Generates moves early with a trusted generator
  // for early detection of mate / stalemate
  // =====================
  FastContainer<Move> moves;
  MoveGenerator(true).Generator(p,moves);
  if (verbosity >= eVerb_debug){
     UtilMove::DisplayList(moves,"Unsorted move list");
  }

  // =====================
  // is it already a mate or stalemate ?
  // =====================
  if ( moves.empty() ){ // check mate or stale mate
    // clear pv
    Line::ClearPV(pv);
    if ( p.IsInCheck() ){ // mate
      if (verbosity >= eVerb_debug){
        LOG(logINFO) << "Game is finished, checkmate";
      }
      Stats::previousScore[p.Turn()] = - (Definitions::scores.checkMateScore - p.CurrentMoveCount());
      // to find shortest checkmate sequence with extension we substract depth (here Moves);
      return SearcherBase::SearchedMove(Move(),  - (Definitions::scores.checkMateScore - p.CurrentMoveCount()), 0 );
    }
    else{ // stalemate
      if (verbosity >= eVerb_debug){
        LOG(logINFO) << "Game is finished, stalemate";
      }
      Stats::previousScore[p.Turn()] = 0;
      return SearcherBase::SearchedMove(Move(), 0, 0);
    }
  }

  // =====================
  // Iterative deepening loop
  // =====================
  DepthType reachedDepth=-1;
  bool stopDeepening = false;

  for (DepthType iterativeDeepeningDepth = Definitions::algo.do_iterativeDeepening?0:depth ;
       iterativeDeepeningDepth <= std::min(depth,DepthType(MAX_SEARCH_DEPTH-1)) && ! stopDeepening ;
       ++iterativeDeepeningDepth){

    Stats::seldepth[iterativeDeepeningDepth] = 0;

    // =====================
    // Should current thread skip this depth ?
    // =====================
    if ( ThreadSkipDepth(*this,p,iterativeDeepeningDepth) ) continue;

    // =====================
    // Transposition table look-up
    // =====================
    Transposition ttt;
    if ( Definitions::ttConfig.do_transpositionTableSortSearch) {
        Transposition::GetTT(PHASH(p), iterativeDeepeningDepth, -Definitions::scores.infScore, Definitions::scores.infScore, ttt, p.CurrentPly());
    }

    // =====================
    // Move ordering
    // =====================
    Move::Ordering(moves, p, *this,
                   true /*isPV*/, iterativeDeepeningDepth,ply,
                   Definitions::ttConfig.do_transpositionTableSortSearch?(&ttt):NULL, pv);
    if (verbosity >= eVerb_debug) {
        UtilMove::DisplayList(moves, "Sorted move list");
    }

    if (verbosity >= eVerb_debug){
      LOG(logINFO) << "=================================================";
      LOG(logINFO) << "iterative level " << (int)iterativeDeepeningDepth;
      LOG(logINFO) << "=================================================";
      double vm, rss;
      Util::ProcessMemUsage(vm, rss);
      LOG(logINFO) << "VM: " << vm << "; RSS: " << rss;
    }

    // =====================
    // Display (debug)
    // =====================
    if (verbosity >= eVerb_debug && Definitions::algo.do_iterativeDeepening && iterativeDeepeningDepth > 0){
      LOG(logINFO) << "Current best move is " << SearcherBase::GetMove(best).ShowAlg(p,false) << " (" << bestScorePrevious << ")";
    }

    // =====================
    // Aspiration window search
    // =====================
    bool aspirationcondition = Definitions::algo.do_iterativeDeepening
                            && iterativeDeepeningDepth > 4 ///@todo json
                            && Definitions::algo.do_aspirationwindow;
    ScoreType alpha = aspirationcondition?bestScorePrevious-Definitions::selectivity.windowSize
                                         :-Definitions::scores.infScore;
    ScoreType beta  = aspirationcondition?bestScorePrevious+Definitions::selectivity.windowSize
                                         : Definitions::scores.infScore;

    // local pv pointer is not null only if pv is not.
    Line pv_loc_obj;
    Line * pv_loc = 0;
    if (pv) {
        pv_loc = &pv_loc_obj;
    }
    Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)

    SearcherBase::SearchedMove current = SearchRoot(alpha,beta,moves,p,iterativeDeepeningDepth,ply,
                                                    pv_loc,              // update pv here if pv_loc !=0
                                                    TimeMan::eTC_on,     // subject to time control
                                                    Searcher::eRedA_on,  // allow reduction
                                                    Searcher::eExtA_on,  // allow extension
                                                    Move::HashType(0),   // no skipped move
                                                    true);               // given moves are trusted

    HANDLE_TC_SEARCH(false) // we cannot use the return move yet (not sure window is ok)

    // SearchRoot may find no move with score > alpha, and will thus return alpha
    if(aspirationcondition){
      ScoreType alpha_asp = alpha;
      ScoreType beta_asp  = beta;
      ScoreType delta = Definitions::selectivity.windowSize;
      // =====================
      // If current score is outside window, a research is needed
      // =====================
      while (   SearcherBase::GetScore(current) <= alpha_asp
             || SearcherBase::GetScore(current) >= beta_asp  ){

          // increase windows size
          OPTIM_STATS(++Stats::windowfails;)

          LOG(logINFO) << "Window re-search at depth " << (int)depth << ": " << alpha_asp << " " << beta_asp << " (" << SearcherBase::GetScore(current) << ", " << SearcherBase::GetMove(current).Show() << ")";
          Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)

          //delta = Definitions::scores.infScore; // full search

          delta += 2 + delta / 2; // from xiphos

          if (SearcherBase::GetScore(current) <= alpha_asp) {
              alpha_asp = SearcherBase::GetScore(current) - delta;
          }
          else if (SearcherBase::GetScore(current) >= beta_asp) {
              beta_asp = SearcherBase::GetScore(current) + delta;
          }

          current = SearchRoot(alpha_asp,beta_asp,
                               moves,p,iterativeDeepeningDepth,ply,
                               pv_loc,        // update pv here if pv_loc !=0
                               (iterativeDeepeningDepth != 0 ? TimeMan::eTC_on : TimeMan::eTC_off),  // subject to time control ?
                               Searcher::eRedA_on,
                               Searcher::eExtA_on,
                               Move::HashType(0),
                               true);         // given moves are trusted

          HANDLE_TC_SEARCH(false) // we cannot use the return move yet (not sure window is ok)
      }
      OPTIM_STATS(++Stats::windowsuccess;)
    }

    // we can use the returned move, even if it's an incompleted root search,
    // unless it is egal to stopFlagScore.
    // Will set stopDeepening = true (thus updating "best" before leaving the loop)
    // or break the loop
    HANDLE_TC_SEARCH(true)

    // =====================
    // if a check mate is found, abort deepening
    // =====================
    ///@todo is this risky to stop deepening in case of reductions ???
    if (std::fabs(SearcherBase::GetScore(current)) >= (Definitions::scores.checkMateScore - MAX_GAME_PLY)){
      if (verbosity >= eVerb_debug){
        LOG(logINFO) << "A forced check mate line was found !";
      }
      stopDeepening = true;
    }

    // =====================
    // update best, pv, stats, ...
    // =====================
    best = current;
    bestScorePrevious = SearcherBase::GetScore(best);

    // update pv
    if (pv) *pv = *pv_loc;

    // depth stat
    Stats::nodesCountByDepth[iterativeDeepeningDepth]  += Stats::currentNodesCount;
    Stats::qnodesCountByDepth[iterativeDeepeningDepth] += Stats::currentqNodesCount;
    ++reachedDepth;

    // =====================
    // display at each iterative deepening loop
    // =====================
    if (verbosity >= eVerb_standard) {
      DisplayPV(p,pv,best,iterativeDeepeningDepth/*,"search"*/);
    }

    // =====================
    // display some stats at each iterative deepening loop
    // =====================
    if (verbosity >= eVerb_debug) {
        Stats::CutCount();
    }

    // =====================
    // time control management
    // =====================
    float sec = TimeMan::Instance().ElapsedTic();
    if (stopFlag || int(sec * 1000) >= _allowedThinkTime) {
        LOG(logINFO) << ">> TC endeed in search";
        OPTIM_STATS(++Stats::tcIdealAbort;)
        break;
    }

    float EBF = float(Stats::currentLeafNodesCount + Stats::currentNodesCount) / Stats::currentNodesCount;
    float predictedNextIterationTime = (sec * 1000)*EBF;

    // emergency time ?
    ///@todo emergency time also trigger from stack evaluation !!!!
    if ( ! stopFlag
        && ! stopDeepening
        && ! TimeMan::Instance().IsForced()                                                                                        // forced mode is not used
        && predictedNextIterationTime < Definitions::timeControl.emergencyTimeFactor * _allowedThinkTime                           // next iteration should not too long (100% overhead)
        && bestScorePrevious < Stats::previousScore[p.Turn()] - Definitions::timeControl.emergencyScoreMargin                      // and score is worst than previous ply
        && TimeMan::Instance().GetNextMaximalTimePerMove(p) > Definitions::timeControl.emergencyTimeFactor * _allowedThinkTime) {  // and we have this time ...
        LOG(logINFO) << "Emergency time " << bestScorePrevious << "<" << Stats::previousScore[p.Turn()]
                     << " ( " << _allowedThinkTime << " => " << _allowedThinkTime *Definitions::timeControl.emergencyTimeFactor << " )";
        _allowedThinkTime = (unsigned long long)(_allowedThinkTime*Definitions::timeControl.emergencyTimeFactor);
    }

    // is it worth going one depth further ?
    if ( ! stopDeepening
        && iterativeDeepeningDepth > 3                                   // we have some certitudes
        && predictedNextIterationTime >= _allowedThinkTime               // next iteration will probably be too long
        && bestScorePrevious > Stats::previousScore[p.Turn()] ) {        // we are increasing score
      LOG(logINFO) << ">> TC endeed in search (EBF extrapolation) : "
                   << sec*1000 << " * EBF " << EBF << " > " << _allowedThinkTime
                   << ", previous score " << Stats::previousScore[p.Turn()] << ", current score " << bestScorePrevious;
      OPTIM_STATS(++Stats::tcEBFAbort;)
      break; // abord iterative deepening
    }

  }
  // =====================
  // end of iterative deepening loop
  // =====================

  if ( !SearcherBase::GetMove(best).IsValid()){
      LOG(logWARNING) << "Search returning an invalid move";
  }

  // =====================
  // stats : analysis nodes rate
  // =====================
  if ( Definitions::debugConfig.do_analysisNodeRate ){
    NodeStatType nodeCount = Stats::currentNodesCount + Stats::currentqNodesCount;
    double nodesPerSecond = 0;
    if (TimeMan::Instance().ElapsedTic() > 0) {
        nodesPerSecond = nodeCount / TimeMan::Instance().ElapsedTic();
    }
    //double pseudoNodesPerSecond = Stats::currentPseudoNodesCount/(double)(TimeMan::Now()-start)/1000.f;
    //double everyNodesPerSecond  = Stats::currentEveryNodesCount/(double)(TimeMan::Now()-start)/1000.f;
    if ( IsMainThread() && verbosity >= eVerb_standard) {
       LOG(logINFO) << "nodes per s : " << int(nodesPerSecond/1000) << " knps (n:" << Stats::currentNodesCount << ", q:" << Stats::currentqNodesCount << " (" << int(100./(Stats::currentqNodesCount+Stats::currentNodesCount)*Stats::currentqNodesCount)  << "%), t:" << (int)(TimeMan::Instance().ElapsedTicMS()) << "ms)";
    }
    //if ( IsMainThread() ) LOG(logINFO) << "nodes per s : " << int(pseudoNodesPerSecond/1000000) << " mnps (pseudo legal)";
    //if ( IsMainThread() ) LOG(logINFO) << "nodes per s : " << int(everyNodesPerSecond/1000000) << " mnps (all tries)";
  }

  // =====================
  // display after last iteration of iterative deepening
  // =====================
  if (verbosity >= eVerb_standard) {
    DisplayPV(p,pv,best,reachedDepth/*,"end"*/);
  }

  // =====================
  // backup score of current player
  // =====================
  Stats::previousScore[p.Turn()] = SearcherBase::GetScore(best);

  if (verbosity >= eVerb_verbose) {
      LOG(logINFO) << "End of search";
  }

  return best;
}

#define HANDLE_TC_WINDOW(b) \
if (! stopFlag) { \
   int msec = TimeMan::Instance().ElapsedTicMS(); \
   if (TimeMan::IsTCOn(timeControl) && msec >= _allowedThinkTime) { \
       LOG(logINFO) << ">> TC endeed in SearchRoot"; \
       stopFlag = true; \
   } \
} \
if (TimeMan::IsTCOn(timeControl) && stopFlag) { \
  if (iterativeDeepeningDepth != 0) { \
    if ( !b || !bestMove.IsValid() ) { \
       return SearcherBase::SearchedMove(Move(), Definitions::scores.stopFlagScore, -99); \
    } \
    else { \
       return SearcherBase::SearchedMove(bestMove, alpha, iterativeDeepeningDepth); \
    } \
  } \
  else { \
    if (alphaUpdated ){ \
       LOG(logWARNING) << "Returning an hasardous move in window search, alpha updated , without time ..."; \
       return SearcherBase::SearchedMove(bestMove, alpha, 0); \
    } \
    else{ \
       LOG(logWARNING) << "Returning an hasardous move in window search, alpha not updated , without time ..."; \
       return SearcherBase::SearchedMove((*it), 0, 0); \
    } \
  } \
}

SearcherBase::SearchedMove Searcher::SearchRoot(ScoreType alpha, ScoreType  beta,
                                                const FastContainer<Move> & moves,
                                                Position &                  p,
                                                const DepthType             iterativeDeepeningDepth,
                                                const PlyType               ply,
                                                Line *                      pv,
                                                TimeMan::eTC                timeControl,
                                                Searcher::ePruningAndReductionAllowed allowPruningAndReduction,
                                                Searcher::eExtensionAllowed           allowExtension,
                                                Move::HashType              skipMove,
                                                bool                        trustedMoves){

  if (iterativeDeepeningDepth < 0) {
     LOG(logFATAL) << "Negative iterativeDeepeningDepth in SearchRoot";
     //iterativeDeepeningDepth = 0;
  }

  // =====================
  // Transposition table look-up
  // =====================
  Transposition ttt;
  if (Definitions::ttConfig.do_transpositionTableSearch && skipMove == Move::HashType(0) ) {
      bool ttok = Transposition::GetTT(PHASH(p), iterativeDeepeningDepth, -Definitions::scores.infScore, Definitions::scores.infScore, ttt, p.CurrentPly());

      // only looking for exact match here at root node
      if (Definitions::ttConfig.do_transpositionTableSearch && ttok
          && ttt.t_type == Transposition::tt_exact && ttt.score > alpha && ttt.score < beta) {

          Move ttmove(Move::HashToMini(ttt.move));

          // is tt move legal ? (this avoid real hash collision at root,
          // as they can tollaty fail to be applied
          if (std::find(moves.begin(), moves.end(), ttmove) != moves.end()) {
#ifdef WITH_MEMORY_POOL_BUILDER
              ScopeClone<Position, _default_block_size> psc(p);
              Position & p2 = psc.Get();
#else
              Position p2(p);
#endif

              ///@todo those verifications looks quite shitty to me !

              // the move must be validated ... (especially en-passant detection, see below)
              UtilMove::DetectStandardCapture(ttmove, p2);
              if (UtilMove::PossibleEnPassant(ttmove, p2) && !UtilMove::ValidateEnPassant(ttmove, p2)) {
                  LOG(logFATAL) << "TT move lead to an illegal en-passant in this position " << p.GetFEN() << ", move was " << ttmove.Show();
              }

              // in case TT move was tagged ep but is not ep in the current position
              if (ttmove.Type() == UtilMove::sm_ep && !ttmove.IsCapture()) {
                  LOG(logFATAL) << "TT move tagged en-passant, but not valid in this position " << p.GetFEN() << ", move was " << ttmove.Show();
              }
              else { // ok
                  if (!UtilMove::ValidateCheckStatus(ttmove, p2)) {
                      LOG(logFATAL) << "TT move lead seems not legal in this position " << p.GetFEN() << ", move was " << ttmove.Show();
                  }
                  UtilMove::ValidateIsCheck(ttmove,p2);

                  ApplyMove(p2, ttmove); // game hash is updated

                  // is tt move leading to a draw ?
                  // if so, do not use it immediatly but keep it as first move ...
                  if (IsForcedDraw(game, p2, analyze.MaterialCache(), 3, Definitions::debugConfig.modedebug) ) {
                      //LOG(logDEBUG) << "TT move seems to lead to a draw";
                      //return SearcherBase::SearchedMove(ttmove, p.IsEndGame()?0:-Definitions::scores.contempt, iterativeDeepeningDepth);
                  }
                  else {
                      OPTIM_STATS(++Stats::ttHitUsed;)
                      // clear pv (we won't go deeper in the search tree)
                      Line::ClearPV(pv);
                      Line::SetPV(pv,ttmove.GetMini());
#ifdef DEBUG_TTHASH
                      LOG(logINFO) << p.GetFEN() << " tt hit " << ttt.score << " " << ttt.move << " " << ttt.depth << " >= " << depthleft << " - " << ttt.type;
#endif
                      //LOG(logINFO) << "TT search success, depth " << iterativeDeepeningDepth;
                      return SearcherBase::SearchedMove(ttmove, ttt.score, iterativeDeepeningDepth);
                  }
              }
          }
          else {
              LOG(logFATAL) << "TT move not in current move list !";
          }
      }
  }

  Move bestMove; // invalid for now ..., will be valid only if some move raises alpha
  bool validMoveFound = false; // this is different of bestMove.IsValid(), is true as soon as a move can be played

  // =====================
  // LMR stuffs
  // =====================
  const float gamephase = p.GamePhasePercent()/100.f;
  DepthType lmr_start = (DepthType)((1-gamephase) * Definitions::selectivity.LMR_ENDGAME
                                      + gamephase * Definitions::selectivity.LMR_STD); // tappered LMR depth
  DepthType lmr_count = 0;

  // =====================
  // who is playing
  // =====================
  const NSColor::eColor color = p.Turn();
  const bool whiteToMove = p.WhiteToPlay();
  const bool isMax = p.Turn() == color; // here always true
  const bool maxIsWhite = color == NSColor::c_white; // same whiteToMove here ...

  bool alphaJustUpdated = false; // true if previous move (in the next loop) has raised alpha
  bool alphaUpdated = false;     // true if aplha was raised at least once (same as bestMove.IsValid() ...)

  // local pv pointer is not null only if pv is not.
  Line pv_loc_best_obj;
  Line * pv_loc_best = 0;
  if (pv) {
      pv_loc_best = &pv_loc_best_obj;
  }

  bool atLeastOneCheckMateDetected = false;

  // =====================
  // loop through depth 0 moves
  // =====================
  for (auto it = moves.begin(); it != moves.end(); ++it) {

    ++Stats::currentNodesCount;

    if ((*it).ZHash() == skipMove) continue;

    // =====================
    // validate current move
    // =====================
    if (!trustedMoves && !UtilMove::ValidateMove(*it,p) ) continue;

    /*
    // don't look twice at lines where we are checkmated ...
    if ((*it).SortScore() <= -(Definitions::checkMateScore - MAX_GAME_PLY) && (*it).SortScore() != Definitions::defaultMoveScore) {
       if (verbosity >= eVerb_debug) {) LOG(logINFO) << "Skipping already detected checkmate node against current player " << (*it).Show() << " " << (*it).SortScore() << " " << p.GetFEN();
       atLeastOneCheckMateDetected = true;
       continue;
    }
    */

    validMoveFound = true;

    bool isPV = lmr_count == 0; // inside current principal variation

    // this is because, even if no bestmove is found > alpha,
    // we insert something in TT with score alpha ...
    if ( isPV ) bestMove = *it;

    // a local pv object (only used if pv != 0)
    Line pv_loc_obj;
    Line * pv_loc = 0;
    if (pv){
        pv_loc = &pv_loc_obj;
    }

    // in case alpha is updated by the last move, we re-count lmr moves from 1
    // this may be an unstable position ...
    if ( alphaJustUpdated ){
        alphaJustUpdated = false;
        lmr_count = 1;
    }

    // copy / make
    Position p2 = ApplyMoveCopyPosition(p, *it);

    // =====================
    // look for applicable extensions
    // =====================
    DepthType nextDepth = iterativeDeepeningDepth;
    if(allowExtension == Searcher::eExtA_on){
        ManageExtensionRoot(nextDepth, p, p2, *it, moves, isPV,*this);
    }
    // ensure depth is not too deep ...
    nextDepth = std::min(nextDepth,DepthType(MAX_SEARCH_DEPTH-1));
    DepthType nextDepthWithoutReduction = nextDepth;

    // =====================
    // reduction : LMR
    // =====================
    if ( Definitions::algo.do_lmrSearch
           && lmr_count >= lmr_start
           //&& alpha_loc > Stats::previousScore[p.Turn()] - Definitions::lmrtol
           && (*it).CanBeReduced()
           && nextDepth > 2
           && ! isPV
           && ! (*it).IsCheck()
           && ! (*it).IsAdvancedPawnPush(p)
           && ! p.IsInCheck()
           && ! p.IsEndGame() ){
        int lmr_id = ReductionId(nextDepth,lmr_count,MAX_REDUCTION_DEPTH);
        int R = lmr_reduction[lmr_id];
        nextDepth -= R;
        nextDepth = std::max(nextDepth,DepthType(0)); // limit lowest depth
        OPTIM_STATS(++Stats::lmrsearch;)
    }

    // =====================
    // eventually apply PVS
    // =====================
    ///@todo is the last condition needed ??
    bool doPVS = Definitions::algo.do_pvsRoot && !isPV && alphaUpdated;
    ScoreType alpha_loc = alpha;
    ScoreType beta_loc =  doPVS ? alpha+1 : beta;

    // =====================
    // classic PVS alpha-beta
    // =====================
    Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
    ScoreType score = -Negamax( -beta_loc, -alpha_loc,  nextDepth, p2, color, iterativeDeepeningDepth, ply+1,
                                isPV, pv_loc,timeControl,allowPruningAndReduction,allowExtension);

    HANDLE_TC_WINDOW(false) // cannot use the move if stopflag because search is not validate yet

    // =====================
    // a reduced search cannot raise alpha !
    // =====================
    if ( score > alpha && nextDepthWithoutReduction != nextDepth){
       OPTIM_STATS(++Stats::lmrsearchfail;)
       nextDepth = nextDepthWithoutReduction;
       Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
       score = -Negamax( -beta_loc, -alpha_loc,  nextDepth, p2, color, iterativeDeepeningDepth, ply+1,
                         isPV, pv_loc,timeControl,allowPruningAndReduction,allowExtension);

       HANDLE_TC_WINDOW(false) // cannot use the move if stopflag because search is not validate yet
    }

    if (doPVS){
      if ( score > alpha && score < beta){ // PVS fail (fail-soft framework)
        OPTIM_STATS(++Stats::pvsRootfails;)
        Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
        score = -Negamax( -beta, -alpha, nextDepth , p2, color,iterativeDeepeningDepth, ply+1,
                          true, pv_loc,timeControl,allowPruningAndReduction,allowExtension); //can be a new pv!
      }
      else{
          OPTIM_STATS(++Stats::pvsRootsuccess;)
      }
    }

    if (verbosity >= eVerb_debug){
      LOG(logINFO) << "move " << (*it).Show() << ((*it).IsCapture()?"*":" ") << ", score " << score;
    }

    // if at least one move has been played,
    // we are able to return it (it is the pv one or a move that has a better score)
    HANDLE_TC_WINDOW(validMoveFound)

    // =====================
    // update alpha
    // =====================
    if ( score > alpha){
      alpha = score;
      bestMove = *it;

      alphaUpdated = true;
      alphaJustUpdated = true;

      // =====================
      // beta cut-off
      // =====================
      if (score >= beta) {
          // ttable insert
          if ((Definitions::ttConfig.do_transpositionTableSearch || Definitions::ttConfig.do_transpositionTableSortSearch)
                  && !stopFlag  ///@todo even if HANDLE_TC_WINDOW(true) ??
                  && skipMove == Move::HashType(0)) {
#ifdef DEBUG_TTHASH
              LOG(logINFO) << p.GetFEN() << " tt insert " << ret << " " << (*it).Show() << " " << iterativeDeepeningDepth << " - " << "beta " << beta;
#endif

#ifdef TT_DEBUG_HASH_COL
              if (!CheckFenHashCol(p2.GetFENShort2(), PHASH(p2), p2)) {
                  LOG(logFATAL) << "Hash test failed";
              }
#endif
              Transposition::InsertTT(PHASH(p), beta, iterativeDeepeningDepth, Transposition::tt_beta, (*it).ZHash()
#ifdef DEBUG_TT_SORT_FAIL
                  , p.GetFEN()
#endif
              );

          }

          if (!Definitions::algo.do_alphabetaminimax) {
              OPTIM_STATS(++Stats::betacutroot;)
              // update pv
              if ( pv ) Line::UpdatePV(pv, *pv_loc, it->GetMini(), p);
              return SearcherBase::SearchedMove(*it, Definitions::algo.do_failsoftalphabeta ? score : beta, iterativeDeepeningDepth);
          }
      } // score >= beta

      // =====================
      // update pv
      // =====================
      Line::ClearPV(pv_loc_best);
      if ( pv ) Line::UpdatePV(pv_loc_best,*pv_loc,it->GetMini(),p);

      // =====================
      // display
      // =====================
      //if (verbosity >= eVerb_verbose) {
      //    DisplayPV(p,pv,Searcher::SearchedMove(bestMove,alpha,iterativeDeepeningDepth),iterativeDeepeningDepth/*,"update"*/);
      //}
    } // score > alpha

    ++lmr_count;

    // backup current score in move container, this allow for checkmate detection against current player
    // even in case of move re-order (TT,PV) and LMR.
    /*
    (*it).SetSortScore(score);
    if ( (*it).SortScore() <= -( Definitions::checkMateScore - MAX_GAME_PLY) ){
        if (verbosity >= eVerb_debug) LOG(logINFO) << "Dectected a forced check mate sequence against current player after move " << (*it).Show() << " " << (*it).SortScore();
    }
    */

    // if at least one move has been played, we are able to return it (it is the pv one or a move that has a better score)
    HANDLE_TC_WINDOW(validMoveFound)

  } // all depth=0 moves

  // =====================
  // ttable insert
  // =====================
  if ((Definitions::ttConfig.do_transpositionTableSearch || Definitions::ttConfig.do_transpositionTableSortSearch) &&
          bestMove.IsValid() && ! stopFlag && skipMove == Move::HashType(0)) {
#ifdef DEBUG_TTHASH
    LOG(logINFO) << p.GetFEN() << " tt insert " << ret << " " << bestMove.Show() << " " << depthleft-1 << " - " << "exact";
#endif
#ifdef TT_DEBUG_HASH_COL
    if (!CheckFenHashCol(p.GetFENShort2(), PHASH(p), p)) {
        LOG(logFATAL) << "Hash test failed";
    }
#endif
    Transposition::InsertTT(PHASH(p), alpha, iterativeDeepeningDepth, alphaUpdated?Transposition::tt_exact:Transposition::tt_alpha, bestMove.ZHash()
#ifdef DEBUG_TT_SORT_FAIL
        , p.GetFEN()
#endif
    );
  }

  // only if stopflag was not trigger, we update the returned pv line !
  if ( ! stopFlag && pv ){
     *pv = *pv_loc_best;
  }

  if ( !bestMove.IsValid() ){
      if (!validMoveFound) {
          LOG(logDEBUG) << "Windows search find no move " << alpha << " " << beta << " " << p.GetFEN() << " no move raising alpha, depth " << iterativeDeepeningDepth;
          LOG(logDEBUG) << "Because no valid moves here ...";
      }
      else {
          LOG(logDEBUG) << "Windows search find no move " << alpha << " " << beta << " " << p.GetFEN() << " no move raising alpha, depth " << iterativeDeepeningDepth;
          if (atLeastOneCheckMateDetected) {
              LOG(logDEBUG) << "Windows search find no move " << alpha << " " << beta << " " << p.GetFEN() << " no move raising alpha, depth " << iterativeDeepeningDepth;
              LOG(logDEBUG) << "Because a forced checkmate against current player is already found";
          }
      }
      return SearcherBase::SearchedMove(Move(), alpha, iterativeDeepeningDepth);
  }

  return SearcherBase::SearchedMove(bestMove,alpha,iterativeDeepeningDepth);

}

#ifdef TT_DEBUG_HASH_COL
bool Searcher::CheckFenHashCol(const std::string & fen, const TTHASHTYPE & hash, const Position & p){

  if ( fenToZ.size() > 100000000 ){
    LOG(logINFO) << "hash/fen test over";
    exit(2);
  }
  if ( ZToFen.size() > 100000000 ){
    LOG(logINFO) << "hash/fen test over";
    exit(2);
  }

  if ( fenToZ.find(fen) != fenToZ.end() ){
    if ( fenToZ[fen] != hash ){
      LOG(logERROR) << "Same fen with != hash " << p.GetFEN() << " " << fenToZ[fen] << " " << hash;
      return false;
    }
  }
  else{
    fenToZ[fen] = hash;
  }
  if ( ZToFen.find(hash) != ZToFen.end() ){
    if ( ZToFen[hash] != fen ){
      LOG(logERROR) << "Same hash with != fen " << hash << " " << ZToFen[hash] << " " << p.GetFEN();
      return false;
    }
  }
  else{
    ZToFen[hash] = fen;
  }
  return true;
}
#endif // TT_DEBUG_HASH_COL
