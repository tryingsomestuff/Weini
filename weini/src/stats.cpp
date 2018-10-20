#include "stats.h"
#include "logger.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>

namespace TheadingTools {
    std::map<std::thread::id, size_t> mapId;

    std::thread::id ThreadId() {
        return std::this_thread::get_id();
    }

    size_t IdFromThreadId() {
        return mapId[ThreadId()];
    }

    void Register(size_t id) {
        mapId[ThreadId()] = id;
    }
}

ScoreType Stats::previousScore[2] = { 0 ,0 }; ///@todo init from CLI ???

Stats::AtomicCounter Stats::betacut                 ;
Stats::AtomicCounter Stats::betacutroot             ;
Stats::AtomicCounter Stats::qbetacut                ;
Stats::AtomicCounter Stats::qearlybetacut           ;
Stats::AtomicCounter Stats::deltaalphacut           ;
Stats::AtomicCounter Stats::seecut                  ;
Stats::AtomicCounter Stats::seecutAlphaBeta         ;
Stats::AtomicCounter Stats::nullmoveverification    ;
Stats::AtomicCounter Stats::nullmovecutafterverification;
Stats::AtomicCounter Stats::nullmovecutbeta         ;
Stats::AtomicCounter Stats::nullmovecuttry          ;
Stats::AtomicCounter Stats::nullmovecutskipped      ;
Stats::AtomicCounter Stats::reductionAndPruningReset;
Stats::AtomicCounter Stats::extensionReset;
Stats::AtomicCounter Stats::razoringbetacut         ;
Stats::AtomicCounter Stats::razoringbetacutQtry     ;
Stats::AtomicCounter Stats::razoringbetacutQ        ;
Stats::AtomicCounter Stats::razoringbetacutH        ;
Stats::AtomicCounter Stats::staticNullMovebetacut   ;
Stats::AtomicCounter Stats::staticNullMovebetacutQtry;
Stats::AtomicCounter Stats::staticNullMovebetacutQ  ;
Stats::AtomicCounter Stats::staticNullMovebetacutH  ;
Stats::AtomicCounter Stats::failedReduction         ;
Stats::AtomicCounter Stats::futilitycut             ;
Stats::AtomicCounter Stats::qfutilitycut            ;
Stats::AtomicCounter Stats::futilityMovePruned      ;
Stats::AtomicCounter Stats::moveCountPruned         ;
Stats::AtomicCounter Stats::lmralphabeta            ;
Stats::AtomicCounter Stats::lmralphabetafail        ;
Stats::AtomicCounter Stats::windowfails             ;
Stats::AtomicCounter Stats::pvsRootfails            ;
Stats::AtomicCounter Stats::pvsAlphaBetafails       ;
Stats::AtomicCounter Stats::windowsuccess           ;
Stats::AtomicCounter Stats::pvsRootsuccess          ;
Stats::AtomicCounter Stats::pvsAlphaBetasuccess     ;
Stats::AtomicCounter Stats::lmrsearch               ;
Stats::AtomicCounter Stats::lmrsearchfail           ;
Stats::AtomicCounter Stats::ttARHitExact            ;
Stats::AtomicCounter Stats::ttARHitAlpha            ;
Stats::AtomicCounter Stats::ttARHitBeta             ;
Stats::AtomicCounter Stats::ttARHitAlphaOver        ;
Stats::AtomicCounter Stats::ttARHitBetaOver         ;
Stats::AtomicCounter Stats::ttARHitUsed             ;
Stats::AtomicCounter Stats::ttARCollision           ;
Stats::AtomicCounter Stats::ttAllHit                ;
Stats::AtomicCounter Stats::ttHitExact              ;
Stats::AtomicCounter Stats::ttHitAlpha              ;
Stats::AtomicCounter Stats::ttHitBeta               ;
Stats::AtomicCounter Stats::ttHitAlphaOver          ;
Stats::AtomicCounter Stats::ttHitBetaOver           ;
Stats::AtomicCounter Stats::ttHitUsed               ;
Stats::AtomicCounter Stats::ttCollision             ;
Stats::AtomicCounter Stats::ttARQHitExact           ;
Stats::AtomicCounter Stats::ttARQHitAlpha           ;
Stats::AtomicCounter Stats::ttARQHitBeta            ;
Stats::AtomicCounter Stats::ttARQHitAlphaOver       ;
Stats::AtomicCounter Stats::ttARQHitBetaOver        ;
Stats::AtomicCounter Stats::ttARQHitUsed            ;
Stats::AtomicCounter Stats::ttARQCollision          ;
Stats::AtomicCounter Stats::ttQAllHit               ;
Stats::AtomicCounter Stats::ttQHitExact             ;
Stats::AtomicCounter Stats::ttQHitAlpha             ;
Stats::AtomicCounter Stats::ttQHitBeta              ;
Stats::AtomicCounter Stats::ttQHitAlphaOver         ;
Stats::AtomicCounter Stats::ttQHitBetaOver          ;
Stats::AtomicCounter Stats::ttQHitUsed              ;
Stats::AtomicCounter Stats::ttQCollision            ;
Stats::AtomicCounter Stats::ttClear                 ;
Stats::AtomicCounter Stats::ttQClear                ;
Stats::AtomicCounter Stats::ttEClear                ;
Stats::AtomicCounter Stats::ttELClear               ;
Stats::AtomicCounter Stats::ttEPClear               ;
Stats::AtomicCounter Stats::leafNodes               ;
Stats::AtomicCounter Stats::visitednodes            ;
Stats::AtomicCounter Stats::visitedqnodes           ;
Stats::AtomicCounter Stats::visitedrealqnodes       ;
Stats::AtomicCounter Stats::analysednodes           ;
Stats::AtomicCounter Stats::analysedlazynodes       ;
Stats::AtomicCounter Stats::ttEvalLazyHits          ;
Stats::AtomicCounter Stats::ttEvalHits              ;
Stats::AtomicCounter Stats::ttEvalPawnHits          ;
Stats::AtomicCounter Stats::ttEvalLazyCollision     ;
Stats::AtomicCounter Stats::ttEvalCollision         ;
Stats::AtomicCounter Stats::ttEvalPawnCollision     ;
Stats::AtomicCounter Stats::terminalnodes           ;
Stats::AtomicCounter Stats::openingBookHits         ;
Stats::AtomicCounter Stats::ttsort                  ;
Stats::AtomicCounter Stats::ttsorttry               ;
Stats::AtomicCounter Stats::ttQsort                 ;
Stats::AtomicCounter Stats::ttQsorttry              ;
Stats::AtomicCounter Stats::pvsort                  ;
Stats::AtomicCounter Stats::pvsorttry               ;
Stats::AtomicCounter Stats::pvrootsort              ;
Stats::AtomicCounter Stats::pvrootsorttry           ;
Stats::AtomicCounter Stats::iidsort                 ;
Stats::AtomicCounter Stats::probCutTry              ;
Stats::AtomicCounter Stats::probCutSuccess          ;
Stats::AtomicCounter Stats::positionCopy            ;
Stats::AtomicCounter Stats::currentEveryNodesCount  ;
Stats::AtomicCounter Stats::currentPseudoNodesCount ;
Stats::AtomicCounter Stats::currentNodesCount       ;
Stats::AtomicCounter Stats::currentqNodesCount      ;
Stats::AtomicCounter Stats::currentMaxDepth         ;
Stats::AtomicCounter Stats::currentLeafNodesCount   ;

Stats::AtomicCounter Stats::countCheckExtensionRoot              ;
Stats::AtomicCounter Stats::countCheckExtensionSearch            ;
Stats::AtomicCounter Stats::countCheckExtensionAlphaBeta         ;
Stats::AtomicCounter Stats::countEndGameExtensionSearch          ;
Stats::AtomicCounter Stats::countVeryEndGameExtensionSearch      ;
Stats::AtomicCounter Stats::countPVExtensionSearch               ;
Stats::AtomicCounter Stats::countSingleReplyExtensionSearch      ;
Stats::AtomicCounter Stats::countSingleReplyExtensionAlphaBeta   ;
Stats::AtomicCounter Stats::countSingularExtensionAlphaBeta      ;
Stats::AtomicCounter Stats::countNearPromotionExtensionSearch    ;
Stats::AtomicCounter Stats::countNearPromotionExtensionAlphaBeta ;
Stats::AtomicCounter Stats::countReCaptureExtensionSearch        ;
Stats::AtomicCounter Stats::countReCaptureExtensionAlphaBeta     ;

Stats::AtomicCounter Stats::nodesCountByDepth[MAX_SEARCH_DEPTH] ;
Stats::AtomicCounter Stats::qnodesCountByDepth[MAX_SEARCH_DEPTH];

Stats::AtomicCounter Stats::seldepth[MAX_SEARCH_DEPTH] ;

Stats::AtomicCounter Stats::tcEBFAbort    ;
Stats::AtomicCounter Stats::tcForcedAbort ;
Stats::AtomicCounter Stats::tcIdealAbort  ;

void Stats::Init(){

    LOG(logINFO) << "Init statistics";

    Stats::betacut                 = 0;
    Stats::betacutroot             = 0;
    Stats::qbetacut                = 0;
    Stats::qearlybetacut           = 0;
    Stats::deltaalphacut           = 0;
    Stats::seecut                  = 0;
    Stats::seecutAlphaBeta         = 0;
    Stats::nullmovecutbeta         = 0;
    Stats::nullmovecuttry          = 0;
    Stats::nullmovecutskipped      = 0;
    Stats::reductionAndPruningReset= 0;
    Stats::extensionReset          = 0;
    Stats::razoringbetacut         = 0;
    Stats::razoringbetacutQtry     = 0;
    Stats::razoringbetacutQ        = 0;
    Stats::razoringbetacutH        = 0;
    Stats::staticNullMovebetacut   = 0;
    Stats::staticNullMovebetacutQtry = 0;
    Stats::staticNullMovebetacutQ  = 0;
    Stats::staticNullMovebetacutH  = 0;
    Stats::failedReduction         = 0;
    Stats::futilitycut             = 0;
    Stats::qfutilitycut            = 0;
    Stats::futilityMovePruned      = 0;
    Stats::moveCountPruned         = 0;
    Stats::lmralphabeta            = 0;
    Stats::lmralphabetafail        = 0;
    Stats::windowfails             = 0;
    Stats::pvsRootfails            = 0;
    Stats::pvsAlphaBetafails       = 0;
    Stats::windowsuccess           = 0;
    Stats::pvsRootsuccess          = 0;
    Stats::pvsAlphaBetasuccess     = 0;
    Stats::lmrsearch               = 0;
    Stats::lmrsearchfail           = 0;
    Stats::ttARHitExact            = 0;
    Stats::ttARHitAlpha            = 0;
    Stats::ttARHitBeta             = 0;
    Stats::ttARHitAlphaOver        = 0;
    Stats::ttARHitBetaOver         = 0;
    Stats::ttARHitUsed             = 0;
    Stats::ttARCollision           = 0;
    Stats::ttAllHit                = 0;
    Stats::ttHitExact              = 0;
    Stats::ttHitAlpha              = 0;
    Stats::ttHitBeta               = 0;
    Stats::ttHitAlphaOver          = 0;
    Stats::ttHitBetaOver           = 0;
    Stats::ttHitUsed               = 0;
    Stats::ttCollision             = 0;
    Stats::ttARQHitExact           = 0;
    Stats::ttARQHitAlpha           = 0;
    Stats::ttARQHitBeta            = 0;
    Stats::ttARQHitAlphaOver       = 0;
    Stats::ttARQHitBetaOver        = 0;
    Stats::ttARQHitUsed            = 0;
    Stats::ttARQCollision          = 0;
    Stats::ttQAllHit               = 0;
    Stats::ttQHitExact             = 0;
    Stats::ttQHitAlpha             = 0;
    Stats::ttQHitBeta              = 0;
    Stats::ttQHitAlphaOver         = 0;
    Stats::ttQHitBetaOver          = 0;
    Stats::ttQHitUsed              = 0;
    Stats::ttQCollision            = 0;
    Stats::ttClear                 = 0;
    Stats::ttQClear                = 0;
    Stats::ttEClear                = 0;
    Stats::ttELClear               = 0;
    Stats::ttEPClear               = 0;
    Stats::leafNodes               = 0;
    Stats::visitednodes            = 0;
    Stats::visitedqnodes           = 0;
    Stats::visitedrealqnodes       = 0;
    Stats::analysednodes           = 0;
    Stats::analysedlazynodes       = 0;
    Stats::ttEvalLazyHits          = 0;
    Stats::ttEvalHits              = 0;
    Stats::ttEvalPawnHits          = 0;
    Stats::ttEvalLazyCollision     = 0;
    Stats::ttEvalCollision         = 0;
    Stats::ttEvalPawnCollision     = 0;
    Stats::terminalnodes           = 0;
    Stats::openingBookHits         = 0;
    Stats::ttsort                  = 0;
    Stats::ttsorttry               = 0;
    Stats::ttQsort                 = 0;
    Stats::ttQsorttry              = 0;
    Stats::pvsort                  = 0;
    Stats::pvsorttry               = 0;
    Stats::pvrootsort              = 0;
    Stats::pvrootsorttry           = 0;
    Stats::iidsort                 = 0;
    Stats::probCutTry              = 0;
    Stats::probCutSuccess          = 0;
    Stats::positionCopy            = 0;
    Stats::currentEveryNodesCount  = 0;
    Stats::currentPseudoNodesCount = 0;
    Stats::currentNodesCount       = 0;
    Stats::currentqNodesCount      = 0;
    Stats::currentMaxDepth         = 0;
    Stats::currentLeafNodesCount   = 0;

    for (DepthType k = 0 ; k < MAX_SEARCH_DEPTH ; ++ k){
      Stats::nodesCountByDepth[k]  = 0;
      Stats::qnodesCountByDepth[k] = 0;
      Stats::seldepth[k]           = 0;
    }

    Stats::tcEBFAbort              = 0;
    Stats::tcForcedAbort           = 0;
    Stats::tcIdealAbort            = 0;

}

void Stats::CutCount(){

  LOG(logINFO) << "betacut                              " << Stats::betacut               ;
  LOG(logINFO) << "betacutroot                          " << Stats::betacutroot           ;
  LOG(logINFO) << "qbetacut                             " << Stats::qbetacut              ;
  LOG(logINFO) << "qearlybetacut                        " << Stats::qearlybetacut         ;
  LOG(logINFO) << "deltaalphacut                        " << Stats::deltaalphacut         ;
  LOG(logINFO) << "seecut                               " << Stats::seecut                ;
  LOG(logINFO) << "seecutAlphaBeta                      " << Stats::seecutAlphaBeta       ;
  LOG(logINFO) << "nullmoveverification                 " << Stats::nullmoveverification  ;
  LOG(logINFO) << "nullmovecutafterverification         " << Stats::nullmovecutafterverification;
  LOG(logINFO) << "nullmovecutbeta                      " << Stats::nullmovecutbeta       ;
  LOG(logINFO) << "nullmovecuttry                       " << Stats::nullmovecuttry        ;
  LOG(logINFO) << "nullmovecutskipped                   " << Stats::nullmovecutskipped    ;
  LOG(logINFO) << "reductionAndPruningReset             " << Stats::reductionAndPruningReset;
  LOG(logINFO) << "extensionReset                       " << Stats::extensionReset        ;
  LOG(logINFO) << "razoringbetacut                      " << Stats::razoringbetacut       ;
  LOG(logINFO) << "razoringbetacutQtry                  " << Stats::razoringbetacutQtry   ;
  LOG(logINFO) << "razoringbetacutQ                     " << Stats::razoringbetacutQ      ;
  LOG(logINFO) << "razoringbetacutH                     " << Stats::razoringbetacutH      ;
  LOG(logINFO) << "staticNullMovebetacut                " << Stats::staticNullMovebetacut ;
  LOG(logINFO) << "staticNullMovebetacutQtry            " << Stats::staticNullMovebetacutQtry;
  LOG(logINFO) << "staticNullMovebetacutQ               " << Stats::staticNullMovebetacutQ;
  LOG(logINFO) << "staticNullMovebetacutH               " << Stats::staticNullMovebetacutH;
  LOG(logINFO) << "failedReduction                      " << Stats::failedReduction       ;
  LOG(logINFO) << "futilitycut                          " << Stats::futilitycut           ;
  LOG(logINFO) << "qfutilitycut                         " << Stats::qfutilitycut          ;
  LOG(logINFO) << "futilityMovePruned                   " << Stats::futilityMovePruned    ;
  LOG(logINFO) << "moveCountPruned                      " << Stats::moveCountPruned       ;
  LOG(logINFO) << "lmralphabeta                         " << Stats::lmralphabeta          ;
  LOG(logINFO) << "lmralphabetafail                     " << Stats::lmralphabetafail      ;
  LOG(logINFO) << "lmrsearch                            " << Stats::lmrsearch             ;
  LOG(logINFO) << "lmrsearchfail                        " << Stats::lmrsearchfail         ;
  LOG(logINFO) << "ttARHitExact                         " << Stats::ttARHitExact          ;
  LOG(logINFO) << "ttARHitAlpha                         " << Stats::ttARHitAlpha          ;
  LOG(logINFO) << "ttARHitBeta                          " << Stats::ttARHitBeta           ;
  LOG(logINFO) << "ttARHitAlphaOver                     " << Stats::ttARHitAlphaOver      ;
  LOG(logINFO) << "ttARHitBetaOver                      " << Stats::ttARHitBetaOver       ;
  LOG(logINFO) << "ttARHitUsed                          " << Stats::ttARHitUsed           ;
  LOG(logINFO) << "ttARCollision                        " << Stats::ttARCollision         ;
  LOG(logINFO) << "ttAllHit                             " << Stats::ttAllHit              ;
  LOG(logINFO) << "ttHitExact                           " << Stats::ttHitExact            ;
  LOG(logINFO) << "ttHitAlpha                           " << Stats::ttHitAlpha            ;
  LOG(logINFO) << "ttHitBeta                            " << Stats::ttHitBeta             ;
  LOG(logINFO) << "ttHitAlphaOver                       " << Stats::ttHitAlphaOver        ;
  LOG(logINFO) << "ttHitBetaOver                        " << Stats::ttHitBetaOver         ;
  LOG(logINFO) << "ttHitUsed                            " << Stats::ttHitUsed             ;
  LOG(logINFO) << "ttCollision                          " << Stats::ttCollision           ;
  LOG(logINFO) << "ttARQHitExact                        " << Stats::ttARQHitExact         ;
  LOG(logINFO) << "ttARQHitAlpha                        " << Stats::ttARQHitAlpha         ;
  LOG(logINFO) << "ttARQHitBeta                         " << Stats::ttARQHitBeta          ;
  LOG(logINFO) << "ttARQHitAlphaOver                    " << Stats::ttARQHitAlphaOver     ;
  LOG(logINFO) << "ttARQHitBetaOver                     " << Stats::ttARQHitBetaOver      ;
  LOG(logINFO) << "ttARQHitUsed                         " << Stats::ttARQHitUsed          ;
  LOG(logINFO) << "ttARQCollision                       " << Stats::ttARQCollision        ;
  LOG(logINFO) << "ttQAllHit                            " << Stats::ttQAllHit             ;
  LOG(logINFO) << "ttQHitExact                          " << Stats::ttQHitExact           ;
  LOG(logINFO) << "ttQHitAlpha                          " << Stats::ttQHitAlpha           ;
  LOG(logINFO) << "ttQHitBeta                           " << Stats::ttQHitBeta            ;
  LOG(logINFO) << "ttQHitAlphaOver                      " << Stats::ttQHitAlphaOver       ;
  LOG(logINFO) << "ttQHitBetaOver                       " << Stats::ttQHitBetaOver        ;
  LOG(logINFO) << "ttQHitUsed                           " << Stats::ttQHitUsed            ;
  LOG(logINFO) << "ttQCollision                         " << Stats::ttQCollision          ;
  LOG(logINFO) << "ttClear                              " << Stats::ttClear               ;
  LOG(logINFO) << "ttQClear                             " << Stats::ttQClear              ;
  LOG(logINFO) << "ttEClear                             " << Stats::ttEClear              ;
  LOG(logINFO) << "ttELClear                            " << Stats::ttELClear             ;
  LOG(logINFO) << "ttEPClear                            " << Stats::ttEPClear             ;
  LOG(logINFO) << "leafNodes                            " << Stats::leafNodes             ;
  LOG(logINFO) << "visitednodes                         " << Stats::visitednodes          ;
  LOG(logINFO) << "visitedqnodes                        " << Stats::visitedqnodes         ;
  LOG(logINFO) << "visitedrealqnodes                    " << Stats::visitedrealqnodes     ;
  LOG(logINFO) << "windowsuccess                        " << Stats::windowsuccess         ;
  LOG(logINFO) << "windowfails                          " << Stats::windowfails           ;
  LOG(logINFO) << "pvsRootsuccess                       " << Stats::pvsRootsuccess        ;
  LOG(logINFO) << "pvsRootfails                         " << Stats::pvsRootfails          ;
  LOG(logINFO) << "pvsAlphaBetasuccess                  " << Stats::pvsAlphaBetasuccess   ;
  LOG(logINFO) << "pvsAlphaBetafails                    " << Stats::pvsAlphaBetafails     ;
  LOG(logINFO) << "analysisnodes                        " << Stats::analysednodes         ;
  LOG(logINFO) << "analysedlazynodes                    " << Stats::analysedlazynodes     ;
  LOG(logINFO) << "ttEvalLazyHits                       " << Stats::ttEvalLazyHits        ;
  LOG(logINFO) << "ttEvalHits                           " << Stats::ttEvalHits            ;
  LOG(logINFO) << "ttEvalPawnHits                       " << Stats::ttEvalPawnHits        ;
  LOG(logINFO) << "ttEvalLazyCollision                  " << Stats::ttEvalLazyCollision   ;
  LOG(logINFO) << "ttEvalCollision                      " << Stats::ttEvalCollision       ;
  LOG(logINFO) << "ttEvalPawnCollision                  " << Stats::ttEvalPawnCollision   ;
  LOG(logINFO) << "terminalnodes                        " << Stats::terminalnodes         ;
  LOG(logINFO) << "openingBookHits                      " << Stats::openingBookHits       ;
  LOG(logINFO) << "positionCopy                         " << Stats::positionCopy          ;
  LOG(logINFO) << "ttsort                               " << Stats::ttsort                ;
  LOG(logINFO) << "ttsorttry                            " << Stats::ttsorttry             ;
  LOG(logINFO) << "pvsort                               " << Stats::pvsort                ;
  LOG(logINFO) << "pvsorttry                            " << Stats::pvsorttry             ;
  LOG(logINFO) << "pvrootsort                           " << Stats::pvrootsort            ;
  LOG(logINFO) << "pvrootsorttry                        " << Stats::pvrootsorttry         ;
  LOG(logINFO) << "iidsort                              " << Stats::iidsort               ;
  LOG(logINFO) << "probCutTry                           " << Stats::probCutTry            ;
  LOG(logINFO) << "probCutSuccess                       " << Stats::probCutSuccess        ;
  LOG(logINFO) << "countCheckExtensionRoot              " << Stats::countCheckExtensionRoot              ;
  LOG(logINFO) << "countCheckExtensionSearch            " << Stats::countCheckExtensionSearch            ;
  LOG(logINFO) << "countCheckExtensionAlphaBeta         " << Stats::countCheckExtensionAlphaBeta         ;
  LOG(logINFO) << "countEndGameExtensionSearch          " << Stats::countEndGameExtensionSearch          ;
  LOG(logINFO) << "countVeryEndGameExtensionSearch      " << Stats::countVeryEndGameExtensionSearch      ;
  LOG(logINFO) << "countPVExtensionSearch               " << Stats::countPVExtensionSearch               ;
  LOG(logINFO) << "countSingleReplyExtensionSearch      " << Stats::countSingleReplyExtensionSearch      ;
  LOG(logINFO) << "countSingularExtensionAlphaBeta      " << Stats::countSingularExtensionAlphaBeta      ;
  LOG(logINFO) << "countNearPromotionExtensionSearch    " << Stats::countNearPromotionExtensionSearch    ;
  LOG(logINFO) << "countNearPromotionExtensionAlphaBeta " << Stats::countNearPromotionExtensionAlphaBeta ;
  LOG(logINFO) << "countReCaptureExtensionSearch        " << Stats::countReCaptureExtensionSearch        ;
  LOG(logINFO) << "countReCaptureExtensionAlphaBeta     " << Stats::countReCaptureExtensionAlphaBeta     ;
  LOG(logINFO) << "tcEBFAbort                           " << Stats::tcEBFAbort            ;
  LOG(logINFO) << "tcForcedAbort                        " << Stats::tcForcedAbort         ;
  LOG(logINFO) << "tcIdealAbort                         " << Stats::tcIdealAbort          ;
  LOG(logINFO) << "*********************************";
  LOG(logINFO) << "Nodes per depths: ";
  for(DepthType k = 0 ; k < MAX_SEARCH_DEPTH ; ++k){
      if ( Stats::nodesCountByDepth[k] == 0 ) break;
      LOG(logINFO) << std::setw(3) << (int)k << " nodes  " << Stats::nodesCountByDepth[k];
      LOG(logINFO) << std::setw(3) << (int)k << " qnodes " << Stats::qnodesCountByDepth[k];
  }
  if (Stats::visitednodes > 0) {
      LOG(logINFO) << "EBF                     " << float(Stats::leafNodes + Stats::visitednodes) / Stats::visitednodes;
  }
  for(DepthType k = 2 ; k < MAX_SEARCH_DEPTH ; ++k){
      if ( Stats::nodesCountByDepth[k]*Stats::nodesCountByDepth[k-2] > 0 ){
        LOG(logINFO) << std::setw(3) << (int)k << " BF                      " << sqrt(float(Stats::nodesCountByDepth[k]) / Stats::nodesCountByDepth[k-2]);
      }
      if ( Stats::qnodesCountByDepth[k]*Stats::qnodesCountByDepth[k-2] > 0 ){
        LOG(logINFO) << std::setw(3) << (int)k << " QBF                     " << sqrt(float(Stats::qnodesCountByDepth[k]) / Stats::qnodesCountByDepth[k-2]);
      }
  }
  for(DepthType k = 0 ; k < MAX_SEARCH_DEPTH ; ++k){
      LOG(logINFO) << std::setw(3) << (int)k << " seldepth  " << Stats::seldepth[k];
  }
  if ( Definitions::ttConfig.do_transpositionTableAlphaBeta && Stats::visitednodes > 0){
    LOG(logINFO) << "TT hit rate (used)      " << 100.f * float(Stats::ttHitUsed) / Stats::visitednodes << "% (" << 100.*Stats::ttHitUsed/float(Stats::ttAllHit) << "%)";
    LOG(logINFO) << "TT collision rate       " << 100.f * float(Stats::ttCollision) / Stats::visitednodes << "%";
    double n = (double)Stats::visitednodes;
    double m = (double)Definitions::ttConfig.ttSize;
    double col = n - m + m * std::pow(1. - 1. / m, n);
    LOG(logINFO) << "col rate (theoretical)    " << (n>0?100.f*col/n:0) << "% ";
  }
  if ( Definitions::ttConfig.do_transpositionTableQuiesce && Stats::visitedqnodes > 0){
    LOG(logINFO) << "QTT hit rate (used)     " << 100.f * float(Stats::ttQHitUsed) / Stats::visitedqnodes << "% (" << 100.*Stats::ttQHitUsed/float(Stats::ttQAllHit) << "%)";
    LOG(logINFO) << "QTT collision rate      " << 100.f * float(Stats::ttQCollision) / Stats::visitedqnodes << "%";
    double n = (double)Stats::visitedqnodes;
    double m = (double)Definitions::ttConfig.ttQSize;
    double col = n - m + m * std::pow(1. - 1. / m, n);
    LOG(logINFO) << "col rate (theoretical)    " << (n>0?100.f*col/n:0) << "%";
  }
  if ( Definitions::ttConfig.do_transpositionTableEval){
    if (Stats::analysednodes > 0) {
      LOG(logINFO) << "ETT hit rate            " << 100.f * float(Stats::ttEvalHits) / Stats::analysednodes << "%";
      LOG(logINFO) << "ETT collision rate      " << 100.f * float(Stats::ttEvalCollision) / Stats::analysednodes << "%";
      double n = (double)Stats::analysednodes;
      double m = (double)Definitions::ttConfig.ttESize;
      double col = n - m + m * std::pow(1. - 1. / m, n);
      LOG(logINFO) << "col rate (theoretical)    " << (n>0?100.f*col/n:0) << "%";
    }
    if ( Stats::analysedlazynodes > 0){
      LOG(logINFO) << "ETTL hit rate           " << 100.f * float(Stats::ttEvalLazyHits) / Stats::analysedlazynodes << "%";
      LOG(logINFO) << "ETTL collision rate     " << 100.f * float(Stats::ttEvalLazyCollision) / Stats::analysedlazynodes << "%";
      double n = (double)Stats::analysedlazynodes;
      double m = (double)Definitions::ttConfig.ttELSize;
      double col = n - m + m * std::pow(1. - 1. / m, n);
      LOG(logINFO) << "col rate (theoretical)    " << (n>0?100.f*col/n:0) << "%";
    }
  }
  if ( Definitions::ttConfig.do_transpositionTableEvalPawn && Stats::analysednodes > 0){
    LOG(logINFO) << "ETTP hit rate           " << 100.f * float(Stats::ttEvalPawnHits) / (Stats::analysednodes-Stats::ttEvalHits) << "%";
    LOG(logINFO) << "ETTP collision rate     " << 100.f * float(Stats::ttEvalPawnCollision) / (Stats::analysednodes-Stats::ttEvalHits) << "%";
    double n = (double)(Stats::analysednodes-Stats::ttEvalHits);
    double m = (double)Definitions::ttConfig.ttEPSize;
    double col = n - m + m * std::pow(1. - 1. / m, n);
    LOG(logINFO) << "col rate (theoretical)    " << (n>0?100.f*col/n:0) << "%";
  }
  if ( Definitions::algo.do_pvsAlphaBeta && Stats::pvsAlphaBetafails + Stats::pvsAlphaBetasuccess > 0){
    LOG(logINFO) << "pvs success rate        " << 100.f * float(Stats::pvsAlphaBetasuccess) / (Stats::pvsAlphaBetafails+Stats::pvsAlphaBetasuccess) << "%";
  }
  if ( Definitions::algo.do_pvsRoot && Stats::pvsAlphaBetafails + Stats::pvsAlphaBetasuccess > 0){
    LOG(logINFO) << "pvs success rate (root) " << 100.f * float(Stats::pvsRootsuccess) / (Stats::pvsRootfails+Stats::pvsRootsuccess) << "%";
  }
  if ( Definitions::algo.do_aspirationwindow && Stats::windowfails + Stats::windowsuccess > 0){
    LOG(logINFO) << "window success rate     " << 100.f * float(Stats::windowsuccess) / (Stats::windowfails+Stats::windowsuccess) << "%";
  }
  if ( Definitions::algo.do_lmrAlphabeta && Stats::lmralphabeta > 0){
    LOG(logINFO) << "lmr success rate        " << 100.f * float(Stats::lmralphabeta-Stats::lmralphabetafail) / (Stats::lmralphabeta) << "%";
  }
  if ( Definitions::algo.do_lmrSearch && Stats::lmrsearch > 0){
    LOG(logINFO) << "lmr success rate (root) " << 100.f * float(Stats::lmrsearch-Stats::lmrsearchfail) / (Stats::lmrsearch) << "%";
  }
  if ( Definitions::algo.do_nullMovePruning && Stats::nullmovecuttry > 0){
    LOG(logINFO) << "nullmove success rate   " << 100.f * float(Stats::nullmovecutbeta) / (Stats::nullmovecuttry) << "%";
  }
  if ( Definitions::algo.do_nullMovePruning && Stats::nullmovecuttry > 0){
    LOG(logINFO) << "nullmove skipped rate   " << 100.f * float(Stats::nullmovecutskipped) / (Stats::nullmovecuttry) << "%";
  }
  if ( (Definitions::ttConfig.do_transpositionTableSearch || Definitions::ttConfig.do_transpositionTableSortAlphaBeta) && Stats::ttsorttry > 0){
    LOG(logINFO) << "tt sort success rate    " << 100.f * float(Stats::ttsort) / (Stats::ttsorttry) << "% (should be 100 %)";
  }
  if ( Definitions::ttConfig.do_transpositionTableQSort && Stats::ttQsorttry > 0){
    LOG(logINFO) << "Qtt sort success rate   " << 100.f * float(Stats::ttQsort) / (Stats::ttQsorttry) << "% (should be 100 %)";
  }
  if ( Stats::pvsorttry > 0){
    LOG(logINFO) << "pv sort success rate    " << 100.f * float(Stats::pvsort) / (Stats::pvsorttry) << "% (around 100/EBF % if TT sort is inactive, around 0 otherwise)";
  }
}

void Stats::NodeCount::Print(){
  LOG(logINFO) << "createdNodes   " << createdNodes;
  LOG(logINFO) << "pseudoNodes    " << pseudoNodes;
  LOG(logINFO) << "validNodes     " << validNodes;
  LOG(logINFO) << "captureNodes   " << captureNodes;
  LOG(logINFO) << "epNodes        " << epNodes;
  LOG(logINFO) << "promotionNodes " << promotionNodes;
  LOG(logINFO) << "checkNodes     " << checkNodes;
  LOG(logINFO) << "checkMateNodes " << checkMateNodes;
}

void Stats::NodeCount::Reset(){
  createdNodes   = 0ull;
  pseudoNodes    = 0ull;
  validNodes     = 0ull;
  captureNodes   = 0ull;
  epNodes        = 0ull;
  promotionNodes = 0ull;
  checkNodes     = 0ull;
  checkMateNodes = 0ull;
}
