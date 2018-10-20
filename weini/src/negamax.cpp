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
                        bool                        allowPruningAndReduction){

        const bool whiteToMove = p.WhiteToPlay();

        const Move::HashType moveHash = m.ZHash();

        // =====================
        // killer heuristic (only is "new" killer is found !)
        // =====================
        if ( OPTIM_BOOL_TRUE(Definitions::sortingConfig.do_killerHeuristic)
             && !m.IsCapture()
             && !m.IsPromotion()){
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
        if ( OPTIM_BOOL_TRUE(Definitions::sortingConfig.do_counterHeuristic)
             && !m.IsCapture()
             && m.Type() == UtilMove::sm_standard) { // ensure from and to are valid ... (so skip castling also, and promotions)
          searcher.counterMove[searcher.game.Info(p.CurrentPly()).lastMove.from][searcher.game.Info(p.CurrentPly()).lastMove.to] = moveHash;
        }

        // =====================
        // history heuristic (history increase)
        // =====================
        if (OPTIM_BOOL_TRUE(Definitions::sortingConfig.do_historyHeuristic)
                && !m.IsCapture() && Square::IsValid(m.From())
                && allowPruningAndReduction) { ///@todo is this condition necessary
            const int idx = TypeToInt(p.Get(m.From()));
            const Square::LightSquare to = m.To();
            searcher.searchHistory[idx][to] += depthleft*depthleft*4
                                             - searcher.searchHistory[idx][to] * depthleft*depthleft*4 / Searcher::historyMax;
            if (searcher.searchHistory[idx][to] > Searcher::historyMax){
                searcher.searchHistory[idx][to] = Searcher::historyMax;
            }
        }

        // =====================
        // history heuristic (history decrease for all (quiet) previous moves)
        // =====================
        if ( OPTIM_BOOL_TRUE(Definitions::sortingConfig.do_historyHeuristic)
                && allowPruningAndReduction){ ///@todo is this condition necessary
            for (auto it = moves.begin() ; it != moves.end() && (*it).ZHash() != m.ZHash(); ++it){
               if (!(*it).IsCapture() && Square::IsValid((*it).From())){
                  const int idx = TypeToInt(p.Get((*it).From()));
                  const Square::LightSquare to = (*it).To();
                  searcher.searchHistory[idx][to] += - depthleft*depthleft*4
                                                   - searcher.searchHistory[idx][to] * depthleft*depthleft*4 / Searcher::historyMax;
                  if ( searcher.searchHistory[idx][to] < -1*(int)Searcher::historyMax){
                      searcher.searchHistory[idx][to] = -1*(int)Searcher::historyMax;
                  }
               }
            }
        }

        /*
        for(int k = 0 ; k < 15 ; ++k){
            for(int p = 0 ; p < BOARD_SIZE ; ++p){
                std::cout << searcher.searchHistory[k][p] << std::endl;
            }
            std::cout << "---" << std::endl;
        }
        std::cout << "**********" << std::endl;
        */

    }

    inline
    bool ManageExtensionNegamax(DepthType &                 depth,
                                const PlyType               ply,
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

        const Square::LightSquare lastCaptureSquare = searcher.game.Info(previousP.CurrentPly()).lastMoveWasCapture() ? searcher.game.Info(previousP.CurrentPly()).lastMove.to       : Square::Invalide;
        const Piece::eType lastCapturePiece         = searcher.game.Info(previousP.CurrentPly()).lastMoveWasCapture() ? searcher.game.Info(previousP.CurrentPly()).lastCapturedPiece : Piece::t_empty;

        if (OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_CheckExtensionAlphaBeta)
            && move.IsCheck()) {
            ++depth;
            extended = true;
            OPTIM_STATS(++Stats::countCheckExtensionAlphaBeta;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_ReCaptureExtensionAlphaBeta)
            && isPV
            && Square::IsValid(lastCaptureSquare)
            && move.To() == lastCaptureSquare
            && Piece::ValueAbs(searcher.game.Info(currentP.CurrentPly()).lastCapturedPiece) - Piece::ValueAbs(lastCapturePiece) < 100) {
            ++depth;
            extended = true;
            OPTIM_STATS(++Stats::countReCaptureExtensionAlphaBeta;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_SingleReplyExtensionAlphaBeta)
            && moves.size() == 1) {
            ++depth;
            extended = true;
            OPTIM_STATS(++Stats::countSingleReplyExtensionAlphaBeta;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_NearPromotionExtensionAlphaBeta) // previous move
            && Move::IsNearPromotion(previousP, searcher.game.Info(previousP.CurrentPly()).lastMove.to)) {
            ++depth;
            extended = true;
            OPTIM_STATS(++Stats::countNearPromotionExtensionAlphaBeta;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_NearPromotionExtensionAlphaBeta) // current move
            && Move::IsNearPromotion(currentP, searcher.game.Info(currentP.CurrentPly()).lastMove.to)) {
            ++depth;
            extended = true;
            OPTIM_STATS(++Stats::countNearPromotionExtensionAlphaBeta;)
            ++nbExtension;
            if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
        }

        if (OPTIM_BOOL_FALSE(Definitions::extensionReductionConfig.do_SingularExtensionAlphaBeta)
            && depth >= 8
            && ttt.hash != 0 // we have a hash move
            && ttt.depth >= depth - 3
            && ttt.t_type == Transposition::tt_alpha // so called restricted SE
            //&& isPV
            && bestMoveFound
            && ! extended) {
            ScoreType rBeta = std::max(ttt.score - 2 * depth, -Definitions::scores.checkMateScore);
            // To verify this we do a reduced search on all the other moves but the tt one and if the
            // result is lower than tt score minus a margin then we will extend the tt move ONLY.
            SearcherBase::SearchedMove swallowValue = searcher.SearchRoot(rBeta - 1, rBeta, moves, previousP, depth / 2, ply,
                                                                          NULL,   // never update pv here
                                                                          TimeMan::eTC_on,  // subject to time control ?
                                                                          Searcher::eRedA_on,
                                                                          Searcher::eExtA_off, // recursive singular shall not be allowed
                                                                          (*moves.begin()).ZHash(), // skip the tt move
                                                                          false); // given moves are NOT trusted
            if (SearcherBase::GetMove(swallowValue).IsValid() && SearcherBase::GetScore(swallowValue) < rBeta) {
                ++depth;
                extended = true;
                OPTIM_STATS(++Stats::countSingularExtensionAlphaBeta;)
                ++nbExtension;
                if (nbExtension >= Definitions::selectivity.extensionAlphaBetaMax) return extended;
            }
        }

        return extended;
    }
}


#define HANDLE_TC_ALPHA_BETA \
if (stopFlag && TimeMan::IsTCOn(timeControl)) { \
    return Definitions::scores.stopFlagScore; \
} \
else { \
    int msec = TimeMan::Instance().ElapsedTicMS(); \
    if (TimeMan::IsTCOn(timeControl) && msec >= _allowedThinkTime) { \
        LOG(logINFO) << ">> TC endeed in AlphaBeta"; \
        stopFlag = true; \
        return Definitions::scores.stopFlagScore; \
    } \
}


ScoreType Searcher::Negamax(ScoreType       alpha,
                            ScoreType       beta,
                            DepthType       depthleft,
                            Position &      p,
                            NSColor::eColor color,
                            const DepthType iterativeDeepeningDepth,
                            const PlyType   ply,
                            bool            fromPV,
                            Line *          pv,
                            TimeMan::eTC    timeControl,
                            Searcher::ePruningAndReductionAllowed  allowPruningAndReduction,
                            Searcher::eExtensionAllowed            allowExtension){

  // =====================
  // Update seldepth
  // =====================
  if (Stats::seldepth[iterativeDeepeningDepth] < ply) {
     Stats::seldepth[iterativeDeepeningDepth] = ply;
  }

  if (iterativeDeepeningDepth < 0) {
    LOG(logFATAL) << "Negative iterativeDeepeningDepth in Negamax, this should not append";
    //iterativeDeepeningDepth = 0;
  }

  if (depthleft < 0) {
    LOG(logFATAL) << "Negative depthleft in Negamax, this should not append";
    depthleft = 0;
  }

  HANDLE_TC_ALPHA_BETA

  // =====================
  // protect against to long games
  // =====================
  if ( p.CurrentMoveCount() >= MAX_GAME_PLY ){
      return Analyze(p, false);
  }

  // =====================
  // who's playing ?
  // =====================
  const bool whiteToMove = p.WhiteToPlay();
  //const bool isMax = p.Turn() == color;
  //const bool maxIsWhite = color == NSColor::c_white;

  // =====================
  // No more king !
  // =====================
  if ((whiteToMove && p.NoWhiteKingAnymore()) || ((!whiteToMove && p.NoBlackKingAnymore())) ){
      LOG(logFATAL) << "No more king ! " << p.GetFEN();
  }

  // =====================
  // Forbid QSearch if currently in check
  // =====================
  if ( p.IsInCheck() && depthleft <= 0 ){
    depthleft=1;
  }

  // =====================
  // Check for draw ! (before lauching qsearch)
  // =====================
  if ( Searcher::IsForcedDraw(game, p, analyze.MaterialCache(), fromPV?3:1, Definitions::debugConfig.modedebug)){
    //LOG(logWARNING) << "Negamax found a draw, phase is " << (int)p.GamePhase();
    return p.IsEndGame()?0:-Definitions::scores.contempt;
  }

  // =====================
  // QSearch at max depth
  // =====================
  if( depthleft <= 0 ){ // max depth reached
    // clear pv
    Line::ClearPV(pv);

    // this is what we call a leaf node
    OPTIM_STATS(++Stats::leafNodes;)
    ++Stats::currentLeafNodesCount;

    const DepthType qdepth = Definitions::algo.do_quiesce?(int)Definitions::selectivity.quies_max:0;
    //LOG(logINFO) << "Q call for " << p.GetFEN();
    return NegaQuiesce( alpha, beta, qdepth, p, color, iterativeDeepeningDepth, ply, fromPV, TimeMan::eTC_on );
  }

  // this is what we call a "visited" node
  OPTIM_STATS(++Stats::visitednodes;)
  // update also current node count : this one is used for GUI display purpose
  ++Stats::currentNodesCount;

  // =====================
  // mate distance pruning
  // =====================
  alpha = std::max( - Definitions::scores.checkMateScore + p.CurrentMoveCount()    , alpha);
  beta  = std::min(   Definitions::scores.checkMateScore - p.CurrentMoveCount() +1 , beta);
  if (alpha >= beta) {
      return alpha;
  }

  // =====================
  // Transposition table look-up
  // =====================
  ///@todo be carefull with mate distance
  Transposition ttt;
  Move ttmove;
  if ( OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
       && Transposition::GetTT(PHASH(p),depthleft,alpha,beta,ttt,p.CurrentPly())) {
    // in pv, only use exact hit inside ]alpha;beta[
    if (Definitions::ttConfig.do_transpositionTableAlphaBeta
            && (!fromPV &&
                ( (ttt.t_type == Transposition::tt_exact) ||
                  (ttt.t_type == Transposition::tt_alpha && ttt.score <= alpha) ||
                  (ttt.t_type == Transposition::tt_beta  && ttt.score >= beta )  ) ) ) {
      if ( ttt.move == 0 ){
          LOG(logFATAL) << "TT move hash is null !";
      }

      ttmove = Move(Move::HashToMini(ttt.move));
#ifdef WITH_MEMORY_POOL_BUILDER
      ScopeClone<Position, _default_block_size> psc(p);
      Position & p2 = psc.Get();
#else
      Position p2(p);
#endif

      ///@todo those verifications looks quite shitty to me !

      UtilMove::DetectStandardCapture(ttmove, p2);
      if ( UtilMove::PossibleEnPassant(ttmove,p2) && !UtilMove::ValidateEnPassant(ttmove,p2)){
          LOG(logFATAL) << "TT move lead seems to be not a legal en-passant in this position " << p2.GetFEN() << ", move was " << ttmove.Show();
      }

      // in case TT move was tagged ep but is not en-passant in the current position
      if ( ttmove.Type() == UtilMove::sm_ep && !ttmove.IsCapture() ){
          LOG(logFATAL) << "TT move tagged en-passant, but not valid in this position " << p2.GetFEN() << ", move was " << ttmove.Show();
      }

      else{ // ok ...
          if ( !UtilMove::ValidateCheckStatus(ttmove,p2) ){
              LOG(logFATAL) << "TT move lead seems not legal in this position " << p2.GetFEN() << ", move was " << ttmove.Show();
          }
          UtilMove::ValidateIsCheck(ttmove,p2);

          ApplyMove(p2,ttmove); // game hash is updated

          // is tt move leading to a draw ?
          //if (IsForcedDraw(game, p2, analyze.MaterialCache(), 1, Definitions::debugConfig.modedebug)) {
             //LOG(logDEBUG) << "TT move lead to draw";
             ////return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
          //}
          //else {
              OPTIM_STATS(++Stats::ttHitUsed;)
              Line::ClearPV(pv);
    #ifdef DEBUG_TTHASH
              LOG(logINFO) << p.GetFEN() << " tt hit " << ttt.score << " " << ttt.move << " " << ttt.depth << " >= " << depthleft << " - " << ttt.type;
    #endif

              if ( ttt.score > beta ){
                  static FastContainer<Move> empty;
                  UpdateMoveStat(p,ttmove,empty,*this,depthleft,allowPruningAndReduction==Searcher::eRedA_on);
              }

              return ttt.score;
          //}
      }
    }
  }

  ScoreType alreadyComputedStandPat = 0; // can be a lazy scoring, a classic scoring, or the tt score
  bool standPatComputed             = false;
  bool futilityPruningAllowed       = false;
  bool moveCountPruningAllowed      = false;

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
  // Move list may be needed by ProbCut
  // =====================
  FastContainer<Move> moves;
  bool moveGenerated = false;
  bool bestMoveFound = false; // is the first move coming from TT (or PV, not from IID)

  // =====================
  // Prunings !
  // =====================
  if (/*! p.isEndGame() &&*/
      beta  >= - Definitions::scores.checkMateScore + MAX_GAME_PLY &&     // we are not near to be checkmated
      alpha <=   Definitions::scores.checkMateScore - MAX_GAME_PLY &&     // we are not near to checkmate
      OPTIM_BOOL_TRUE(!Definitions::algo.do_alphabetaminimax) &&
      !p.IsInCheck() &&
      //!fromPV &&
      allowPruningAndReduction == Searcher::eRedA_on){

    // =====================
    // Static null move
    // >= beta + margin
    // =====================
    if (OPTIM_BOOL_TRUE(Definitions::algo.do_staticNullMove) &&
          !fromPV &&
          depthleft <= Definitions::selectivity.staticNullMoveMaxDepth &&
          iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
          !p.IsEndGame()
          ) {
          alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                     : Analyze(p, Definitions::algo.do_lazyEvaluation,fromPV);
          standPatComputed = true;

          ScoreType margin = Definitions::selectivity.staticNullMoveMargin
                           + Definitions::selectivity.staticNullMoveDepthCoeff*depthleft;

          if (alreadyComputedStandPat - margin >= beta) {
              OPTIM_STATS(++Stats::staticNullMovebetacutH;)
              return Definitions::algo.do_failsoftalphabeta ? alreadyComputedStandPat : beta;
          }
    }

    // =====================
    // Razoring
    // <= alpha - margin
    // =====================
    if (OPTIM_BOOL_TRUE(Definitions::algo.do_razoring) &&
        !fromPV &&
        depthleft <= Definitions::selectivity.razoringMaxDepth &&
        iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
        !Move::IsNearPromotion(p,game.Info(p.CurrentPly()).lastMove.to)) { // do not perform razoring just before promotion

        alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                   : Analyze(p, Definitions::algo.do_lazyEvaluation,fromPV);
        standPatComputed = true;

        ScoreType margin = Definitions::selectivity.razoringMargin
                         + Definitions::selectivity.razoringDepthCoeff*depthleft;

        if (alreadyComputedStandPat + margin <= alpha) {
            // verification qsearch...
            OPTIM_STATS(++Stats::razoringbetacutQtry;)
            ScoreType rAlpha = alpha - margin;
            const DepthType qdepth = Definitions::algo.do_quiesce?(int)Definitions::selectivity.quies_max:0;
            ScoreType val = NegaQuiesce(rAlpha, rAlpha+1, qdepth, p, color, iterativeDeepeningDepth, ply+1, fromPV, TimeMan::eTC_on);
            if (std::abs(val) != Definitions::scores.stopFlagScore && val <= rAlpha) {
                OPTIM_STATS(++Stats::razoringbetacutQ;)
                return Definitions::algo.do_failsoftalphabeta ? std::max(val, alreadyComputedStandPat) : alpha;
            }
        }
    }

    // =====================
    // Null move pruning
    // =====================
    if (OPTIM_BOOL_TRUE(Definitions::algo.do_nullMovePruning) &&
        !fromPV &&
        // alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        depthleft >= Definitions::selectivity.nullMovePruningMinDepth &&
        !p.IsVeryEndGame() &&
        !Square::IsValid(p.EnPassant()) //protecting from hash issue in swithPlayer !!! ///@todo debug !!!!!
        ) {

      alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                 : Analyze(p, Definitions::algo.do_lazyEvaluation,fromPV);
      standPatComputed = true;

      // performing null-move pruning only if stand-pat suggess position is bad already
      if ( alreadyComputedStandPat - Definitions::selectivity.nullMoveMargin >= beta ) {

          OPTIM_STATS(++Stats::nullmovecuttry;)

#ifdef WITH_MEMORY_POOL_BUILDER
        ScopeClone<Position, _default_block_size> psc(p);
        Position & p2 = psc.Get();
#else
        Position p2(p);
#endif
        p2.SwitchPlayerNullMove(); // same player get to play again but currentPly++
        game.SetHash(1/*totally false hash...*/,p.CurrentPly()+1,false); // add a fake game hash ...
        //LOG(logINFO) << "Null move lookup";

        //DepthType R = 2 + (depthleft>=3) + (depthleft>=6) + (depthleft>=9) + (depthleft>=13) // adaptative null-move pruning
        //                + (Definitions::algo.do_verifiedNullMove?(int)std::min((alreadyComputedStandPat - beta) / Piece::Value(Piece::t_Wpawn), 3):0); // also based on eval if verified !
        //DepthType R = depthleft/4 + 3; // from xiphos
        //DepthType R = 3 + std::min(3,(alreadyComputedStandPat-beta)/128); // from Monolith
        DepthType R = 2 + depthleft/3; // from chess22k

        // dynamic reduction +1 if retBase > beta + Pawn ///@todo give it a chance again ?
        //if ( alreadyComputedStandPat > beta + Piece::ValueAbs(Piece::t_Wpawn)) R++;

        bool tryNull = true;

        // omit null move search if normal search to the same depth wouldn't exceed beta
        // (sometimes we can check it for free via hash table)
        Transposition tttNull;
        if ( (Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
             && Transposition::GetTT(PHASH(p),depthleft-R,alpha,beta,tttNull,p.CurrentPly())) { // we use p (not p2) here on purpose
            if (tttNull.score < beta) tryNull = false;
        }

        if ( tryNull ){

            const DepthType qdepth = Definitions::algo.do_quiesce?(int)Definitions::selectivity.quies_max:0;
            ScoreType nullScore = (depthleft-R-1>0) ? -Negamax(-beta, -beta+1, depthleft - R - 1,
                                                               p2, color, iterativeDeepeningDepth/*0*/, ply+1, // not skipping a ply
                                                               false, NULL, TimeMan::eTC_on, Searcher::eRedA_off, Searcher::eExtA_on)
                                                    : -NegaQuiesce(-beta, -beta+1, qdepth,
                                                               p2, color, iterativeDeepeningDepth, ply+1, // not skipping a ply
                                                               false, TimeMan::eTC_on);

            if (std::abs(nullScore) != Definitions::scores.stopFlagScore) {

                // do not return mate score
                if (nullScore > Definitions::scores.checkMateScore - MAX_GAME_PLY) {
                    nullScore = beta;
                }

                if (nullScore /*- Definitions::nullMoveMargin*/ >= beta) { // too good move

                  // verified nullmove at high R
                    if (Definitions::algo.do_verifiedNullMove && R >= Definitions::selectivity.verifiedNullMoveMinDepth) {
                        OPTIM_STATS(++Stats::nullmoveverification;)
                        const DepthType qdepth = Definitions::algo.do_quiesce ? (int)Definitions::selectivity.quies_max : 0;
                        nullScore = (depthleft - R - 1 > 0) ? Negamax(beta - 1, beta, depthleft - R - 1, p, color, iterativeDeepeningDepth/*0*/, ply + 1,
                            false, NULL, TimeMan::eTC_on, Searcher::eRedA_off, Searcher::eExtA_on)
                            : NegaQuiesce(beta - 1, beta, qdepth, p, color, iterativeDeepeningDepth, ply + 1,
                                false, TimeMan::eTC_on);
                        if (std::abs(nullScore) != Definitions::scores.stopFlagScore
                            && nullScore /*- Definitions::nullMoveMargin*/ >= beta) { // too good move
                            //LOG(logINFO) << "Null move pruning, beta was " << beta << " null move gets " << nullMove.second;
                            OPTIM_STATS(++Stats::nullmovecutbeta;)
                            OPTIM_STATS(++Stats::nullmovecutafterverification;)
                            return Definitions::algo.do_failsoftalphabeta ? nullScore : beta;
                        }
                    }
                    else {
                        //LOG(logINFO) << "Null move pruning, beta was " << beta << " null move gets " << nullMove.second;
                        OPTIM_STATS(++Stats::nullmovecutbeta;)
                        return Definitions::algo.do_failsoftalphabeta ? nullScore : beta;
                    }
                }
                else {
                    ///@todo something when null move fail low ?
                }
            }
        }
        else{
            OPTIM_STATS(++Stats::nullmovecutskipped;)
        }
      }
    }

    // =====================
    // Null move reduction
    // =====================
    ///@todo bigger depth, do not trigger cut-off but a reduction

    // =====================
    // IID shall be here ! if we want to use the iid move as the ttmove in order to skip move generation in case of cut-off !
    // =====================
    ///@todo iid here and affect ttmove

    // =====================
    // ProbCut
    // =====================
    if ( OPTIM_BOOL_TRUE(Definitions::algo.do_probCut) &&
         depthleft >= Definitions::selectivity.probCutMinDepth &&
         !fromPV &&
         !p.IsEndGame()){

      OPTIM_STATS(++Stats::probCutTry;)

      int probCutCount = 0;
      const ScoreType margin = Definitions::selectivity.probCutMargin
                             + Definitions::selectivity.probCutDepthCoeff * depthleft;
      ScoreType betaPC = beta + margin;

      // move generation
      MoveGenerator(Definitions::algo.do_trustedGenerator).Generator(p,moves);
      moveGenerated = true;

      // move ordering
      if (!moves.empty()) {
          bestMoveFound = Move::Ordering(moves, p, *this, fromPV, depthleft, ply, Definitions::ttConfig.do_transpositionTableSortAlphaBeta ? (&ttt) : NULL, pv);
      }
      else { // no moves found...
             // =====================
             // Early _exit for checkmate and stalemate
             // !!!! : can/will be missed because of pseudo legal move generator !
             // =====================
          if (p.IsInCheck()) { // mate
                               //LOG(logINFO) << "Check mate " << p.GetFEN();
              Line::ClearPV(pv);
              return -(Definitions::scores.checkMateScore - p.CurrentMoveCount());
          }
          else { // stalemate
                 //LOG(logINFO) << "Stale mate " << p.GetFEN();
              Line::ClearPV(pv);
              return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
          }
      }

      for (auto it = moves.begin() ; it != moves.end() && probCutCount < Definitions::selectivity.probCutMaxMoves; ++it){

        //HANDLE_TC_ALPHA_BETA

        // skip tt move if silent and skip bad captures
        if ((*it).ZHash() == ttt.hash && ( IsSilent(*it,p)
                                      || ((*it).SortScore() < 0) ) ) {
          continue;
        }

        // =====================
        // Validate move
        // =====================
        if ( ttt.hash != (*it).ZHash() ){
            if ( ! UtilMove::ValidateMove(*it,p) ) continue;
        }
        ++probCutCount;

        // =====================
        // Apply the current move
        // =====================
        // copy / make
        Position p2 = ApplyMoveCopyPosition(p, *it); // game hash is updated

        ///@todo make a preliminary qsearch to verifiy the move ?

        // Perform a preliminary qsearch to verify that the move holds
        const DepthType qdepth = Definitions::algo.do_quiesce?(int)Definitions::selectivity.quies_max:0;
        ScoreType scorePC = -NegaQuiesce(-betaPC,-betaPC + 1,qdepth,p2,color,iterativeDeepeningDepth,ply+1,false, TimeMan::eTC_on);

        if (std::abs(scorePC) != Definitions::scores.stopFlagScore
            && scorePC >= betaPC){
            scorePC = -Negamax(-betaPC, -betaPC + 1,
                depthleft - Definitions::selectivity.probCutMinDepth, p2, color,
                iterativeDeepeningDepth, ply + 1,
                false, // not from PV
                NULL,
                TimeMan::eTC_on,
                Searcher::eRedA_on,
                Searcher::eExtA_off);
        }

        if (std::abs(scorePC) != Definitions::scores.stopFlagScore && scorePC >= betaPC){
          OPTIM_STATS(++Stats::probCutSuccess;)
          return scorePC;
        }
      }
    }

    // =====================
    // Move count pruning (see moveCountPruningAllowed)
    // =====================
    if (OPTIM_BOOL_TRUE(Definitions::algo.do_moveCountPruning) &&
        !fromPV &&
        //alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        depthleft <= Definitions::selectivity.moveCountPruningMaxDepth &&
        iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
        !p.IsEndGame()
        ){

        moveCountPruningAllowed = true;
    }

    // =====================
    // Futility pruning (see futilityPruningAllowed)
    // < alpha - margin
    // =====================
    if (OPTIM_BOOL_TRUE(Definitions::algo.do_futilityPruning) &&
        !fromPV &&
        //alpha > Definitions::previousScore[p.Turn()] - Definitions::lmrtol &&
        depthleft <= Definitions::selectivity.futilityMaxDepth &&
        iterativeDeepeningDepth > Definitions::selectivity.iterativeDeepeningSelectivityMinDepth &&
        !Move::IsNearPromotion(p, game.Info(p.CurrentPly()).lastMove.to)) { // to not perform futility pruning just before promotion

        alreadyComputedStandPat = standPatComputed ? alreadyComputedStandPat
                                                   : Analyze(p, Definitions::algo.do_lazyEvaluation,fromPV);
        standPatComputed = true;

        ScoreType margin = Definitions::selectivity.futilityMargin
                         + Definitions::selectivity.futilityDepthCoeff*depthleft;

        futilityPruningAllowed = (alreadyComputedStandPat + margin <= alpha);

    }

  } // end of early pruning ...

  // =====================
  // Move generation and ordering (if not done in ProbCut yet)
  // =====================
  //FastContainer<Move> moves;
    if ( ! moveGenerated ){ // moves may have been generated by probCut already ...
      //if (!ttmove.IsValid()) { // if no tt move (this should not happen thanks to iid that replace ttmove with a swallow search)
          MoveGenerator(Definitions::algo.do_trustedGenerator).Generator(p, moves);
          moveGenerated = true;
          if (!moves.empty()) {
              bestMoveFound = Move::Ordering(moves, p, *this, fromPV, depthleft, ply, Definitions::ttConfig.do_transpositionTableSortAlphaBeta ? (&ttt) : NULL, pv);
          }
          else { // no moves found...
                 // =====================
                 // Early _exit for checkmate and stalemate
                 // !!!! : can/will be missed because of pseudo legal move generator !
                 // =====================
              if (p.IsInCheck()) { // mate
                                   //LOG(logINFO) << "Check mate " << p.GetFEN();
                  Line::ClearPV(pv);
                  return -(Definitions::scores.checkMateScore - p.CurrentMoveCount());
              }
              else { // stalemate
                     //LOG(logINFO) << "Stale mate " << p.GetFEN();
                  Line::ClearPV(pv);
                  return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
              }
          }
      /*}
      else { // if tt move is valid we will try it first (it is already validated)
          moves.push_back(ttmove);
      }
      */
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
  // let reductions happen again (a.k.a recursive nullmove... for example )
  // =====================
  if ( OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_recursiveReduction) && (allowPruningAndReduction == Searcher::eRedA_off) ) {
      OPTIM_STATS(++Stats::reductionAndPruningReset;)
      allowPruningAndReduction = Searcher::eRedA_on;
  }

  // =====================
  // let extensions happen again (a.k.a recursive singular extension... for example )
  // =====================
  if (OPTIM_BOOL_FALSE(Definitions::extensionReductionConfig.do_recursiveExtension) && (allowExtension == Searcher::eExtA_off) ) {
      OPTIM_STATS(++Stats::extensionReset;)
      allowExtension = Searcher::eExtA_on;
  }

  // =====================
  // Iterate through moves
  // =====================
  unsigned int moveCount = 0;
  //bool fullOrdered = false;

  //bool ttmoveTryDone = moveGenerated; // if moves are already generated no need to try tt move (this is true if no ProbCut is done)

  for (auto it = moves.begin() ; it != moves.end() ; ++it, ++moveCount){

    //HANDLE_TC_ALPHA_BETA

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

    // =====================
    // Late Move generation and ordering (if only TT move was tried yet)
    // =====================
    /*
    if (!ttmoveTryDone && !moveGenerated ) { // only if ttmove was tried yet
        ttmoveTryDone = true;
        moves.clear(); // remove the ttmove from the list
        MoveGenerator(Definitions::algo.do_trustedGenerator).Generator(p, moves);
        moveGenerated = true;
        if (!moves.empty()) {
            bestMoveFound = Move::Ordering(moves, p, *this, fromPV, depthleft, ply, Definitions::ttConfig.do_transpositionTableSortAlphaBeta ? (&ttt) : NULL, pv);
        }
        else { // no moves found...
               // =====================
               // Early _exit for checkmate and stalemate
               // !!!! : can/will be missed because of pseudo legal move generator !
               // =====================
            if (p.IsInCheck()) { // mate
                                 //LOG(logINFO) << "Check mate " << p.GetFEN();
                Line::ClearPV(pv);
                return -(Definitions::scores.checkMateScore - p.CurrentMoveCount());
            }
            else { // stalemate
                   //LOG(logINFO) << "Stale mate " << p.GetFEN();
                Line::ClearPV(pv);
                return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
            }
        }
        // reset iterator
        it = moves.begin();
        continue; // skip the first one which is the ttmove ...
    }
    */

    Line::ClearPV(pv_loc);

    // =====================
    // Validate move
    // =====================
    if ( ttt.hash != (*it).ZHash() ){
        if ( ! UtilMove::ValidateMove(*it,p) ) continue;
    }

    // this is because, even if no bestmove is found > alpha,
    // we insert something in TT with score alpha ...
    if ( isFirstMove ) bestMove = &*it;

    bool isSilentMove    = IsSilent(*it,p);
    bool isSilentMove2   = IsVerySilent(*it,p);
    bool isSilentCapture = IsSilentCapture(*it,p);

    // =====================
    // Prune moves with bad SEE
    // =====================
    if ( OPTIM_BOOL_FALSE(Definitions::algo.do_seePruningAlphaBeta)
         && futilityPruningAllowed
         && depthleft <= Definitions::selectivity.seePruningAlphaBetaMaxDepth
         && !fromPV
         && !isFirstMove
         && validMoveFound // cannot prune until a valid move is found in order to return something ...
         && isSilentCapture
         //&& (*it).SortScore() < -Definitions::scores.captureSortScore + Definitions::selectivity.seePruningAlphaBetaMaxDepth - depthleft)
         && !SEE(p,*it,-29*depthleft*depthleft) )
    {
            OPTIM_STATS(++Stats::seecutAlphaBeta;)
            continue; // just skip it !
    }

    // =====================
    // prune some moves if possible (futility)
    // =====================
    if (futilityPruningAllowed
         && !isFirstMove
         && validMoveFound // cannot prune until a valid move is found in order to return something ...
         && isSilentMove
         && (*it).CanBeReduced()
         ){
        OPTIM_STATS(++Stats::futilityMovePruned;)
        continue; // just skip it !
    }

    // =====================
    // prune some moves if possible (move count pruning)
    // =====================
    static const DepthType lmp_min[11] = { 0, 5, 6, 9, 15, 23, 32, 42, 54, 68, 83 }; // xiphos
    if (moveCountPruningAllowed
        && !isFirstMove
        && validMoveFound // cannot prune until a valid move is found in order to return something ...
        && isSilentMove
        && (*it).CanBeReduced()
        && (int(lmr_count) >= lmp_min[depthleft] )
        ){
        OPTIM_STATS(++Stats::moveCountPruned;)
        continue; // just skip it !
    }

    // ONLY AFTER pruning !!!
    validMoveFound = true;

    // =====================
    // Apply the current move
    // =====================
    // copy / make
    Position p2 = ApplyMoveCopyPosition(p, *it); // game hash is updated

    // =====================
    // look for applicable extensions
    // =====================
    DepthType nextDepth = depthleft-1;
    //DepthType nextDepthWithoutExtension = nextDepth;
    if (allowExtension==Searcher::eExtA_on){
        ManageExtensionNegamax(nextDepth, ply, p, p2, *it, moves, isFirstMove, bestMoveFound, ttt, *this);
    }
    // ensure depth is not too deep ...
    nextDepth = std::min(nextDepth,DepthType(MAX_SEARCH_DEPTH-1)); // limit highest depth
    DepthType nextDepthWithoutReduction = nextDepth;

    // =====================
    // reduction : LMR
    // =====================
    if ( OPTIM_BOOL_TRUE(Definitions::algo.do_lmrAlphabeta)
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
        if ( p.IsEndGame() && R > 2 ) --R;
        // be more carefull for PV parent
        if ( fromPV ) --R;
        nextDepth -= R;
        OPTIM_STATS(++Stats::lmralphabeta;)
    }

    // =====================
    // apply other reductions
    // =====================
    DepthType reduction = 0;
    if ( ! isFirstMove ){
       if ( OPTIM_BOOL_TRUE(Definitions::extensionReductionConfig.do_ttmoveCaptureReduction)
         && ttmove.IsValid() && ttmove.IsCapture()){
           reduction += 1;
       }
       /*
       // from Demolito
       else if ( !(*it).IsCapture()){ ///@todo json
           reduction += 1;
       }
       */
       nextDepth -= reduction; // computed above ...
    }

    // limit lowest depth
    nextDepth = std::max(nextDepth,DepthType(0));

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
    val = - Negamax( -beta_loc, -alpha_loc, nextDepth, p2, color, iterativeDeepeningDepth, ply+1,
                     isPV && fromPV,pv_loc, timeControl, allowPruningAndReduction, allowExtension);

    // =====================
    // a reduced search cannot raise alpha !
    // =====================
    if ( nextDepthWithoutReduction != nextDepth && val > alpha ){
        if ( reduction == 0 ){
           OPTIM_STATS(++Stats::lmralphabetafail;)
        }
        else{
           OPTIM_STATS(++Stats::failedReduction;)
        }
        nextDepth = nextDepthWithoutReduction;
        goto research;
    }

    if(doPVS){
      if ( /*fromPV from Demolito &&*/ (val > alpha && val < beta) ){ // PVS fail (fail-soft framework)
        OPTIM_STATS(++Stats::pvsAlphaBetafails;)
        Line::ClearPV(pv_loc); // reset pv_loc (lmr, pvs, ...)
        val = - Negamax( -beta, -alpha, nextDepth , p2, color,iterativeDeepeningDepth, ply+1,
                         true,pv_loc,timeControl, allowPruningAndReduction, allowExtension); //can be a new pv
      }
      else{
          OPTIM_STATS(++Stats::pvsAlphaBetasuccess;)
      }
    }

    ++lmr_count;

    // =====================
    // update move score (this is totally useless, unless generated moves is inside a static cache indexed by position hash ...)
    // =====================
    //(*it).SetSortScore(val);

    HANDLE_TC_ALPHA_BETA

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

          UpdateMoveStat(p,*it,moves,*this,depthleft,allowPruningAndReduction==Searcher::eRedA_on);

          // =====================
          // ttable insert
          // =====================
          if (OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
                  /*&& !stopFlag*/) { // see HANDLE_TC_ALPHA_BETA just above
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

          if ( OPTIM_BOOL_TRUE(! Definitions::algo.do_alphabetaminimax) ){
            OPTIM_STATS(++Stats::betacut;)
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
      return - (Definitions::scores.checkMateScore - p.CurrentMoveCount());
    }
    else{ // stalemate
      return p.IsEndGame() ? 0 : -Definitions::scores.contempt;
    }
  }

  // =====================
  // ttable insert
  // =====================
  if (OPTIM_BOOL_TRUE(Definitions::ttConfig.do_transpositionTableAlphaBeta || Definitions::ttConfig.do_transpositionTableSortAlphaBeta)
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
