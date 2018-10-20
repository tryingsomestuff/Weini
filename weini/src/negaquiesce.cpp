#include "search.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "ttable.h"
#include "book.h"
#include "sort.h"

#include <iterator>
#include <iomanip>

///@todo generate check and check evasion in QSearch

// return > 0 if "color" player is better
ScoreType Searcher::NegaQuiesce(ScoreType       alpha,
                                ScoreType       beta,
                                DepthType       depthleft,
                                Position &      p,
                                NSColor::eColor color,
                                const DepthType iterativeDeepeningDepth,
                                const PlyType   ply,
                                bool            fromPV,
                                TimeMan::eTC    timeControl){

  // =====================
  // Update seldepth
  // =====================
  if (Stats::seldepth[iterativeDeepeningDepth] < ply) {
     Stats::seldepth[iterativeDeepeningDepth] = ply;
  }

  // =====================
  // forward stop order (at very very short TC, this is usefull)
  // =====================
  if (stopFlag && TimeMan::IsTCOn(timeControl)) {
    return Definitions::scores.stopFlagScore;
  }

  // =====================
  // trigger stop order if needed
  // =====================
  int msec = TimeMan::Instance().ElapsedTicMS();
  if (msec >= _allowedThinkTime && TimeMan::IsTCOn(timeControl)) {
    LOG(logINFO) << ">> TC endeed in NegaQuiesce";
    // abord immediatly
    // set stopflag and return a false score
    stopFlag = true;
    return Definitions::scores.stopFlagScore;
  }

  // =====================
  // protect against to long games
  // =====================
  if ( p.CurrentMoveCount() >= MAX_GAME_PLY ){
      return Analyze(p, false, fromPV);
  }

  // =====================
  // collect statistics
  // =====================
  // this is what we call a "visited" qnode
  OPTIM_STATS(++Stats::visitedqnodes;)
  // update also current qnode count
  ++Stats::currentqNodesCount;

  // =====================
  // mate distance pruning
  // =====================
  alpha = std::max( - Definitions::scores.checkMateScore + p.CurrentMoveCount()    , alpha);
  beta  = std::min(   Definitions::scores.checkMateScore - p.CurrentMoveCount() +1 , beta);
  if (alpha >= beta) {
      return alpha;
  }

  // =====================
  // who's playing ?
  // =====================
  const bool whiteToMove = p.WhiteToPlay();
  //const bool isMax = p.Turn() == color;
  //const bool maxIsWhite = color == NSColor::c_white;

#ifdef QUIESCE_WITHOUT_CHECK_VALIDATION
  // =====================
  // No more king !
  // =====================
  if ((whiteToMove && p.NoWhiteKingAnymore()) || ((!whiteToMove && p.NoBlackKingAnymore())) ){
      return -(Definitions::scores.checkMateScore - p.CurrentMoveCount()); // to find shortest checkmate sequence with extension we substract depth (here Moves)
  }
#endif

  // =====================
  // Transposition table look-up
  // =====================
  Transposition ttt;
  if ( OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableQuiesce || Definitions::ttConfig.do_transpositionTableQSort)
       && Transposition::GetTTQ(PHASH(p), depthleft, alpha, beta, ttt)) {
    // in pv, only use exact hit
    if ( !fromPV && (
            (ttt.t_type == Transposition::tt_exact )
         || (ttt.t_type == Transposition::tt_alpha && ttt.score <= alpha )
         || (ttt.t_type == Transposition::tt_beta  && ttt.score >= beta  ) ) ) {
      OPTIM_STATS(++Stats::ttQHitUsed;)
      return ttt.score;
    }
  }

  ///@todo if position IsInCheck, generate all moves
  ///and if not use this classic Generator for QSearch

  FastContainer<Move> moves;

#ifndef QUIESCE_WITH_ONLY_CAPS
  // =====================
  // Move generation (all moves)
  // =====================
  MoveGenerator(Definitions::algo.do_trustedGenerator,false).Generator(p,moves); // a chance to track down checkmate if not only capture

  // CARE CARE CARE if do_trustedGenerator if false, this is useless and thus result with do_trustedGenerator true/false slightly differ (at end game generally)
  // =====================
  // Early _exit for checkmate and stalemate
  // =====================
  if ( moves.empty() ){ // checkmate or stalemate
    if ( p.IsInCheck() ){ // mate
      //LOG(logINFO) << "Check mate " << p.GetFEN();
      return - (Definitions::checkMateScore - p.Moves()); // to find shortest checkmate sequence with extension we substract depth (here Moves)
    }
    else{ // stalemate
      //LOG(logINFO) << "Stale mate " << p.GetFEN();
      return p.IsEndGame() ? 0 : -Definitions::contempt;
    }
    LOG(logFATAL) << "Gros soucis ! " << p.GetFEN();
  }
#endif

  // =====================
  // Evaluation of static position
  // =====================
  ScoreType retBase = Analyze(p, false, fromPV); ///@todo use Definitions::algo.do_lazyEvaluation ?;

  ///@todo Use tt score when possible
  /*
  if (ttt.hash != 0 &&
           ( ( ttt.t_type != Transposition::tt_alpha  && (ttt.score > retBase) )
          || ( ttt.t_type != Transposition::tt_beta   && (ttt.score < retBase) ) ) ){
     retBase = ttt.score;
  }
  else{ // standard static eval
     retBase = Analyze(p, false); ///@todo use Definitions::algo.do_lazyEvaluation ??
  }
  */

  // =====================
  // Early exit, max quiesce depth reached
  // =====================
  if ( depthleft == 0){
    OPTIM_STATS(++Stats::terminalnodes;) // this shall be small, because in this case we will miss something !
    //if (Definitions::algo.do_lazyEvaluation) retBase += what is missing
    //LOG(logDEBUG) << "Quiesce ends at max depth" << retBase;
    return retBase;
  }

  // =====================
  // Delta pruning (without see value)
  // =====================
  if ( OPTIM_BOOL_TRUE(
       Definitions::algo.do_deltaPruning &&
     ! Definitions::algo.do_quiesceminimax) &&
     !p.IsEndGame() && !p.IsInCheck() ){
    const ScoreType deltaMargin = Definitions::selectivity.deltaMargin;
    if (retBase < alpha - deltaMargin /*- std::abs(Piece::Value(p.Info().lastCapture))*/){
      //LOG(logINFO) << "Delta alpha cut-off " << retBase << " " << alpha;
      OPTIM_STATS(++Stats::deltaalphacut;)
      return Definitions::algo.do_failsoftquiesce ? retBase : alpha - deltaMargin;
    }
  }

  // =====================
  // compute the full static evaluation if former was a Lazy Eval
  // =====================
  //if (Definitions::algo.do_lazyEvaluation) retBase += what is missing

  // =====================
  // Early exit, versus current alpha-beta
  // Beta cut-off, update alpha
  // =====================
  if (OPTIM_BOOL_TRUE(! Definitions::algo.do_quiesceminimax) && retBase >= beta){
    OPTIM_STATS(++Stats::qearlybetacut;)
    return Definitions::algo.do_failsoftquiesce?retBase:beta;
  }
  alpha = std::max(alpha,retBase);

  bool validThreatFound = false; // because of pinned piece, all moves may be invalid

#ifdef QUIESCE_WITH_ONLY_CAPS
  // =====================
  // Move generation (only capture moves)
  // =====================
  MoveGenerator(Definitions::algo.do_trustedGenerator,true).Generator(p,moves); // cannot track checkmate this way !!!!
#endif

  // =====================
  // Move ordering
  // =====================
  if ( !moves.empty() ){
     Sort::SortQMoves(moves,p,*this);

     // TT move first
     if (Definitions::ttConfig.do_transpositionTableQSort && ttt.move != 0 ) {
        OPTIM_STATS(++Stats::ttQsorttry;)
        if (Transposition::Sort( ttt, moves, p)) {
          OPTIM_STATS(++Stats::ttQsort;)
        }
     }
  }

  // =====================
  // collect statistics
  // =====================
  // this is what we call a "really visited" qnode
  // a qnode without early cut-off
  OPTIM_STATS(++Stats::visitedrealqnodes;)

  Move * bestmove = NULL;

  // =====================
  // Quiesce alpha beta search
  // =====================
  DepthType lmr_count = 0;

  ScoreType alpha_init = alpha;
  bool alphaRaised = false;

  ScoreType futilityBase = retBase + Definitions::selectivity.qfutilityMargin;

  // =====================
  // Loop on moves
  // =====================
  for(auto it = moves.begin() ; it != moves.end() ; ++it, lmr_count++){

    //bool isPV = fromPV && lmr_count == 0; ///@todo ?
    bool isFirstMove = lmr_count == 0;

    if (   !p.IsInCheck()
        && !(*it).IsCheck()
        && !(*it).IsAdvancedPawnPush(p)
        && !isFirstMove
        && futilityBase > -Definitions::scores.checkMateScore + MAX_GAME_PLY) {

        ScoreType futilityValue = futilityBase + Piece::ValueAbs(p.Get((*it).To()));

        // =====================
        // Futility pruning (q)
        // =====================
        if (OPTIM_BOOL_TRUE(Definitions::algo.do_qfutilityPruning)
            && futilityValue <= alpha_init) {
            alpha = std::max(alpha, futilityValue);
            OPTIM_STATS(++Stats::qfutilitycut;)
            continue;
        }

        // =====================
        // SEE pruning (q)
        // =====================
        if (OPTIM_BOOL_TRUE(Definitions::algo.do_seePruning)
            && futilityBase <= alpha_init
            && (*it).SortScore() < -Definitions::scores.captureSortScore) {
            alpha = std::max(alpha, futilityBase);
            OPTIM_STATS(++Stats::seecut;)
            continue;
        }
    }

#ifndef QUIESCE_WITH_ONLY_CAPS
    // =====================
    // skip not capture moves
    // =====================
    if ( ! (*it).IsCapture() ){
      //LOG(logDEBUG) << "Only capture node is interesting here!";
      continue;
    }
#endif

#ifndef QUIESCE_WITHOUT_CHECK_VALIDATION
    // =====================
    // validate current move
    // =====================
    if ( ! UtilMove::ValidateMove(*it,p) ) continue;
#endif

    validThreatFound = true;

    // this is because, even if no bestmove is found > alpha, insert something in TT with score alpha ...
    if ( isFirstMove ) bestmove = &*it;

    // =====================
    // Apply the current move
    // =====================
    // copy / make
    Position p2 = ApplyMoveCopyPosition(p, *it, false);

    const DepthType nextDepth = depthleft - 1;
    const ScoreType val = - NegaQuiesce(-beta, -alpha, nextDepth, p2, color, iterativeDeepeningDepth, ply+1, fromPV, timeControl);

    if (val > alpha) {

        alphaRaised = true;
        bestmove    = &*it;
        alpha       = val;

        // =====================
        // Beta cut-off
        // =====================
        if (val >= beta){

          // ttable insert
          if ( OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableQuiesce||Definitions::ttConfig.do_transpositionTableQSort)
            && !stopFlag){
    #ifdef TT_DEBUG_HASH_COL
            if (!CheckFenHashCol(p2.GetFENShort2(), PHASH(p2), p2)) {
                LOG(logFATAL) << "Hash test failed";
            }
    #endif
            Transposition::InsertTTQ(PHASH(p), val, depthleft, Transposition::tt_beta, (*it).ZHash()
    #ifdef DEBUG_TT_SORT_FAIL
                , p.GetFEN()
    #endif
            );

          }

          if ( OPTIM_BOOL_TRUE(! Definitions::algo.do_quiesceminimax) ){
            OPTIM_STATS(++Stats::qbetacut;)
            return Definitions::algo.do_failsoftquiesce?val:beta;
          }
        }
    }
  }

  // ttable insert and return in case a valid move is found
  if ( validThreatFound  ///@todo this is wrong (always true...) in case of QUIESCE_WITHOUT_CHECK_VALIDATION ...
    && OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableQuiesce||Definitions::ttConfig.do_transpositionTableQSort)
    && !stopFlag
    && bestmove){
#ifdef TT_DEBUG_HASH_COL
        if (!CheckFenHashCol(p.GetFENShort2(), PHASH(p), p)) {
            LOG(logFATAL) << "Hash test failed";
        }
#endif
      Transposition::InsertTTQ(PHASH(p), alpha, depthleft, alphaRaised ? Transposition::tt_exact : Transposition::tt_alpha, (*bestmove).ZHash()
#ifdef DEBUG_TT_SORT_FAIL
          , p.GetFEN()
#endif
      );

      return alpha;
  }

  //there was no valid capture ! (never reach here if QUIESCE_WITHOUT_CHECK_VALIDATION)
  ///@todo InsertTTQ anyway ?
  return alpha;
}
