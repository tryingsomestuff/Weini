#include "search.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "ttable.h"
#include "book.h"
#include "line.h"
#include "sort.h"

#include <iterator>
#include <iomanip>

namespace {

    inline
    void UpdateMoveStat(const Position &            p,
                        const Move &                m,
                        const FastContainer<Move> & moves,
                        Searcher &                  searcher,
                        DepthType                   depthleft,
                        bool                        allowNullMove){

        const bool whiteToMove = p.WhiteToPlay();

        const Move::HashType moveHash = m.ZHash();

        // =====================
        // killer heuristic (only is "new" killer is found !)
        // =====================
        if ( Definitions::sortingConfig.do_killerHeuristic && !m.IsCapture() && !m.IsPromotion()){
          const int currentPly = p.CurrentPly();
          if (whiteToMove){
            if ( searcher.killerMovesWhite[0][currentPly] != moveHash){
               searcher.killerMovesWhite[1][currentPly] = searcher.killerMovesWhite[0][currentPly]; // swap
               searcher.killerMovesWhite[0][currentPly] = moveHash;
            }
          }
          else{
            if ( searcher.killerMovesBlack[0][currentPly] != moveHash){
               searcher.killerMovesBlack[1][currentPly] = searcher.killerMovesBlack[0][currentPly]; // swap
               searcher.killerMovesBlack[0][currentPly] = moveHash;
            }
          }
        }

        // =====================
        // counter move heuristic
        // =====================
        if (Definitions::sortingConfig.do_counterHeuristic && !m.IsCapture()
                && m.Type() == UtilMove::sm_standard) { // ensure from and to are valid ... (so skip castling also, and promotions)
            searcher.counterMove[p.Info().lastMove.from][p.Info().lastMove.to] = moveHash;
        }

        // =====================
        // history heuristic (history increase)
        // =====================
        if (Definitions::sortingConfig.do_historyHeuristic
                && !m.IsCapture() && Square::IsValid(m.From())
                && allowNullMove) {
            const int idx = TypeToInt(p.Get(m.From()));
            const Square::LightSquare to = m.To();
            searcher.searchHistory[idx][to] += depthleft*depthleft*4
                    - searcher.searchHistory[idx][to] * depthleft*depthleft*4 / Searcher::historyMax;
            //searcher.searchHistory[idx][to] += depthleft*depthleft*4;
            if (searcher.searchHistory[idx][to] > Searcher::historyMax){
                searcher.searchHistory[idx][to] = Searcher::historyMax;
            }
        }

        // =====================
        // history heuristic (history decrease for all (quiet) previous moves)
        // =====================
        if ( Definitions::sortingConfig.do_historyHeuristic && allowNullMove){
            for (auto it = moves.begin() ; it != moves.end() && (*it).ZHash() != m.ZHash(); ++it){
               if (!(*it).IsCapture() && Square::IsValid((*it).From())){
                  const int idx = TypeToInt(p.Get((*it).From()));
                  const Square::LightSquare to = (*it).To();
                  searcher.searchHistory[idx][to] += - depthleft*depthleft*4
                          - searcher.searchHistory[idx][to] * depthleft*depthleft*4 / Searcher::historyMax;
                  //searcher.searchHistory[idx][to] -= depthleft*depthleft*4;
                  if ( searcher.searchHistory[idx][to] < -1*(int)Searcher::historyMax){
                      searcher.searchHistory[idx][to] = -1*(int)Searcher::historyMax;
                  }
               }
            }
        }
    }

    inline
    bool ManageExtensionNegamax(DepthType &                 depth,
                                Position &                  previousP,
                                const Position &            currentP,
                                const Move &                move,
                                const FastContainer<Move> & moves,
                                bool                        isPV,
                                bool                        bestMoveFound,
                                const Transposition &       ttt,
                                Searcher &                  searcher) {

        unsigned short int nbExtension = 0;
        bool extended = false;

        const Square::LightSquare lastCaptureSquare = previousP.Info().lastMoveWasCapture() ? previousP.Info().lastMove.to : Square::Invalide;
        const Piece::eType lastCapturePiece         = previousP.Info().lastMoveWasCapture() ? previousP.Info().lastCapturedPiece : Piece::t_empty;

        if (Definitions::extensionConfig.do_CheckExtensionAlphaBeta
            && move.IsCheck()) {
            ++depth;
            extended = true;
            ++Stats::countCheckExtensionAlphaBeta;
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (Definitions::extensionConfig.do_ReCaptureExtensionAlphaBeta
            && isPV
            && Square::IsValid(lastCaptureSquare)
            && move.To() == lastCaptureSquare
            && Piece::ValueAbs(currentP.Info().lastCapturedPiece) - Piece::ValueAbs(lastCapturePiece) < 100) {
            ++depth;
            extended = true;
            ++Stats::countReCaptureExtensionAlphaBeta;
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (Definitions::extensionConfig.do_SingleReplyExtensionAlphaBeta
            && moves.size() == 1) {
            ++depth;
            extended = true;
            ++Stats::countSingleReplyExtensionAlphaBeta;
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (Definitions::extensionConfig.do_NearPromotionExtensionAlphaBeta
            && Move::IsNearPromotion(previousP, previousP.Info().lastMove.to)) {
            ++depth;
            extended = true;
            ++Stats::countNearPromotionExtensionAlphaBeta;
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (Definitions::extensionConfig.do_NearPromotionExtensionAlphaBeta
            && Move::IsNearPromotion(currentP, currentP.Info().lastMove.to)) {
            ++depth;
            extended = true;
            ++Stats::countNearPromotionExtensionAlphaBeta;
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (Definitions::extensionConfig.do_SingularExtensionAlphaBeta
            && depth >= 8
            && ttt.depth >= depth - 3
            && ttt.t_type == Transposition::tt_alpha
            && isPV
            && bestMoveFound
            && ! extended) {
            ScoreType rBeta = std::max(ttt.score - 2 * depth, -Definitions::scores.checkMateScore);
            // To verify this we do a reduced search on all the other moves but the tt one and if the
            // result is lower than tt score minus a margin then we will extend the tt move ONLY.
            FastContainer<Move> moves2(moves);
            moves2.erase(moves2.begin());
            SearcherBase::SearchedMove swallowValue = searcher.SearchRoot(rBeta - 1, rBeta, moves2, previousP, depth / 2,
                                                                          NULL,   // never update pv here
                                                                          false,  // forbid nullmove
                                                                          false,  // no subject to time control
                                                                          true,   // forbid extension
                                                                          false); // given moves are NOT trusted
            if (SearcherBase::GetMove(swallowValue).IsValid() && SearcherBase::GetScore(swallowValue) < rBeta) {
                ++depth;
                extended = true;
                ++Stats::countSingularExtensionAlphaBeta;
                ++nbExtension;
                if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
            }
        }

        return extended;
    }
}

ScoreType Searcher::Negamax(ScoreType       alpha,
                            ScoreType       beta,
                            DepthType       depthleft,
                            Position &      p,
                            NSColor::eColor color,
                            DepthType       iterativeDeepeningDepth,
                            bool            fromPV,
                            Line *          pv,
                            //DepthType       depthSinceLastNullMove,
                            bool            allowNullMove,
                            bool            timeControl,
                            bool            forbidExtension){

  if (iterativeDeepeningDepth < 0) {
    LOG(logWARNING) << "Negative iterativeDeepeningDepth in Negamax, this should not append";
    iterativeDeepeningDepth = 0;
  }

  if (depthleft < 0) {
      LOG(logWARNING) << "Negative depthleft in Negamax, this should not append";
      depthleft = 0;
  }

  // =====================
  // forward stop order
  // =====================
  if (stopFlag && timeControl) {
    return Definitions::scores.stopFlagScore;
  }

  // =====================
  // trigger stop order if needed
  // =====================
  int msec = TimeMan::Instance().ElapsedTicMS();
  if ( timeControl && msec >= _allowedThinkTime) {
    LOG(logINFO) << ">> TC endeed in AlphaBeta";
    // abord alphabeta immediatly
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

  // =====================
  // who's playing ?
  // =====================
  const bool whiteToMove = p.WhiteToPlay();
  //const bool isMax = p.Turn() == color;
  //const bool maxIsWhite = color == NSColor::c_white;

  //const bool neverNullMove = (depthSinceLastNullMove == -1);
  ///@todo Great influence of this parameter !!!
  //const bool allowNullMove = (depthSinceLastNullMove > 0 && depthleft > 4);  // (a.k.a recursive nullmove... )

  // =====================
  // forbid quiesce if currently in check
  // =====================
  if ( p.IsInCheck() && depthleft <= 0 ){
    depthleft=1;
  }

  // =====================
  // Check for draw ! (before lauching qsearch)
  // =====================
  if ( Searcher::IsForcedDraw(game, p, 1, Definitions::debugConfig.modedebug)){
    //LOG(logWARNING) << "Negamax found a draw, phase is " << (int)p.GamePhase();
    return p.IsEndGame()?0:-Definitions::scores.contempt;
  }

  // =====================
  // Quiescent search at max depth
  // =====================
  if( depthleft <= 0 ){ // max depth reached
    // clear pv
    Line::ClearPV(pv);

    // this is what we call a leaf node
    ++Stats::leafNodes;

    const DepthType qdepth = Definitions::algo.do_quiesce?(int)Definitions::selectivity.quies_max:0;
    //LOG(logINFO) << "Q call for " << p.GetFEN();
    return NegaQuiesce( alpha, beta, qdepth,  p, color, fromPV, true );
  }

  // this is what we call a "visited" node
  ++Stats::visitednodes;
  // update also current node count
  // this one is used for GUI display purpose
  ++Stats::currentNodesCount;

  // =====================
  // Transposition table look-up
  // =====================
  ///@todo be carefull with mate distance
  Transposition ttt;
  if ( (Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
       && Transposition::GetTT(PHASH(p),depthleft,alpha,beta,ttt)) {
    // in pv, only use exact hit inside ]alpha;beta[
    if (Definitions::ttConfig.do_transpositionTableAlphaBeta
            && (!fromPV ||
                (ttt.t_type == Transposition::tt_exact
                   && ttt.score > alpha
                   && ttt.score < beta))) {
      if ( ttt.move == 0 ){
          LOG(logFATAL) << "TT move hash is null !";
      }

      Move ttmove(Move::HashToMini(ttt.move));
#ifdef WITH_MEMORY_POOL_BUILDER
      ScopeClone<Position, _default_block_size> psc(p);
      Position & p2 = psc.Get();
#else
      Position p2(p);
#endif

      ///@todo those checks looks quite shitty to me !

      UtilMove::CheckForCapture(ttmove, p2);
      if ( UtilMove::PossibleEnPassant(ttmove,p2) && !UtilMove::ValidateEnPassant(ttmove,p2)){
          LOG(logFATAL) << "TT move lead seems to be not a legal en-passant in this position " << p.GetFEN() << ", move was " << ttmove.Show();
      }

      // in case TT move was tagged ep but is not en-passant in the current position
      if ( ttmove.Type() == UtilMove::sm_ep && !ttmove.IsCapture() ){
          LOG(logFATAL) << "TT move tagged en-passant, but not valid in this position " << p.GetFEN() << ", move was " << ttmove.Show();
      }

      else{ // ok ...
          if ( !UtilMove::ValidateCheckStatus(ttmove,p2) ){
              LOG(logFATAL) << "TT move lead seems not legal in this position " << p.GetFEN() << ", move was " << ttmove.Show();
          }
          // UtilMove::ValidateIsCheck(ttmove,p2);

          ApplyMove(p2,ttmove); // game hash is updated

          // is tt move leading to a draw ?
          if (IsForcedDraw(game, p2, 1, Definitions::debugConfig.modedebug) /*&& ttt.score > Definitions::contempt*/) {
             LOG(logDEBUG) << "TT move lead to draw";
             //return p.IsEndGame() ? 0 : -Definitions::contempt;
          }
          else {
              ++Stats::ttHitUsed;
              Line::ClearPV(pv);
    #ifdef DEBUG_TTHASH
              LOG(logINFO) << p.GetFEN() << " tt hit " << ttt.score << " " << ttt.move << " " << ttt.depth << " >= " << depthleft << " - " << ttt.type;
    #endif

              if ( ttt.score > beta ){
                  static FastContainer<Move> empty;
                  UpdateMoveStat(p,ttmove,empty,*this,depthleft,allowNullMove);
              }

              return ttt.score;
          }
      }
    }
  }

  ScoreType alreadyComputedStandPat = 0; // can be a lazy scoring, a classic scoring, or the tt score
  bool standPatComputed = false;
  bool pruningAllowed = false;
  bool moveCountPruningAllowed = false;
  DepthType reduction = 0;

  // Use tt score when possible
  /*
  if (ttt.hash != 0 &&
           ( ( ttt.t_type != Transposition::tt_alpha  && (ttt.score > alpha) )
          || ( ttt.t_type != Transposition::tt_beta   && (ttt.score < beta) ) ) ){
     alreadyComputedStandPat = ttt.score;
     standPatComputed = true;
  }
  */

  // =====================
  // all type of pruning are performed only if we are far from mate.
  // and not in minimax test mode
  // and not in check
  // =====================
  if (/*! p.isEndGame() &&*/
      beta  >= - Definitions::scores.checkMateScore + MAX_GAME_PLY &&     // we are not near to be checkmated
      alpha <=   Definitions::scores.checkMateScore - MAX_GAME_PLY &&     // we are not near to checkmate
      !Definitions::algo.do_alphabetaminimax &&
      !p.IsInCheck()){

    // =====================
    // Static null move (adaptative reduction or forward-eventually-verified-pruning)
    // > beta + margin
    // =====================
    if (Definitions::algo.do_staticNullMove &&
          //alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
          !fromPV &&
          depthleft <= Definitions::selectivity.staticNullMoveMaxDepth &&
          iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
          allowNullMove &&
          //! p.Info().lastMoveWasCapture() &&
          !p.IsEndGame()
          ) {
          alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                     : Analyze(p, Definitions::algo.do_lazyEvaluation);
          standPatComputed = true;
          ScoreType margin = Definitions::selectivity.staticNullMoveMargin
                           + Definitions::selectivity.staticNullMoveDepthCoeff*depthleft;

          // extremely too good situation, a beta cut-off will allmost certainly occur ==> return the value immediatly
          if (alreadyComputedStandPat - /*3 **/ margin >= beta) {
              ++Stats::staticNullMovebetacutH;
              return Definitions::algo.do_failsoftalphabeta ? (alreadyComputedStandPat /*- 3 * margin*/) : beta;
          }
          /*
          // too too good situation, a beta cut-off shall occur ==> directly to QSearch
          else if (alreadyComputedStandPat - 2 * margin >= beta) {
              ++Stats::staticNullMovebetacutQtry;
              // verification qsearch...
              ScoreType rBeta = beta + margin;
              ScoreType val = NegaQuiesce(rBeta - 1, rBeta, (int)Definitions::selectivity.quies_max, p, color, fromPV, false);
              if (val > rBeta) {
                  ++Stats::staticNullMovebetacutQ;
                  return Definitions::algo.do_failsoftalphabeta ? val : beta;
              }
          }
          // too good situation, a beta cut-off may occur ==> reduce depth
          if (alreadyComputedStandPat - margin >= beta) {
              ++Stats::staticNullMovebetacut;
              int razoring_id = ReductionId(depthleft, depthleft, MAX_REDUCTION_DEPTH);
              reduction = razoring_reduction[razoring_id];
          }
          */
    }

    // =====================
    // Futility pruning (see pruningAllowed)
    // < alpha - margin
    // =====================
    if (Definitions::algo.do_futilityPruning &&
        //alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        !fromPV &&
        depthleft <= Definitions::selectivity.futilityMaxDepth &&
        iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
        allowNullMove &&
        //!p.IsEndGame() &&
        //! p.Info().lastMoveWasCapture() && // no futility after a capture
        !Move::IsNearPromotion(p, p.Info().lastMove.to)) { // to not perform futility pruning just before promotion

        alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                   : Analyze(p, Definitions::algo.do_lazyEvaluation);
        standPatComputed = true;
        ScoreType margin = Definitions::selectivity.futilityMargin
                         + Definitions::selectivity.futilityDepthCoeff*depthleft;

        /*
        if (alreadyComputedStandPat + 3 * margin < alpha) { // no chance to raise alpha
            return Definitions::algo.do_failsoftalphabeta ? (alreadyComputedStandPat + 3 * margin) : alpha;
        }
        else*/ if (alreadyComputedStandPat + /*2 **/ margin <= alpha) {
            // verification qsearch...
            ScoreType val = NegaQuiesce(alpha, beta, (int)Definitions::selectivity.quies_max, p, color, fromPV, false);
            if (val <= alpha) {
                return Definitions::algo.do_failsoftalphabeta ? std::max(val, alreadyComputedStandPat) : alpha;
            }
            //pruningAllowed = true;
        }
        /*
        if (depthleft <= 2 && alreadyComputedStandPat + margin <= alpha) {
            ++Stats::futilitycut;
            pruningAllowed = true;
        }
        */
    }

    // =====================
    // Null move pruning
    // =====================
    if (Definitions::algo.do_nullMovePruning &&
        // alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        // !fromPV &&
        allowNullMove &&
        depthleft >= Definitions::selectivity.nullMovePruningMinDepth &&
        !p.IsEndGame() &&
        !Square::IsValid(p.Info().enPassant) //&& //protecting from hash issue in swithPlayer !!! ///@todo debug !!!!!
        // !p.Info().lastMoveWasCapture
        ) {

      alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                 : Analyze(p, Definitions::algo.do_lazyEvaluation);
      standPatComputed = true;

      // performing null-move pruning only if stand-pat suggess position is bad already
      if ( alreadyComputedStandPat - Definitions::selectivity.nullMoveMargin >= beta ) {

          ++Stats::nullmovecuttry;

#ifdef WITH_MEMORY_POOL_BUILDER
        ScopeClone<Position, _default_block_size> psc(p);
        Position & p2 = psc.Get();
#else
        Position p2(p);
#endif
        p2.SwitchPlayer(); // same player get to play again
        //LOG(logINFO) << "Null move lookup";

        //DepthType R = 2 + (depthleft>=3) + (depthleft>=6) + (depthleft>=9) + (depthleft>=13) // adaptative null-move pruning
        //                + (Definitions::algo.do_verifiedNullMove?(int)std::min((alreadyComputedStandPat - beta) / Piece::Value(Piece::t_Wpawn), 3):0); // also based on eval if verified !
        DepthType R = depthleft/4 + 3;

        ///@todo dynamic reduction +1 is retBase > beta + Pawn

        bool tryNull = true;

        // omit null move search if normal search to the same depth wouldn't exceed beta
        // (sometimes we can check it for free via hash table)
        Transposition tttNull;
        if ( (Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
             && Transposition::GetTT(PHASH(p),depthleft-R,alpha,beta,tttNull)) { // we use p (not p2) here on purpose
            if (tttNull.score < beta) tryNull = false;
        }

        if ( tryNull ){

            ScoreType nullScore = (depthleft-R-1>0) ? -Negamax(-beta, -beta+1, depthleft - R - 1,
                                                               p2, color, iterativeDeepeningDepth/*0*/,
                                                               false, NULL, /*0*/ false, false, true)
                                                    : -NegaQuiesce(-beta, -beta+1, (int)Definitions::selectivity.quies_max,
                                                               p2, color,
                                                               false, false);

            // do not return mate score
            if (nullScore > Definitions::scores.checkMateScore - MAX_GAME_PLY) {
                nullScore = beta;
            }

            if (nullScore /*- Definitions::nullMoveMargin*/ >= beta) { // too good move

              // verified nullmove at high R
              if (Definitions::algo.do_verifiedNullMove && R >= Definitions::selectivity.verifiedNullMoveMinDepth) {
                  ++Stats::nullmoveverification;
                  nullScore = (depthleft - R -1 > 0) ?  Negamax(beta-1, beta, depthleft - R - 1, p, color, iterativeDeepeningDepth/*0*/,
                                                          false, NULL, /*0*/ false, false, true)
                                                :  NegaQuiesce(beta-1, beta, (int)Definitions::selectivity.quies_max, p, color,
                                                          false, false);
                  if (nullScore /*- Definitions::nullMoveMargin*/ >= beta) { // too good move
                      //LOG(logINFO) << "Null move pruning, beta was " << beta << " null move gets " << nullMove.second;
                      ++Stats::nullmovecutbeta;
                      ++Stats::nullmovecutafterverification;
                      return Definitions::algo.do_failsoftalphabeta ? nullScore : beta;
                  }
              }
              else{
                  //LOG(logINFO) << "Null move pruning, beta was " << beta << " null move gets " << nullMove.second;
                  ++Stats::nullmovecutbeta;
                  return Definitions::algo.do_failsoftalphabeta ? nullScore : beta;
              }
            }
            else {
                ///@todo something when null move fail low ?
            }
        }
        else{
            ++Stats::nullmovecutskipped;
        }
      }
    }

    // =====================
    // Null move reduction
    // =====================
    ///@todo bigger depth, do not trigger cut-off but a reduction

    // =====================
    // ProbCut
    // =====================
    ///@todo ProbCut

    // =====================
    // Move count pruning (see moveCountPruningAllowed)
    // =====================
    if (Definitions::algo.do_moveCountPruning &&
        //alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        //!fromPV &&
        depthleft <= Definitions::selectivity.moveCountPruningMaxDepth &&
        iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
        !p.IsEndGame() //&&
        // !Move::IsNearPromotion(p, p.Info().lastMove.to)
        ){
        moveCountPruningAllowed = true;
    }

  } // end of early pruning ...

  // =====================
  // Move ordering
  // =====================
  FastContainer<Move> moves;
  MoveGenerator(Definitions::algo.do_trustedGenerator).Generator(p,moves);

  bool bestMoveFound = false; // is the first move coming from TT (or PV, not from IID)

  if (! moves.empty()){
     bestMoveFound = Move::Ordering(moves,p,*this,fromPV,depthleft,Definitions::ttConfig.do_transpositionTableSortAlphaBeta?(&ttt):NULL,pv);
  }
  else{ // no moves found...
      // =====================
      // Early _exit for checkmate and stalemate
      // !!!! : can/will be missed because of pseudo legal move generator !
      // =====================
      if ( p.IsInCheck() ){ // mate
          //LOG(logINFO) << "Check mate " << p.GetFEN();
          Line::ClearPV(pv);
          return - (Definitions::scores.checkMateScore - p.Moves());
      }
      else{ // stalemate
          //LOG(logINFO) << "Stale mate " << p.GetFEN();
          Line::ClearPV(pv);
          return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
      }
  }

  // =====================
  // Alpha-beta search
  // =====================
  bool validMoveFound = false;
  Move * bestMove = NULL;

  const float gamephase = p.GamePhasePercent()/100.f;
  DepthType lmr_start = (DepthType)((1-gamephase) * Definitions::selectivity.LMR_ENDGAME
                                      + gamephase * Definitions::selectivity.LMR_STD); // tappered LMR depth
  DepthType lmr_count = 0;
  bool alphaRaised = false;
  bool alphaJustRaised = false;

  // local pv pointer is not null only if pv is not.
  Line pv_loc_best_obj;
  Line pv_loc_obj;
  Line * pv_loc_best = 0;
  Line * pv_loc = 0;
  if (pv) {
      pv_loc_best = &pv_loc_best_obj;
      pv_loc = &pv_loc_obj;
  }

  // =====================
  // let nullmove happen again (a.k.a recursive nullmove... )
  // =====================
  if ( (!allowNullMove) /*&& depthleft > Definitions::selectivity.recursiveNullMoveMinDepth*/) {
      ++Stats::nullmoveReset;
      allowNullMove = true;
  }

  // =====================
  // Iterate through moves
  // =====================
  unsigned int moveCount = 0;
  //bool fullOrdered = false;
  for (auto it = moves.begin() ; it != moves.end() ; ++it, ++moveCount){

    /*
    ///@todo make partial sorting work ...
    // sort last moves if no cut-off occurs yet
    if ( moveCount >= SORT_FIRST_TIME_LIMIT && !fullOrdered){
       fullOrdered = true;
       Sort::SortMoves(moves, p, *this, Sort::SP_last);
    }
    */

    // =====================
    // in case alpha is updated, we re-count lmr moves from 1
    // =====================
    if (alphaJustRaised) {
      lmr_count = 1;
      alphaJustRaised = false;
    }

    bool isFirstMove = lmr_count == 0;
    bool isPV = fromPV && isFirstMove;

    Line::ClearPV(pv_loc);

    // =====================
    // Validate move
    // =====================
    ///@todo I think the pv move is always valid ! maybe validation can be skipped ?
    if ( ! Definitions::algo.do_trustedGenerator && ! UtilMove::ValidateCheckStatus(*it,p) ) continue;
    if ( ! Definitions::algo.do_trustedGenerator) UtilMove::ValidateIsCheck(*it,p);
    validMoveFound = true;

    // this is because, even if no bestmove is found > alpha,
    // we insert something in TT with score alpha ...
    if ( isFirstMove ) bestMove = &*it;

    bool isSilentMove = !(*it).IsCapture()
                     && !(*it).IsCheck()
                     && !(*it).IsNearPromotion(p)
                     && !(*it).IsPromotion()
                     && !(*it).IsCastling();

    bool isSilentMove2 = !(*it).IsCapture()
                      && !(*it).IsCheck()
                      //&& !(*it).IsNearPromotion(p)
                      //&& !(*it).IsPromotion()
                      && !(*it).IsCastling()
                      && !(*it).IsAdvancedPawnPush(p);

    bool isSilentCapture = (*it).IsCapture()
                        && !(*it).IsCheck()
                        && !(*it).IsNearPromotion(p)
                        && !(*it).IsPromotion()
                        && !(*it).IsAdvancedPawnPush(p);

    // =====================
    // Prune moves with bad SEE
    // =====================
    if ( Definitions::algo.do_seePruningAlphaBeta
         && !fromPV
         && !isFirstMove
         && depthleft <= Definitions::selectivity.seePruningAlphaBetaMaxDepth
         //&& pruningAllowed
         && validMoveFound // cannot prune until a valid move is found in order to return something ...
         && isSilentCapture
         //&& !SEE(p, *it, 0)
         && (*it).SortScore() < -Definitions::scores.captureSortScore + 8 - depthleft) {
            ++Stats::seecutAlphaBeta;
            continue; // just skip it !
    }

    // =====================
    // prune some moves if possible (futility)
    // =====================
    if (pruningAllowed
         && !fromPV
         && !isFirstMove
         && validMoveFound // cannot prune until a valid move is found in order to return something ...
         && isSilentMove
         ){
        ++Stats::futilityMovePruned;
        continue; // just skip it !
    }

    // =====================
    // prune some moves if possible (move count pruning)
    // =====================
    static const DepthType lmp_min[11] = { 0, 5, 6, 9, 15, 23, 32, 42, 54, 68, 83 }; // xiphos
    if (moveCountPruningAllowed
        && !fromPV
        && !isFirstMove
        && validMoveFound // cannot prune until a valid move is found in order to return something ...
        && isSilentMove
        && (int(lmr_count) >= lmp_min[depthleft] /*int(0.84*depthleft*depthleft-0.45*depthleft + 3.83)*/ )
        ){
        ++Stats::moveCountPruned;
        continue; // just skip it !
    }

    // =====================
    // Apply the current move
    // =====================
    // copy / make
    Position p2 = ApplyMoveCopyPosition(p, *it); // game hash is updated

    // =====================
    // look for applicable extensions
    // =====================
    DepthType nextDepth = depthleft-1;
    DepthType nextDepthWithoutExtension = nextDepth;
    if (!forbidExtension){
        ManageExtensionNegamax(nextDepth, p, p2, *it, moves, isFirstMove, bestMoveFound, ttt, *this);
    }
    // ensure depth is not too deep ...
    nextDepth = std::min(nextDepth,DepthType(MAX_SEARCH_DEPTH-1)); // limit highest depth
    DepthType nextDepthWithoutReduction = nextDepth;

    // =====================
    // reduction : LMR
    // =====================
    if ( Definitions::algo.do_lmrAlphabeta
           && lmr_count >= lmr_start
           && isSilentMove2
           && (*it).CanBeReduced() // filter killer, counter, good history ...
           && nextDepth > Definitions::selectivity.lmrMinDepth
           && iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth
           //&& ! fromPV
           /*&& ! p.isEndGame()*/ ){

        int lmr_id = ReductionId(nextDepth,lmr_count,MAX_REDUCTION_DEPTH);
        DepthType R = lmr_reduction[lmr_id];
        // be more carefull in end games
        if ( p.IsEndGame() && R > 2 ) R /= 2;
        // be more carefull for PV parent
        if ( fromPV ) --R;
        nextDepth -= R;
        ++Stats::lmralphabeta;
    }

    // =====================
    // apply other reductions
    // =====================
    if ( ! isFirstMove ){
       nextDepth -= reduction; // computed above ...
    }

    nextDepth = std::max(nextDepth,DepthType(0)); // limit lowest depth

    // will be the return of alphabeta for this move
    ScoreType val = 0;

    // =====================
    // eventually apply PVS
    // =====================
    ///@todo is the last condition needed ??
    bool doPVS = Definitions::algo.do_pvsAlphaBeta && !isPV && alphaRaised;
    ScoreType alpha_loc = alpha;
    ScoreType beta_loc  = (doPVS) ? alpha+1 : beta;

    // =====================
    // classic PVS alpha-beta
    // =====================
research:
    Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
    val = - Negamax( -beta_loc, -alpha_loc, nextDepth, p2, color, iterativeDeepeningDepth,
                     isPV && fromPV,pv_loc, /*depthSinceLastNullMove+1*/ allowNullMove,timeControl, forbidExtension);

    // =====================
    // a reduced search cannot raise alpha !
    // =====================
    if ( nextDepthWithoutReduction != nextDepth && val > alpha ){
        if ( reduction == 0 ){
           ++Stats::lmralphabetafail;
        }
        else{
           ++Stats::failedReduction;
        }
        nextDepth = nextDepthWithoutReduction;
        goto research;
    }

    if(doPVS){
      if ( (val > alpha && val < beta) ){ // PVS fail (fail-soft framework)
        ++Stats::pvsAlphaBetafails;
        Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
        val = - Negamax( -beta, -alpha, nextDepth , p2, color,iterativeDeepeningDepth,
                         true,pv_loc, /*depthSinceLastNullMove+1*/ allowNullMove,timeControl, forbidExtension); //can be a new pv
      }
      else{
          ++Stats::pvsAlphaBetasuccess;
      }
    }

    ++lmr_count;

    // =====================
    // update move score (this is totally useless, unless generated moves is inside a static cache indexed by position hash ...)
    // =====================
    //(*it).SetSortScore(val);

    // =====================
    // update best score
    // =====================
    if ( val > alpha ){

        bestMove = &*it;

        if (pv) {
            *pv_loc_best = *pv_loc;
        }

        // =====================
        // Beta cut-off
        // =====================
        if (val >= beta){

          UpdateMoveStat(p,*it,moves,*this,depthleft,allowNullMove);

          // =====================
          // ttable insert
          // =====================
          if ((Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
                  && !stopFlag) {
    #ifdef DEBUG_TTHASH
            LOG(logINFO) << p.GetFEN() << " tt insert " << alpha << " " << (*it).Show() << " " << nextDepth << " - " << "beta " << beta;
    #endif

    #ifdef TT_DEBUG_HASH_COL
            if (!CheckFenHashCol(p2.GetFENShort2(), PHASH(p2), p2)) {
              LOG(logFATAL) << "Hash test failed";
            }
    #endif
            Transposition::InsertTT(PHASH(p), beta, depthleft, Transposition::tt_beta,(*it).ZHash()
    #ifdef DEBUG_TT_SORT_FAIL
                , p.GetFEN()
    #endif
            );
          }

          if ( ! Definitions::algo.do_alphabetaminimax ){
            ++Stats::betacut;
            // =====================
            // update pv
            // =====================
            if ( pv ) Line::UpdatePV(pv,*pv_loc,it->GetMini(),p);
            return (Definitions::algo.do_failsoftalphabeta?val:beta);
          }
        }

        // =====================
        // update alpha
        // =====================
        alpha = val;
        alphaRaised = true;
        alphaJustRaised = true;
    }

  }

  if ( ! validMoveFound ){

    ///@todo can we insert something in TT ?

    // clear pv
    Line::ClearPV(pv);
    //LOG(logDEBUG) << "Checkmate or stalemate not detected, no valid move !!! (because of pseudo move generator)" ;
    if ( p.IsInCheck() ){ // mate
      return - (Definitions::scores.checkMateScore - p.Moves());
    }
    else{ // stalemate
      return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
    }
  }

  // =====================
  // ttable insert
  // =====================
  if ((Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
          && bestMove && ! stopFlag) {
#ifdef DEBUG_TTHASH
    LOG(logINFO) << p.GetFEN() << " tt insert " << alpha << " " << (*bestMove).Show() << " " << depthleft-1 << " - " << "exact";
#endif
#ifdef TT_DEBUG_HASH_COL
    if (!CheckFenHashCol(p.GetFENShort2(), PHASH(p), p)) {
        LOG(logFATAL) << "Hash test failed";
    }
#endif
    Transposition::InsertTT(PHASH(p), alpha, depthleft, alphaRaised?Transposition::tt_exact : Transposition::tt_alpha, (*bestMove).ZHash()
#ifdef DEBUG_TT_SORT_FAIL
        , p.GetFEN()
#endif
    );
  }

  // =====================
  // update pv
  // =====================
  if ( bestMove && pv ){
     Line::UpdatePV(pv,*pv_loc_best,bestMove->GetMini(),p);
  }

  return alpha;
}
