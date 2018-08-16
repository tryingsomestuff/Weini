#include "definitions.h"
#include "logger.hpp"

#include <fstream>

//*****************************
// Piece values
//*****************************
Definitions::PieceValue Definitions::pieceValue;

Definitions::PieceValue::PieceValue() {
    pawnValueW      = 100;
    knightValueW    = 315;
    bishopValueW    = 325;
    rookValueW      = 500;
    queenValueW     = 900;
    kingValueW      = 20000;
    pawnEGValueW    = 100;
    knightEGValueW  = 255;
    bishopEGValueW  = 275;
    rookEGValueW    = 550;
    queenEGValueW   = 1200;
    kingEGValueW    = 20000;

    Init();
}

void Definitions::PieceValue::Init(){
    pawnValueB      = -pawnValueW     ;
    knightValueB    = -knightValueW   ;
    bishopValueB    = -bishopValueW   ;
    rookValueB      = -rookValueW     ;
    queenValueB     = -queenValueW    ;
    kingValueB      = -kingValueW     ;
    pawnEGValueB    = -pawnEGValueW   ;
    knightEGValueB  = -knightEGValueW ;
    bishopEGValueB  = -bishopEGValueW ;
    rookEGValueB    = -rookEGValueW   ;
    queenEGValueB   = -queenEGValueW  ;
    kingEGValueB    = -kingEGValueW   ;
}

//*****************************
// TC config
//*****************************
Definitions::TimeControl Definitions::timeControl;

Definitions::TimeControl::TimeControl() {
    currentMoveMs               = 10000;     // millisec
    timeControl                 = 120;       // sec
    increment                   = 0;         // sec
    movePerSession              = 40;        // number of move until next TC start
    emergencyTimeFactor         = 7;
    emergencyScoreMargin        = 150;
    minMoveToGo                 = 24;
    maxMoveToGo                 = 65;
}

//*****************************
// Score config
//*****************************
Definitions::Scores Definitions::scores;

Definitions::Scores::Scores() {
    epSortScore             = 10;
    checkSortScore          = 500;
    reductibleSortScore     = 1200;
    counterSortScore        = 1500;
    //historyMax            = 1800;
    killer2SortScore        = 2000;
    killer1SortScore        = 4000;
    captureLastMovedBonus   = 600;
    bookScore               = 5000;
    captureSortScore        = 6000;
    promotionSortScore      = 8000;
    iidSortScore            = 10000;
    pvSortScore             = 12000;
    ttSortScore             = 14000;
    defaultMoveScore        = 32000;

    infScore                = 32222;
    checkMateScore          = 7900;  // must be big and < Definitions::infScore
    contempt                = 10;
    stopFlagScore           = 31000; // must be bigger than mate to ensure easy debug
}

//*****************************
// Selectivity config
//*****************************
Definitions::Selectivity Definitions::selectivity;

Definitions::Selectivity::Selectivity(){
    quies_max               = 48;
    windowSize              = 80;
    seeThreshold            = -100;
    exponentialWindowGrowth = 4.f;
    nullMoveMargin          = 0;
    futilityMargin          = 0;
    futilityDepthCoeff      = 80;
    qfutilityMargin         = 128;
    deltaMargin             = 1300;
    razoringMargin          = 200;
    razoringDepthCoeff      = 0;
    staticNullMoveMargin    = 0;
    staticNullMoveDepthCoeff= 80;
    probCutMargin           = 200;
    probCutDepthCoeff       = 0;
    lmrtol                  = 100;
    LMR_STD                 = 2;
    LMR_ENDGAME             = 2;

    extensionRootMax        = 2;
    extensionAlphaBetaMax   = 1;

    iterativeDeepeningSelectivityMinDepth = 3;
    staticNullMoveMaxDepth                = 3;
    futilityMaxDepth                      = 6;
    razoringMaxDepth                      = 3;
    nullMovePruningMinDepth               = 2;
    verifiedNullMoveMinDepth              = 6;
    moveCountPruningMaxDepth              = 10;
    recursiveNullMoveMinDepth             = 5;
    seePruningAlphaBetaMaxDepth           = 6;
    lmrMinDepth                           = 2;
    probCutMinDepth                       = 5;
    IIDMinDepth                           = 5;

    probCutMaxMoves                       = 3;
}

//*****************************
// Evaluation config
//*****************************
Definitions::EvalBonus Definitions::evalBonus;

Definitions::EvalBonus::EvalBonus(){

  adjKnight    = { -20, -16, -12, -8, -4,  0,  4,  8, 12 }; // seems correct (CPW)...
  adjRook      = {  15,  12,   9,  6,  3,  0, -3, -6, -9 }; // seems correct (CPW)...

  bishopPairBonus             = 20;  // seems correct (CPW)...
  knightPairMalus             = -8;  // seems correct (CPW)...
  rookPairMalus               = -16; // seems correct (CPW)...

  mobilityValueDivisor        = 300;

  kingSafetyUnitaryBonus      = 1.f;
  kingSafetyUnitaryBonus2     = 0.5f;
  kingSafetyFiancietto        = 1.f;
  kingSafetyNoFianciettoMalus = -0.5f;
  kingSafetyFactor            = 35;

  stormFactor                 = 10;  ///@todo to be tuned

  badBishopMalus              = -15; ///@todo not sure

  isolatedPawnMalus           = -15; ///@todo not sure
  doublePawnMalus             = -24; ///@todo not sure
  bonusCandidatePassedPawn    = 10;  ///@todo not sure

  rookOnOpenFile              = 25;  // seems correct ...
  rookOnSemiOpenFile          = 15;  ///@todo not sure
  openFileNearKing            = 30;

  spaceFactor                 = 3;   // smaller do nothing, higher is too risky play
  tradeOffPieceBonusFactor    = 5;
  tradeOffPawnBonusFactor     = 15;
  tradeOffLimit               = 185;
  noPawnMalus                 = -10;

  blockedBishopByPawn         = -24;  // seems correct (CPW)...
  blockedKnight               = -150; // seems correct (CPW)...
  blockedKnight2              = -100; // seems correct (CPW)...
  blockedBishop               = -150; // seems correct (CPW)...
  blockedBishop2              = -100; // seems correct (CPW)...
  blockedBishop3              = -50;  // seems correct (CPW)...
  returningBishopBonus        =  16;  // seems correct (CPW)...
  blockedRookByKing           = -22;  // seems correct (CPW)...

  passedPawn     = { 0, 10, 20, 35, 45, 60, 70, 200 }; ///@todo to be tuned

  protectedPassedPawnFactor   = 1.1f; ///@todo to be tuned
  connectedPassedPawnFactor   = 1.1f; ///@todo to be tuned
  outsidePassedPawnFactor     = 1.1f; ///@todo to be tuned
  kingTooFarPassedPawnFactor  = 1.1f; ///@todo to be tuned

  kingInCheckMalus            = 10;   ///@todo to be tuned

  hasCastleBonus              = 10;
  canCastleBonus              = 5;
  castleBonusMiddleGameFactor = 1.5f;
  castlePlyLimit              = 8;

  connectedRooksBonus         = 15;

  centerControlFactor         = 5.f;

  kingTroppismFrontFactor     = 2;
  kingTroppismFrontFactor2    = 2;
  kingTroppismSameRankFactor  = 3;
  kingTroppismBackRankFactor  = 2;
  kingTroppismBackRankFactor2 = 1;
  kingTroppismValueFactor     = 15;
  kingTroppismAttackWeight = { 0.f,0.f,0.35f,0.50f,0.75f,0.85f,0.90f,0.95f,1.f }; // max 8

  tempoBonus              = 33;

}

//*****************************
// Evaluation options
//*****************************
Definitions::EvalConfig Definitions::evalConfig;

Definitions::EvalConfig::EvalConfig(){
    evalWithTapered          = true;
    evalWithInCheck          = false;
    evalWithCastle           = false; // seems redoundant with PSQT
    evalWithConnectedRooks   = false; // seems redoundant with PSQT
    evalWithKingTroppism     = false; // too slow ?
    evalWithSpace            = false; // validated
    evalWithMobility         = false; // too slow
    evalWithPawnStructure    = true;
    evalWithPawnStructure2   = true;
    evalWithPawnStructure3   = true;
    evalWithRookOnOpenFile   = true;  // validated (still need some optim)
    evalWithOpenFileNearKing = false;
    evalWithPawnShield       = true;
    evalWithBadBishop        = true;  // not sure at all it gain strength
    evalWithPiecePair        = true;  // validated
    evalWithBlockedPiece     = true;  // validated
    evalWithCenterControl    = false; // too slow
    evalWithPawnStorm        = false;
    evalWithTradeOffBonus    = true;
    evalWithNoPawnPenalty    = true;
    do_randomNoise           = false;
}

//*****************************
// TT config
//*****************************
Definitions::TranspositionTableConfig Definitions::ttConfig;

Definitions::TranspositionTableConfig::TranspositionTableConfig(){
    do_transpositionTableSearch        = false; // this loss elo
    do_transpositionTableAlphaBeta     = true;
    do_transpositionTableSortSearch    = true;
    do_transpositionTableSortAlphaBeta = true;
    do_transpositionTableQSort         = true;
    do_transpositionTableQuiesce       = true;
    do_transpositionTableEval          = true;
    do_transpositionTableEvalPawn      = true;
    do_ttAging                         = false;

    ttSize              = 72;
    ttQSize             = 36;
    ttESize             = 64;
    ttELSize            = 32;
    ttEPSize            = 32;
}

//*****************************
// Extension config
//*****************************
Definitions::ExtensionReductionConfig Definitions::extensionReductionConfig;

Definitions::ExtensionReductionConfig::ExtensionReductionConfig(){
    do_CheckExtensionRoot              = true;
    do_CheckExtensionSearch            = true;
    do_CheckExtensionAlphaBeta         = true;
    do_EndGameExtensionSearch          = false;
    do_VeryEndGameExtensionSearch      = false;
    do_PVExtensionSearch               = false;
    do_SingleReplyExtensionSearch      = true;
    do_SingleReplyExtensionAlphaBeta   = true;
    do_SingularExtensionAlphaBeta      = false;
    do_NearPromotionExtensionSearch    = true;
    do_NearPromotionExtensionAlphaBeta = true;
    do_ReCaptureExtensionSearch        = true;
    do_ReCaptureExtensionAlphaBeta     = true;

    do_ttmoveCaptureReduction          = true;
}

//*****************************
// Sorting config
//*****************************
Definitions::SortingConfig Definitions::sortingConfig;

Definitions::SortingConfig::SortingConfig(){
    do_sortmvvlva            = true;
    do_sortsee               = true;
    do_qsortmvvlva           = true;
    do_qsortsee              = true;
    do_sortpositional        = true;
    do_sortenpassantbonus    = true;
    do_sortcheckbonus        = false;
    do_captureLastMovedBonus = true;
    do_internaliterativeDeepening = true;
    do_killerHeuristic       = true;
    do_counterHeuristic      = true;
    do_historyHeuristic      = true;
}

//*****************************
// Book config
//*****************************
Definitions::BookConfig Definitions::bookConfig;

Definitions::BookConfig::BookConfig(){
    with_bigBook     = false;
    with_smallBook   = true;
    bookSkip         = 1;
    bookFileName     = "Book/book3.pgn";
    maxBookMoves     = 24;
    debugBook        = false;
}

//*****************************
// Algo config
//*****************************
Definitions::Algorithms Definitions::algo;

Definitions::Algorithms::Algorithms(){
    do_failsoftalphabeta  = true;
    do_failsoftquiesce    = true;
    do_alphabetaminimax   = false;
    do_quiesceminimax     = false;
    do_quiesce            = true;
    do_futilityPruning    = true;
    do_qfutilityPruning   = true;
    do_razoring           = true;
    do_staticNullMove     = true;
    do_seePruning         = false;
    do_seePruningAlphaBeta= true;
    do_deltaPruning       = true;
    do_nullMovePruning    = true;
    do_verifiedNullMove   = false;
    do_lmrAlphabeta       = true;
    do_lmrQuiesce         = false;  ///@todo !?
    do_lmrSearch          = true;
    do_moveCountPruning   = true;
    do_aspirationwindow   = true;
    do_pvsRoot            = true;
    do_pvsAlphaBeta       = true;
    do_lazyEvaluation     = false;
    do_probCut            = false;
    do_iterativeDeepening = true;
    do_trustedGenerator   = false;
    forceNoPonder         = false;
}

//*****************************
// SMP config
//*****************************
Definitions::SMPConfig Definitions::smpConfig;

Definitions::SMPConfig::SMPConfig(){
    threads                       = 1;
    shuffleThreadMoves            = false;
    shuffleThreadMovesAlphaBeta   = false;
}

//*****************************
// Debug config
//*****************************
Definitions::DebugConfig Definitions::debugConfig;

Definitions::DebugConfig::DebugConfig(){
    nameSuffixe           = "";
    modedebug             = false;
    do_analysisNodeRate   = true;
    defaultDepth          = MAX_SEARCH_DEPTH-1;
}



std::vector<Definitions::KeyBase> Definitions::_keys;

void Definitions::InitKeys() {

    LOG(logINFO) << "Init definition keys";

    //*****************************
    // Piece values
    //*****************************
    _keys.push_back(KeyBase(k_score, "pawnValue"      , &pieceValue.pawnValueW    , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "knightValue"    , &pieceValue.knightValueW  , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "bishopValue"    , &pieceValue.bishopValueW  , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "rookValue"      , &pieceValue.rookValueW    , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "queenValue"     , &pieceValue.queenValueW   , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "kingValue"      , &pieceValue.kingValueW    , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "pawnEGValue"    , &pieceValue.pawnEGValueW  , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "knightEGValue"  , &pieceValue.knightEGValueW, std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "bishopEGValue"  , &pieceValue.bishopEGValueW, std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "rookEGValue"    , &pieceValue.rookEGValueW  , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "queenEGValue"   , &pieceValue.queenEGValueW , std::bind(&PieceValue::Init, &pieceValue)));
    _keys.push_back(KeyBase(k_score, "kingEGValue"    , &pieceValue.kingEGValueW  , std::bind(&PieceValue::Init, &pieceValue)));

    //*****************************
    // TC config
    //*****************************
    _keys.push_back(KeyBase(k_ull, "tc"                                  , &timeControl.timeControl));
    _keys.push_back(KeyBase(k_ull, "st"                                  , &timeControl.currentMoveMs));
    _keys.push_back(KeyBase(k_ull, "inc"                                 , &timeControl.increment));
    _keys.push_back(KeyBase(k_ull, "mps"                                 , &timeControl.movePerSession));
    _keys.push_back(KeyBase(k_float, "emergencyTimeFactor"               , &timeControl.emergencyTimeFactor));
    _keys.push_back(KeyBase(k_float, "emergencyScoreMargin"              , &timeControl.emergencyScoreMargin));
    _keys.push_back(KeyBase(k_depth, "minMoveToGo"                       , &timeControl.minMoveToGo));
    _keys.push_back(KeyBase(k_depth, "maxMoveToGo"                       , &timeControl.maxMoveToGo));

    //*****************************
    // Score config
    //*****************************
    _keys.push_back(KeyBase(k_score, "contempt"                          , &scores.contempt));

    //*****************************
    // Selectivity config
    //*****************************
    _keys.push_back(KeyBase(k_depth, "quies_max"                         , &selectivity.quies_max));
    _keys.push_back(KeyBase(k_score, "windowSize"                        , &selectivity.windowSize));
    _keys.push_back(KeyBase(k_float, "exponentialWindowGrowth"           , &selectivity.exponentialWindowGrowth));
    _keys.push_back(KeyBase(k_score, "nullMoveMargin"                    , &selectivity.nullMoveMargin));
    _keys.push_back(KeyBase(k_score, "futilityMargin"                    , &selectivity.futilityMargin));
    _keys.push_back(KeyBase(k_score, "futilityDepthCoeff"                , &selectivity.futilityDepthCoeff));
    _keys.push_back(KeyBase(k_score, "qfutilityMargin"                   , &selectivity.qfutilityMargin));
    _keys.push_back(KeyBase(k_score, "deltaMargin"                       , &selectivity.deltaMargin));
    _keys.push_back(KeyBase(k_score, "seeThreshold"                      , &selectivity.seeThreshold));
    _keys.push_back(KeyBase(k_score, "razoringMargin"                    , &selectivity.razoringMargin));
    _keys.push_back(KeyBase(k_score, "razoringDepthCoeff"                , &selectivity.razoringDepthCoeff));
    _keys.push_back(KeyBase(k_score, "staticNullMoveMargin"              , &selectivity.staticNullMoveMargin));
    _keys.push_back(KeyBase(k_score, "staticNullMoveDepthCoeff"          , &selectivity.staticNullMoveDepthCoeff));
    _keys.push_back(KeyBase(k_score, "probCutMargin"                     , &selectivity.probCutMargin));
    _keys.push_back(KeyBase(k_score, "probCutDepthCoeff"                 , &selectivity.probCutDepthCoeff));

    _keys.push_back(KeyBase(k_score, "lmrtol"                            , &selectivity.lmrtol));
    _keys.push_back(KeyBase(k_depth, "LMR_STD"                           , &selectivity.LMR_STD));
    _keys.push_back(KeyBase(k_depth, "LMR_ENDGAME"                       , &selectivity.LMR_ENDGAME));

    _keys.push_back(KeyBase(k_depth, "extensionRootMax"                  , &selectivity.extensionRootMax));
    _keys.push_back(KeyBase(k_depth, "extensionAlphaBetaMax"             , &selectivity.extensionAlphaBetaMax));

    _keys.push_back(KeyBase(k_depth, "iterativeDeepeningSelectivityMinDepth", &selectivity.iterativeDeepeningSelectivityMinDepth));
    _keys.push_back(KeyBase(k_depth, "staticNullMoveMaxDepth"               , &selectivity.staticNullMoveMaxDepth));
    _keys.push_back(KeyBase(k_depth, "futilityMaxDepth"                     , &selectivity.futilityMaxDepth));
    _keys.push_back(KeyBase(k_depth, "razoringMaxDepth"                     , &selectivity.razoringMaxDepth));
    _keys.push_back(KeyBase(k_depth, "nullMovePruningMinDepth"              , &selectivity.nullMovePruningMinDepth));
    _keys.push_back(KeyBase(k_depth, "verifiedNullMoveMinDepth"             , &selectivity.verifiedNullMoveMinDepth));
    _keys.push_back(KeyBase(k_depth, "moveCountPruningMaxDepth"             , &selectivity.moveCountPruningMaxDepth));
    _keys.push_back(KeyBase(k_depth, "recursiveNullMoveMinDepth"            , &selectivity.recursiveNullMoveMinDepth));
    _keys.push_back(KeyBase(k_depth, "seePruningAlphaBetaMaxDepth"          , &selectivity.seePruningAlphaBetaMaxDepth));
    _keys.push_back(KeyBase(k_depth, "lmrMinDepth"                          , &selectivity.lmrMinDepth));
    _keys.push_back(KeyBase(k_depth, "probCutMinDepth"                      , &selectivity.probCutMinDepth));
    _keys.push_back(KeyBase(k_int,   "probCutMaxMoves"                      , &selectivity.probCutMaxMoves));
    _keys.push_back(KeyBase(k_depth, "IIDMinDepth"                          , &selectivity.IIDMinDepth));

    //*****************************
    // Evaluation config
    //*****************************
    _keys.push_back(KeyBase(k_table_score, "adjKnight"                   , &evalBonus.adjKnight));
    _keys.push_back(KeyBase(k_table_score, "adjRook"                     , &evalBonus.adjRook));

    _keys.push_back(KeyBase(k_score, "bishopPairBonus"                   , &evalBonus.bishopPairBonus));
    _keys.push_back(KeyBase(k_score, "knightPairMalus"                   , &evalBonus.knightPairMalus));
    _keys.push_back(KeyBase(k_score, "rookPairMalus"                     , &evalBonus.rookPairMalus));
    _keys.push_back(KeyBase(k_float, "mobilityValueDivisor"              , &evalBonus.mobilityValueDivisor));

    _keys.push_back(KeyBase(k_float, "kingSafetyUnitaryBonus"            , &evalBonus.kingSafetyUnitaryBonus));
    _keys.push_back(KeyBase(k_float, "kingSafetyUnitaryBonus2"           , &evalBonus.kingSafetyUnitaryBonus2));
    _keys.push_back(KeyBase(k_float, "kingSafetyFiancietto"              , &evalBonus.kingSafetyFiancietto));
    _keys.push_back(KeyBase(k_float, "kingSafetyNoFianciettoMalus"       , &evalBonus.kingSafetyNoFianciettoMalus));
    _keys.push_back(KeyBase(k_score, "kingSafetyFactor"                  , &evalBonus.kingSafetyFactor));

    _keys.push_back(KeyBase(k_score, "stormFactor"                       , &evalBonus.stormFactor));

    _keys.push_back(KeyBase(k_score, "badBishopMalus"                    , &evalBonus.badBishopMalus));

    _keys.push_back(KeyBase(k_score, "isolatedPawnMalus"                 , &evalBonus.isolatedPawnMalus));
    _keys.push_back(KeyBase(k_score, "doublePawnMalus"                   , &evalBonus.doublePawnMalus));
    _keys.push_back(KeyBase(k_score, "bonusCandidatePassedPawn"          , &evalBonus.bonusCandidatePassedPawn));

    _keys.push_back(KeyBase(k_score, "rookOnOpenFile"                    , &evalBonus.rookOnOpenFile));
    _keys.push_back(KeyBase(k_score, "rookOnSemiOpenFile"                , &evalBonus.rookOnSemiOpenFile));
    _keys.push_back(KeyBase(k_score, "openFileNearKing"                  , &evalBonus.openFileNearKing));

    _keys.push_back(KeyBase(k_score, "spaceFactor"                       , &evalBonus.spaceFactor));
    _keys.push_back(KeyBase(k_score, "tradeOffPieceBonusFactor"          , &evalBonus.tradeOffPieceBonusFactor));
    _keys.push_back(KeyBase(k_score, "tradeOffPawnBonusFactor"           , &evalBonus.tradeOffPawnBonusFactor));
    _keys.push_back(KeyBase(k_score, "tradeOffLimit"                     , &evalBonus.tradeOffLimit));
    _keys.push_back(KeyBase(k_score, "noPawnMalus"                       , &evalBonus.noPawnMalus));

    _keys.push_back(KeyBase(k_score, "blockedBishopByPawn"               , &evalBonus.blockedBishopByPawn));
    _keys.push_back(KeyBase(k_score, "blockedKnight"                     , &evalBonus.blockedKnight));
    _keys.push_back(KeyBase(k_score, "blockedKnight2"                    , &evalBonus.blockedKnight2));
    _keys.push_back(KeyBase(k_score, "blockedBishop"                     , &evalBonus.blockedBishop));
    _keys.push_back(KeyBase(k_score, "blockedBishop2"                    , &evalBonus.blockedBishop2));
    _keys.push_back(KeyBase(k_score, "blockedBishop3"                    , &evalBonus.blockedBishop3));
    _keys.push_back(KeyBase(k_score, "returningBishopBonus"              , &evalBonus.returningBishopBonus));
    _keys.push_back(KeyBase(k_score, "blockedRookByKing"                 , &evalBonus.blockedRookByKing));

    _keys.push_back(KeyBase(k_table_score, "passedPawn"                  , &evalBonus.passedPawn));
    _keys.push_back(KeyBase(k_float, "protectedPassedPawnFactor"         , &evalBonus.protectedPassedPawnFactor));
    _keys.push_back(KeyBase(k_float, "connectedPassedPawnFactor"         , &evalBonus.connectedPassedPawnFactor));
    _keys.push_back(KeyBase(k_float, "outsidePassedPawnFactor"           , &evalBonus.outsidePassedPawnFactor));
    _keys.push_back(KeyBase(k_float, "kingTooFarPassedPawnFactor"        , &evalBonus.kingTooFarPassedPawnFactor));

    _keys.push_back(KeyBase(k_score, "kingInCheckMalus"                  , &evalBonus.kingInCheckMalus));

    _keys.push_back(KeyBase(k_score, "hasCastleBonus"                    , &evalBonus.hasCastleBonus));
    _keys.push_back(KeyBase(k_score, "canCastleBonus"                    , &evalBonus.canCastleBonus));
    _keys.push_back(KeyBase(k_float, "castleBonusMiddleGameFactor"       , &evalBonus.castleBonusMiddleGameFactor));
    _keys.push_back(KeyBase(k_depth, "castlePlyLimit"                    , &evalBonus.castlePlyLimit));

    _keys.push_back(KeyBase(k_score, "connectedRooksBonus"               , &evalBonus.connectedRooksBonus));

    _keys.push_back(KeyBase(k_float, "centerControlFactor"               , &evalBonus.centerControlFactor));

    _keys.push_back(KeyBase(k_float, "kingTroppismFrontFactor"           , &evalBonus.kingTroppismFrontFactor));
    _keys.push_back(KeyBase(k_float, "kingTroppismFrontFactor2"          , &evalBonus.kingTroppismFrontFactor2));
    _keys.push_back(KeyBase(k_float, "kingTroppismSameRankFactor"        , &evalBonus.kingTroppismSameRankFactor));
    _keys.push_back(KeyBase(k_float, "kingTroppismBackRankFactor"        , &evalBonus.kingTroppismBackRankFactor));
    _keys.push_back(KeyBase(k_float, "kingTroppismBackRankFactor2"       , &evalBonus.kingTroppismBackRankFactor2));
    _keys.push_back(KeyBase(k_float, "kingTroppismValueFactor"           , &evalBonus.kingTroppismValueFactor));
    _keys.push_back(KeyBase(k_table_float, "kingTroppismAttackWeight"    , &evalBonus.kingTroppismAttackWeight));

    _keys.push_back(KeyBase(k_score, "tempoBonus"                        , &evalBonus.tempoBonus));

    //*****************************
    // Evaluation options
    //*****************************
    _keys.push_back(KeyBase(k_bool, "evalWithTapered"                    , &evalConfig.evalWithTapered));
    _keys.push_back(KeyBase(k_bool, "evalWithInCheck"                    , &evalConfig.evalWithInCheck));
    _keys.push_back(KeyBase(k_bool, "evalWithCastle"                     , &evalConfig.evalWithCastle));
    _keys.push_back(KeyBase(k_bool, "evalWithConnectedRooks"             , &evalConfig.evalWithConnectedRooks));
    _keys.push_back(KeyBase(k_bool, "evalWithKingTroppism"               , &evalConfig.evalWithKingTroppism));
    _keys.push_back(KeyBase(k_bool, "evalWithSpace"                      , &evalConfig.evalWithSpace));
    _keys.push_back(KeyBase(k_bool, "evalWithMobility"                   , &evalConfig.evalWithMobility));
    _keys.push_back(KeyBase(k_bool, "evalWithPawnStructure"              , &evalConfig.evalWithPawnStructure));
    _keys.push_back(KeyBase(k_bool, "evalWithPawnStructure2"             , &evalConfig.evalWithPawnStructure2));
    _keys.push_back(KeyBase(k_bool, "evalWithPawnStructure3"             , &evalConfig.evalWithPawnStructure3));
    _keys.push_back(KeyBase(k_bool, "evalWithRookOnOpenFile"             , &evalConfig.evalWithRookOnOpenFile));
    _keys.push_back(KeyBase(k_bool, "evalWithOpenFileNearKing"           , &evalConfig.evalWithOpenFileNearKing));
    _keys.push_back(KeyBase(k_bool, "evalWithPawnShield"                 , &evalConfig.evalWithPawnShield));
    _keys.push_back(KeyBase(k_bool, "evalWithBadBishop"                  , &evalConfig.evalWithBadBishop));
    _keys.push_back(KeyBase(k_bool, "evalWithPiecePair"                  , &evalConfig.evalWithPiecePair));
    _keys.push_back(KeyBase(k_bool, "evalWithBlockedPiece"               , &evalConfig.evalWithBlockedPiece));
    _keys.push_back(KeyBase(k_bool, "evalWithCenterControl"              , &evalConfig.evalWithCenterControl));
    _keys.push_back(KeyBase(k_bool, "evalWithPawnStorm"                  , &evalConfig.evalWithPawnStorm));
    _keys.push_back(KeyBase(k_bool, "evalWithTradeOffBonus"              , &evalConfig.evalWithTradeOffBonus));
    _keys.push_back(KeyBase(k_bool, "evalWithNoPawnPenalty"              , &evalConfig.evalWithNoPawnPenalty));
    _keys.push_back(KeyBase(k_bool, "do_randomNoise"                     , &evalConfig.do_randomNoise));

    //*****************************
    // TT config
    //*****************************
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableSearch"        , &ttConfig.do_transpositionTableSearch));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableAlphaBeta"     , &ttConfig.do_transpositionTableAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableSortSearch"    , &ttConfig.do_transpositionTableSortSearch));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableSortAlphaBeta" , &ttConfig.do_transpositionTableSortAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableQSort"         , &ttConfig.do_transpositionTableQSort));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableEval"          , &ttConfig.do_transpositionTableEval));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableEvalPawn"      , &ttConfig.do_transpositionTableEvalPawn));
    _keys.push_back(KeyBase(k_bool, "do_transpositionTableQuiesce"       , &ttConfig.do_transpositionTableQuiesce));
    _keys.push_back(KeyBase(k_bool, "do_ttAging"                         , &ttConfig.do_ttAging));
    _keys.push_back(KeyBase(k_ull,  "ttSize"                             , &ttConfig.ttSize));
    _keys.push_back(KeyBase(k_ull,  "ttQSize"                            , &ttConfig.ttQSize));
    _keys.push_back(KeyBase(k_ull,  "ttESize"                            , &ttConfig.ttESize));
    _keys.push_back(KeyBase(k_ull,  "ttELSize"                           , &ttConfig.ttELSize));
    _keys.push_back(KeyBase(k_ull,  "ttEPSize"                           , &ttConfig.ttEPSize));

    //*****************************
    // Extension config
    //*****************************
    _keys.push_back(KeyBase(k_bool, "do_checkExtensionRoot"              , &extensionReductionConfig.do_CheckExtensionRoot));
    _keys.push_back(KeyBase(k_bool, "do_checkExtensionSearch"            , &extensionReductionConfig.do_CheckExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_checkExtensionAlphaBeta"         , &extensionReductionConfig.do_CheckExtensionAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_NearPromotionExtensionAlphaBeta" , &extensionReductionConfig.do_NearPromotionExtensionAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_EndGameExtensionSearch"          , &extensionReductionConfig.do_EndGameExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_VeryEndGameExtensionSearch"      , &extensionReductionConfig.do_VeryEndGameExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_PVExtensionSearch"               , &extensionReductionConfig.do_PVExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_SingleReplyExtensionSearch"      , &extensionReductionConfig.do_SingleReplyExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_SingleReplyExtensionAlphaBeta"   , &extensionReductionConfig.do_SingleReplyExtensionAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_SingularExtensionAlphaBeta"      , &extensionReductionConfig.do_SingularExtensionAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_NearPromotionExtensionSearch"    , &extensionReductionConfig.do_NearPromotionExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_ReCaptureExtensionSearch"        , &extensionReductionConfig.do_ReCaptureExtensionSearch));
    _keys.push_back(KeyBase(k_bool, "do_ReCaptureExtensionAlphaBeta"     , &extensionReductionConfig.do_ReCaptureExtensionAlphaBeta));

    _keys.push_back(KeyBase(k_bool, "do_ttmoveCaptureReduction"          , &extensionReductionConfig.do_ttmoveCaptureReduction));

    //*****************************
    // Sorting config
    //*****************************
    _keys.push_back(KeyBase(k_bool, "do_sortmvvlva"                      , &sortingConfig.do_sortmvvlva));
    _keys.push_back(KeyBase(k_bool, "do_sortsee"                         , &sortingConfig.do_sortsee));
    _keys.push_back(KeyBase(k_bool, "do_qsortmvvlva"                     , &sortingConfig.do_qsortmvvlva));
    _keys.push_back(KeyBase(k_bool, "do_qsortsee"                        , &sortingConfig.do_qsortsee));
    _keys.push_back(KeyBase(k_bool, "do_sortpositional"                  , &sortingConfig.do_sortpositional));
    _keys.push_back(KeyBase(k_bool, "do_sortenpassantbonus"              , &sortingConfig.do_sortenpassantbonus));
    _keys.push_back(KeyBase(k_bool, "do_sortcheckbonus"                  , &sortingConfig.do_sortcheckbonus));
    _keys.push_back(KeyBase(k_bool, "do_captureLastMovedBonus"           , &sortingConfig.do_captureLastMovedBonus));
    _keys.push_back(KeyBase(k_bool, "do_internaliterativeDeepening"      , &sortingConfig.do_internaliterativeDeepening));
    _keys.push_back(KeyBase(k_bool, "do_killerHeuristic"                 , &sortingConfig.do_killerHeuristic));
    _keys.push_back(KeyBase(k_bool, "do_counterHeuristic"                , &sortingConfig.do_counterHeuristic));
    _keys.push_back(KeyBase(k_bool, "do_historyHeuristic"                , &sortingConfig.do_historyHeuristic));

    //*****************************
    // Book config
    //*****************************
    _keys.push_back(KeyBase(k_bool,   "with_bigBook"                     , &bookConfig.with_bigBook));
    _keys.push_back(KeyBase(k_bool,   "with_smallBook"                   , &bookConfig.with_smallBook));
    _keys.push_back(KeyBase(k_int,    "bookSkip"                         , &bookConfig.bookSkip));
    _keys.push_back(KeyBase(k_string, "bookFileName"                     , &bookConfig.bookFileName));
    _keys.push_back(KeyBase(k_int,    "maxBookMoves"                     , &bookConfig.maxBookMoves));
    _keys.push_back(KeyBase(k_bool,   "debugBook"                        , &bookConfig.debugBook));

    //*****************************
    // Algo config
    //*****************************
    _keys.push_back(KeyBase(k_bool, "do_failsoftalphabeta"               , &algo.do_failsoftalphabeta));
    _keys.push_back(KeyBase(k_bool, "do_failsoftquiesce"                 , &algo.do_failsoftquiesce));
    _keys.push_back(KeyBase(k_bool, "do_alphabetaminimax"                , &algo.do_alphabetaminimax));
    _keys.push_back(KeyBase(k_bool, "do_quiesceminimax"                  , &algo.do_quiesceminimax));
    _keys.push_back(KeyBase(k_bool, "do_quiesce"                         , &algo.do_quiesce));
    _keys.push_back(KeyBase(k_bool, "do_futilityPruning"                 , &algo.do_futilityPruning));
    _keys.push_back(KeyBase(k_bool, "do_qfutilityPruning"                , &algo.do_qfutilityPruning));
    _keys.push_back(KeyBase(k_bool, "do_razoring"                        , &algo.do_razoring));
    _keys.push_back(KeyBase(k_bool, "do_staticNullMove"                  , &algo.do_staticNullMove));
    _keys.push_back(KeyBase(k_bool, "do_seePruning"                      , &algo.do_seePruning));
    _keys.push_back(KeyBase(k_bool, "do_seePruningAlphaBeta"             , &algo.do_seePruningAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_deltaPruning"                    , &algo.do_deltaPruning));
    _keys.push_back(KeyBase(k_bool, "do_nullMovePruning"                 , &algo.do_nullMovePruning));
    _keys.push_back(KeyBase(k_bool, "do_verifiedNullMove"                , &algo.do_verifiedNullMove));
    _keys.push_back(KeyBase(k_bool, "do_lmrAlphabeta"                    , &algo.do_lmrAlphabeta));
    _keys.push_back(KeyBase(k_bool, "do_lmrQuiesce"                      , &algo.do_lmrQuiesce));
    _keys.push_back(KeyBase(k_bool, "do_lmrSearch"                       , &algo.do_lmrSearch));
    _keys.push_back(KeyBase(k_bool, "do_moveCountPruning"                , &algo.do_moveCountPruning));
    _keys.push_back(KeyBase(k_bool, "do_aspirationwindow"                , &algo.do_aspirationwindow));
    _keys.push_back(KeyBase(k_bool, "do_pvsRoot"                         , &algo.do_pvsRoot));
    _keys.push_back(KeyBase(k_bool, "do_pvsAlphaBeta"                    , &algo.do_pvsAlphaBeta));
    _keys.push_back(KeyBase(k_bool, "do_lazyEvaluation"                  , &algo.do_lazyEvaluation));
    _keys.push_back(KeyBase(k_bool, "do_probCut"                         , &algo.do_probCut));
    _keys.push_back(KeyBase(k_bool, "do_iterativeDeepening"              , &algo.do_iterativeDeepening));
    _keys.push_back(KeyBase(k_bool, "do_trustedGenerator"                , &algo.do_trustedGenerator));
    _keys.push_back(KeyBase(k_bool, "forceNoPonder"                      , &algo.forceNoPonder));

    //*****************************
    // SMP config
    //*****************************
    _keys.push_back(KeyBase(k_int,  "threads"                            , &smpConfig.threads));
    _keys.push_back(KeyBase(k_bool, "shuffleThreadMoves"                 , &smpConfig.shuffleThreadMoves));
    _keys.push_back(KeyBase(k_bool, "shuffleThreadMovesAlphaBeta"        , &smpConfig.shuffleThreadMovesAlphaBeta));

    //*****************************
    // Debug config
    //*****************************
    _keys.push_back(KeyBase(k_string, "nameSuffixe"                      , &debugConfig.nameSuffixe));
    _keys.push_back(KeyBase(k_bool,   "modedebug"                        , &debugConfig.modedebug));
    _keys.push_back(KeyBase(k_bool,   "do_analysisNodeRate"              , &debugConfig.do_analysisNodeRate));
    _keys.push_back(KeyBase(k_depth,  "defaultDepth"                     , &debugConfig.defaultDepth));

}

void Definitions::ShowKeys() {
    for (size_t k = 0 ; k < _keys.size() ; ++k){
        switch (_keys[k].type) {
        case k_bool:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<bool*>(_keys[k].value);
            break;
        case k_depth:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<DepthType*>(_keys[k].value);
            break;
        case k_int:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<int*>(_keys[k].value);
            break;
        case k_score:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<ScoreType*>(_keys[k].value);
            break;
        case k_ull:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<unsigned long long*>(_keys[k].value);
            break;
        case k_float:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<float*>(_keys[k].value);
            break;
        case k_string:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<std::string*>(_keys[k].value);
            break;
        case k_table_int:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<std::vector<int>*>(_keys[k].value);
            break;
        case k_table_float:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<std::vector<float>*>(_keys[k].value);
            break;
        case k_table_score:
            LOG(logINFO) << _keys[k].key << " : " << *static_cast<std::vector<ScoreType>*>(_keys[k].value);
            break;
        default:
            LOG(logERROR) << "Bad key type";
            return;
        }
    }
}

bool Definitions::SetValue(const std::string & key, const std::string & value){
    bool keyFound = false;
    KeyBase & keyRef = _keys[0]; // CARE :: assume _keys is not empty ...
    for (size_t k = 0; k < _keys.size() && !keyFound; ++k) {
        if (key == _keys[k].key) {
           keyFound = true;
           keyRef = _keys[k];
        }
    }
    nlohmann::json o;
    std::stringstream str(value);
    switch (keyRef.type) {
    case k_bool:
    {
        bool v;
        str >> v;
        o[key] = v;
    }
        break;
    case k_depth:
    {
        /*DepthType*/ int v;
        str >> v;
        o[key] = (DepthType)v;
    }
        break;
    case k_int:
    {
        int v;
        str >> v;
        o[key] = v;
    }
        break;
    case k_score:
    {
        ScoreType v;
        str >> v;
        o[key] = v;
    }
        break;
    case k_ull:
    {
        unsigned long long v;
        str >> v;
        o[key] = v;
    }
        break;
    case k_float:
    {
        float v;
        str >> v;
        o[key] = v;
    }
        break;
    case k_string:
    {
        o[key] = value;
    }
        break;
    case k_table_int:
    {
        std::vector<int> v{std::istream_iterator<int>(str),
                           std::istream_iterator<int>()};
        o[key] = v;
    }
        break;
    case k_table_float:
    {
        std::vector<float> v{std::istream_iterator<float>(str),
                           std::istream_iterator<float>()};
        o[key] = v;
    }
        break;
    case k_table_score:
    {
        std::vector<ScoreType> v{std::istream_iterator<int>(str),
                           std::istream_iterator<int>()};
        o[key] = v;
    }
        break;
    default:
        LOG(logERROR) << "Bad key type";
        return false;
    }

    return SetValue(o);
}

bool Definitions::SetValue(const nlohmann::json & o){
    auto it = o.begin(); // assume only one object inside o !
    bool keyFound = false;
    for (size_t k = 0; k < _keys.size() && !keyFound; ++k) {
        if (it.key() == _keys[k].key) {
            keyFound = true;
            switch (_keys[k].type) {
            case k_bool:
                if ( ! it.value().is_boolean()){
                    LOG(logFATAL) << "Error reading boolean " << _keys[k].value;
                }
                *static_cast<bool*>(_keys[k].value) = (bool)it.value();
                break;
            case k_score:
                if ( ! it.value().is_number()){
                    LOG(logFATAL) << "Error reading int " << _keys[k].value;
                }
                *static_cast<ScoreType*>(_keys[k].value) = (ScoreType)it.value();
                break;
            case k_depth:
                if ( ! it.value().is_number()){
                    LOG(logFATAL) << "Error reading int " << _keys[k].value;
                }
                *static_cast<DepthType*>(_keys[k].value) = (DepthType)(int)it.value();
                break;
            case k_float:
                if ( ! it.value().is_number()){
                    LOG(logFATAL) << "Error reading float " << _keys[k].value;
                }
                *static_cast<float*>(_keys[k].value) = (float)it.value();
                break;
            case k_ull:
                if ( ! it.value().is_number()){
                    LOG(logFATAL) << "Error reading int " << _keys[k].value;
                }
                *static_cast<unsigned long long*>(_keys[k].value) = (unsigned long long)it.value();
                break;
            case k_int:
                if ( ! it.value().is_number()){
                    LOG(logFATAL) << "Error reading int " << _keys[k].value;
                }
                *static_cast<int*>(_keys[k].value) = (int)it.value();
                break;
            case k_string:
                if ( ! it.value().is_string()){
                    LOG(logFATAL) << "Error reading string " << _keys[k].value;
                }
                *static_cast<std::string*>(_keys[k].value) = it.value();
                break;
            case k_table_int:
            {
                if ( ! it.value().is_array()){
                    LOG(logFATAL) << "Error reading array " << _keys[k].value;
                }
                std::transform(it.value().cbegin(), it.value().cend(),
                    std::back_inserter(*static_cast<std::vector<unsigned long long>*>(_keys[k].value)),
                    [](const nlohmann::json & value) {
                        return (unsigned long long)value.get<double>();
                    }
                );
            }
                break;
            case k_table_score:
            {
                if ( ! it.value().is_array()){
                    LOG(logFATAL) << "Error reading array " << _keys[k].value;
                }
                std::transform(it.value().cbegin(), it.value().cend(),
                    std::back_inserter(*static_cast<std::vector<ScoreType>*>(_keys[k].value)),
                    [](const nlohmann::json & value) {
                        return (ScoreType)value.get<double>();
                    }
                );
            }
                break;
            case k_table_float:
            {
                if ( ! it.value().is_array()){
                    LOG(logFATAL) << "Error reading array " << _keys[k].value;
                }
                std::transform(it.value().cbegin(), it.value().cend(),
                    std::back_inserter(*static_cast<std::vector<float>*>(_keys[k].value)),
                    [](const nlohmann::json & value) {
                        return (float)value.get<double>();
                    }
                );
            }
                break;
            default:
                LOG(logERROR) << "Bad key type";
                return false;

            } // switch

            LOG(logINFO) << "Key overriden : " << it.key() << " to " << it.value();
            _keys[k].callBack();
            break;
        } // if ==
    } // for keys
    if ( !keyFound ){
        LOG(logWARNING) << "Weini does not know this key : " << it.key();
    }
    return keyFound;
}

bool Definitions::ReadConfig(const std::string & confFile) {

    LOG(logINFO) << "Reading config file " << confFile;

    std::ifstream str(confFile);
    if (!str.is_open()) {
        LOG(logERROR) << "Cannot open " << confFile;
        return false;
    }

    nlohmann::json json;
    str >> json;
    /*
    std::string err = picojson::get_last_error();
    if (!err.empty()) {
        LOG(logERROR) << err;
        return false;
    }
    */

    // check if the type of the value is "object"
    if (!json.is_object()) {
        LOG(logERROR) << "JSON is not an object";
        return false;
    }

    std::map<std::string, bool > keysRead;
    std::transform(_keys.cbegin(), _keys.cend(),
                   std::inserter(keysRead, keysRead.end()),
                   [](const Definitions::KeyBase & k) {return std::make_pair(k.key, false); }
                  );

    for (auto it = json.begin(); it != json.end(); ++it) {
        nlohmann::json o;
        o[it.key()] = it.value();
        if ( SetValue(o)){
            keysRead[it.key()] = true;
        }
    }

    for (auto it = keysRead.cbegin(); it != keysRead.cend(); ++it) {
        if (!it->second) {
            LOG(logINFO) << "Key " << it->first << " was not found in config file";
        }
    }

    return true;
}
