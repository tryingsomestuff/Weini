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
ScoreType Searcher::NegaQuiesce(ScoreType alpha, ScoreType beta, DepthType depthleft,
                                Position & p, NSColor::eColor color, bool fromPV,
                                bool timeControl){

  // =====================
  // forward stop order (at very short TC, this is usefull)
  if (stopFlag && timeControl) {
    return Definitions::scores.stopFlagScore;
  }

  // =====================
  // trigger stop order if needed
  // =====================
  int msec = TimeMan::Instance().ElapsedTicMS();
  if ( timeControl && msec >= _allowedThinkTime) {
    LOG(logINFO) << ">> TC endeed in NegaQuiesce";
    // abord immediatly
    // set stopflag and return a false score
    stopFlag = true;
    return Definitions::scores.stopFlagScore;
  }

  // =====================
  // mate distance pruning
  // =====================
  alpha = std::max( - Definitions::scores.checkMateScore + p.Moves()    , alpha);
  beta  = std::min(   Definitions::scores.checkMateScore - p.Moves() +1 , beta);
  if (alpha >= beta) {
      return alpha;
  }

  // =====================
  // protect against to long games
  // =====================
  if ( p.Moves() >= MAX_GAME_PLY ){
      return Analyze(p, false);
  }

  // this is what we call a "visited" qnode
  ++Stats::visitedqnodes;
  // update also current qnode count
  ++Stats::currentqNodesCount;

  // =====================
  // who's playing ?
  const bool whiteToMove = p.WhiteToPlay();
  //const bool isMax = p.Turn() == color;
  //const bool maxIsWhite = color == NSColor::c_white;

#ifdef QUIESCE_WITHOUT_CHECK_VALIDATION
  // =====================
  // No more king !
  // =====================
  if ((whiteToMove && p.NoWhiteKingAnymore()) || ((!whiteToMove && p.NoBlackKingAnymore())) ){
      return -(Definitions::scores.checkMateScore - p.Moves()); // to find shortest checkmate sequence with extension we substract depth (here Moves)
  }
#endif

  // =====================
  // Transposition table look-up
  // =====================
  Transposition ttt;
  if ( (Definitions::ttConfig.do_transpositionTableQuiesce || Definitions::ttConfig.do_transpositionTableQSort)
       && Transposition::GetTTQ(PHASH(p), depthleft, alpha, beta, ttt)) {
    // in pv, only use exact hit
    if ( !fromPV || (ttt.t_type == Transposition::tt_exact && ttt.score > alpha && ttt.score < beta)) {
      ++Stats::ttQHitUsed;
      return ttt.score;
    }
  }

  ///@todo if position IsInCheck, generate all moves
  ///and if not use this classic Generator for QSearch

  FastContainer<Move> moves;
  //ScopePoolObject<FastContainer<Move>, _default_block_size> moves_;
  //FastContainer<Move> & moves = moves_.Get();
#ifndef QUIESCE_WITH_ONLY_CAPS
  // generate all moves
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

  // Evaluation of static position
  ScoreType retBase = Analyze(p, false); ///@todo use Definitions::algo.do_lazyEvaluation ??/*0*/;

  /*
  // Use tt score when possible
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
  // Early _exit, max quiesce depth reached
  // =====================
  if ( depthleft == 0){
    ++Stats::terminalnodes; // this shall be small, because in this case we will miss something !
    //if (Definitions::algo.do_lazyEvaluation) retBase += what is missing
    //LOG(logDEBUG) << "Quiesce ends at max depth" << retBase;
    return retBase;
  }

  // =====================
  // Delta pruning (without see value)
  // =====================
  if ( Definitions::algo.do_deltaPruning &&
     ! Definitions::algo.do_quiesceminimax &&
     !p.IsEndGame() && !p.IsInCheck() ){
    const ScoreType deltaMargin = Definitions::selectivity.deltaMargin;
    if (retBase < alpha - deltaMargin /*- std::abs(Piece::Value(p.Info().lastCapture))*/){
      //LOG(logINFO) << "Delta alpha cut-off " << retBase << " " << alpha;
      ++Stats::deltaalphacut;
      return Definitions::algo.do_failsoftquiesce ? retBase : alpha - deltaMargin;
    }
  }

  // compute the full static evaluation
  //if (Definitions::algo.do_lazyEvaluation) retBase += what is missing

  // =====================
  // Early exit, versus current alpha-beta
  // Beta cut-off
  // =====================
  if (! Definitions::algo.do_quiesceminimax && retBase >= beta){
    ++Stats::qearlybetacut;
    return Definitions::algo.do_failsoftquiesce?retBase:beta;
  }
  alpha = std::max(alpha,retBase);

  //LOG(logDEBUG) << "Quiesce needed after move to " << Square(p.Info().lastMove.to).position() << " " << ret << " " << alpha << " " << beta << " " << threats.size();

  bool validThreatFound = false; // because of pinned piece, all moves may be invalid

#ifdef QUIESCE_WITH_ONLY_CAPS
  // generate only capture moves
  MoveGenerator(Definitions::algo.do_trustedGenerator,true).Generator(p,moves); // cannot track checkmate this way !!!!
#endif

  // =====================
  // Move ordering
  // =====================
  if ( !moves.empty() ){
     Sort::SortQMoves(moves,p);

     // TT move first
     if (Definitions::ttConfig.do_transpositionTableQSort && ttt.move != 0 ) {
        ++Stats::ttQsorttry;
        if (Transposition::Sort( ttt, moves, p)) {
          ++Stats::ttQsort;
        }
     }
  }

  // this is what we call a "really visited" qnode
  // a qnode without early cut-off
  ++Stats::visitedrealqnodes;

  //debug
  // LOG(logINFO) << p.GetFEN();

  Move * bestmove = NULL;

  // =====================
  // Quiesce alpha beta search
  // =====================
  DepthType lmr_count = 0;

  ScoreType alpha_init = alpha;
  bool alphaRaised = false;

  for(auto it = moves.begin() ; it != moves.end() ; ++it, lmr_count++){

    //bool isPV = fromPV && lmr_count == 0; ///@todo ?
    bool isFirstMove = lmr_count == 0;

    ScoreType futilityBase = retBase + Definitions::selectivity.qfutilityMargin;
    ScoreType futilityValue = futilityBase + Piece::Value(p.Get((*it).To()));

    // Futility pruning (q)
    if ( Definitions::algo.do_qfutilityPruning
          && ! p.IsInCheck()
          && ! (*it).IsCheck()
          && ! (*it).IsAdvancedPawnPush(p)
          && ! isFirstMove
          && futilityBase > - Definitions::scores.checkMateScore + MAX_GAME_PLY
          && futilityValue <= alpha_init){
            alpha = std::max(alpha, futilityValue);
            ++Stats::qfutilitycut;
            continue;
    }

    // SEE pruning (q)
    if (Definitions::algo.do_seePruning
        && ! p.IsInCheck()
        && ! (*it).IsCheck()
        && ! (*it).IsAdvancedPawnPush(p)
        && ! isFirstMove
        && futilityBase > - Definitions::scores.checkMateScore + MAX_GAME_PLY
        && futilityBase <= alpha_init
        && !SEE(p, (*it), 1)){ // not 0 ... we also cut equal capture !
           alpha = std::max(alpha, futilityBase);
           ++Stats::seecut;
           continue;
    }

#ifndef QUIESCE_WITH_ONLY_CAPS
    if ( ! (*it).IsCapture() ){
      //LOG(logDEBUG) << "Only capture node is interesting here!";
      continue;
    }
#endif

#ifndef QUIESCE_WITHOUT_CHECK_VALIDATION
    if ( ! Definitions::do_trustedGenerator && ! UtilMove::ValidateCheckStatus(*it,p) ) continue;
    if ( ! Definitions::do_trustedGenerator) UtilMove::ValidateIsCheck(*it,p);
#endif

    validThreatFound = true;

    // this is because, even if no bestmove is found > alpha, insert something in TT with score alpha ...
    if ( isFirstMove ) bestmove = &*it;

    // =====================
    // Apply the current move
    // =====================
    // copy / make
    Position p2 = ApplyMoveCopyPosition(p, *it, false);

    DepthType nextDepth = depthleft - 1;
    ScoreType val = - NegaQuiesce(-beta, -alpha, nextDepth , p2, color, fromPV, true);

    if (val > alpha) {

        alphaRaised = true;
        bestmove    = &*it;
        alpha       = val;

        // =====================
        // Beta cut-off
        // =====================
        if (val >= beta){

          if ((Definitions::ttConfig.do_transpositionTableQuiesce||Definitions::ttConfig.do_transpositionTableQSort)
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
          if ( ! Definitions::algo.do_quiesceminimax ){
            ++Stats::qbetacut;
            return Definitions::algo.do_failsoftquiesce?val:beta;
          }
        }
    }
  }

  if ( validThreatFound ){
    // ttable insert
    if ((Definitions::ttConfig.do_transpositionTableQuiesce||Definitions::ttConfig.do_transpositionTableQSort) &&
            !stopFlag && bestmove){
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
    }
    return alpha;
  }

  ///@todo InsertTTQ ? (there was no valid capture !)
  return alpha;
}
