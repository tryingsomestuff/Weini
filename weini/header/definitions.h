#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"

#define MAX_SEARCH_DEPTH DepthType(64)
#define MAX_REDUCTION_DEPTH DepthType(10)
#define MAX_GAME_PLY 512
#define MAX_FEN_SIZE 128
#define BOARD_SIZE 64

//#define WITH_MEMORY_POOL
//#define WITH_MEMORY_POOL_BUILDER

//#define RANDOM_TEST

#define FILE(x) ((x)&7)
#define RANK(x) ((x)>>3)
#define SQUARE(f,r) ((f)+8*(r))

#define PARABOL_TAPERED(x) (1-(((x)*2-1)*((x)*2-1)))
#define COMPLEMENT_TAPERED(x) (1.f-(x))

template<typename T>
inline unsigned long long  TTMbToSize(unsigned long long nbSizeMb){return nbSizeMb*1024*1024/sizeof(T); }

#define QUIESCE_WITH_ONLY_CAPS ///@todo is that dangerous to forget checkmate tracking in quiesce ???

#define QUIESCE_WITHOUT_CHECK_VALIDATION

#define SEE_WITHOUT_CHECK_VALIDATION

//#define WITH_CAPTURED

#define TT_AUTO_SEARCH_CLEAR

typedef char DepthType;
typedef unsigned long long int NodeStatType;
typedef int ScoreType;

class Definitions{

public:

  //*****************************
  // Piece values
  //*****************************
  struct PieceValue {
      PieceValue();

      void Init();

      ScoreType pawnValueW;
      ScoreType knightValueW;
      ScoreType bishopValueW;
      ScoreType rookValueW;
      ScoreType queenValueW;
      ScoreType kingValueW;
      ScoreType pawnEGValueW;
      ScoreType knightEGValueW;
      ScoreType bishopEGValueW;
      ScoreType rookEGValueW;
      ScoreType queenEGValueW;
      ScoreType kingEGValueW;

      ScoreType pawnValueB;
      ScoreType knightValueB;
      ScoreType bishopValueB;
      ScoreType rookValueB;
      ScoreType queenValueB;
      ScoreType kingValueB;
      ScoreType pawnEGValueB;
      ScoreType knightEGValueB;
      ScoreType bishopEGValueB;
      ScoreType rookEGValueB;
      ScoreType queenEGValueB;
      ScoreType kingEGValueB;
  };

  static PieceValue pieceValue;

  //*****************************
  // TC config
  //*****************************
  struct TimeControl {
    TimeControl();
    unsigned long long currentMoveMs;
    unsigned long long timeControl;
    unsigned long long increment;
    unsigned long long movePerSession;
    float emergencyTimeFactor;
    float emergencyScoreMargin;
    DepthType minMoveToGo;
    DepthType maxMoveToGo;
  };

  static TimeControl timeControl;

  //*****************************
  // Score config
  //*****************************
  struct Scores {
    Scores();
    ScoreType defaultMoveScore;
    ScoreType infScore;
    ScoreType reductibleSortScore;
    ScoreType killer1SortScore;
    ScoreType killer2SortScore;
    ScoreType counterSortScore;
    ScoreType captureLastMovedBonus;
    ScoreType bookScore;
    ScoreType captureSortScore;
    ScoreType checkSortScore;
    ScoreType epSortScore;
    ScoreType promotionSortScore;
    ScoreType ttSortScore;
    ScoreType pvSortScore;
    ScoreType iidSortScore;
    ScoreType checkMateScore;
    ScoreType contempt;
    ScoreType stopFlagScore;
  };

  static Scores scores;

  //*****************************
  // Selectivity config
  //*****************************
  struct Selectivity {
    Selectivity();
    DepthType quies_max;
    ScoreType nullMoveMargin;
    ScoreType futilityMargin;
    ScoreType futilityDepthCoeff;
    ScoreType qfutilityMargin;
    ScoreType deltaMargin;
    ScoreType windowSize;
    ScoreType seeThreshold;
    float     exponentialWindowGrowth;
    ScoreType razoringMargin;
    ScoreType razoringDepthCoeff;
    ScoreType staticNullMoveMargin;
    ScoreType staticNullMoveDepthCoeff;
    ScoreType probCutMargin;
    ScoreType probCutDepthCoeff;
    ScoreType lmrtol;
    DepthType LMR_STD;
    DepthType LMR_ENDGAME;

    DepthType extensionRootMax;
    DepthType extensionAlphaBetaMax;

    DepthType iterativeDeepeningSelectivityMinDepth;
    DepthType staticNullMoveMaxDepth;
    DepthType futilityMaxDepth;
    DepthType razoringMaxDepth;
    DepthType nullMovePruningMinDepth;
    DepthType verifiedNullMoveMinDepth;
    DepthType moveCountPruningMaxDepth;
    DepthType recursiveNullMoveMinDepth;
    DepthType seePruningAlphaBetaMaxDepth;
    DepthType lmrMinDepth;
    DepthType probCutMinDepth;
    DepthType IIDMinDepth;

    unsigned long long probCutMaxMoves;

  };

  static Selectivity selectivity;

  //*****************************
  // Evaluation config
  //*****************************
  struct EvalBonus {
      EvalBonus();
      std::vector<ScoreType> adjKnight;
      std::vector<ScoreType> adjRook;

      ScoreType bishopPairBonus;
      ScoreType knightPairMalus;
      ScoreType rookPairMalus;

      float     mobilityValueDivisor;

      float     kingSafetyUnitaryBonus;
      float     kingSafetyUnitaryBonus2;
      float     kingSafetyFiancietto;
      float     kingSafetyNoFianciettoMalus;
      ScoreType kingSafetyFactor;

      ScoreType stormFactor;

      ScoreType badBishopMalus;

      ScoreType isolatedPawnMalus;
      ScoreType doublePawnMalus;
      ScoreType bonusCandidatePassedPawn;

      ScoreType rookOnOpenFile;
      ScoreType rookOnSemiOpenFile;
      ScoreType openFileNearKing;

      ScoreType spaceFactor;
      ScoreType tradeOffPieceBonusFactor;
      ScoreType tradeOffPawnBonusFactor;
      ScoreType tradeOffLimit;
      ScoreType noPawnMalus;

      ScoreType blockedBishopByPawn;
      ScoreType blockedKnight;
      ScoreType blockedKnight2;
      ScoreType blockedBishop;
      ScoreType blockedBishop2;
      ScoreType blockedBishop3;
      ScoreType returningBishopBonus;
      ScoreType blockedRookByKing;

      std::vector<ScoreType> passedPawn;

      float protectedPassedPawnFactor;
      float connectedPassedPawnFactor;
      float outsidePassedPawnFactor;
      float kingTooFarPassedPawnFactor;

      ScoreType kingInCheckMalus;

      ScoreType hasCastleBonus;
      ScoreType canCastleBonus;
      float     castleBonusMiddleGameFactor;
      DepthType castlePlyLimit;

      ScoreType connectedRooksBonus;

      float centerControlFactor;

      float kingTroppismFrontFactor;
      float kingTroppismFrontFactor2;
      float kingTroppismSameRankFactor;
      float kingTroppismBackRankFactor;
      float kingTroppismBackRankFactor2;
      float kingTroppismValueFactor;
      std::vector<float> kingTroppismAttackWeight;

      ScoreType tempoBonus;
  };

  static EvalBonus evalBonus;

  //*****************************
  // Evaluation options
  //*****************************
  struct EvalConfig {
      EvalConfig();
      bool evalWithTapered;
      bool evalWithInCheck;
      bool evalWithCastle;
      bool evalWithConnectedRooks;
      bool evalWithKingTroppism;
      bool evalWithSpace;
      bool evalWithMobility;
      bool evalWithPawnStructure;
      bool evalWithPawnStructure2;
      bool evalWithPawnStructure3;
      bool evalWithRookOnOpenFile;
      bool evalWithOpenFileNearKing;
      bool evalWithPawnShield;
      bool evalWithBadBishop;
      bool evalWithPiecePair;
      bool evalWithBlockedPiece;
      bool evalWithCenterControl;
      bool evalWithPawnStorm;
      bool evalWithTradeOffBonus;
      bool evalWithNoPawnPenalty;
      bool do_randomNoise;
  };

  static EvalConfig evalConfig;

  //*****************************
  // TT config
  //*****************************
  struct TranspositionTableConfig {
    TranspositionTableConfig();
    bool  do_transpositionTableSearch;
    bool  do_transpositionTableAlphaBeta;
    bool  do_transpositionTableSortSearch;
    bool  do_transpositionTableSortAlphaBeta;
    bool  do_transpositionTableQSort;
    bool  do_transpositionTableEval;
    bool  do_transpositionTableEvalPawn;
    bool  do_transpositionTableQuiesce;
    bool  do_ttAging;
    unsigned long long ttSize  ;
    unsigned long long ttQSize ;
    unsigned long long ttESize ;
    unsigned long long ttELSize;
    unsigned long long ttEPSize;
  };

  static TranspositionTableConfig ttConfig;

  //*****************************
  // Extension / reduction config
  //*****************************
  struct ExtensionReductionConfig {
      ExtensionReductionConfig();
      bool  do_CheckExtensionRoot;
      bool  do_CheckExtensionSearch;
      bool  do_CheckExtensionAlphaBeta;
      bool  do_EndGameExtensionSearch;
      bool  do_VeryEndGameExtensionSearch;
      bool  do_PVExtensionSearch;
      bool  do_SingleReplyExtensionSearch;
      bool  do_SingleReplyExtensionAlphaBeta;
      bool  do_SingularExtensionAlphaBeta;
      bool  do_NearPromotionExtensionSearch;
      bool  do_NearPromotionExtensionAlphaBeta;
      bool  do_ReCaptureExtensionSearch;
      bool  do_ReCaptureExtensionAlphaBeta;

      bool  do_ttmoveCaptureReduction;
  };

  static ExtensionReductionConfig extensionReductionConfig;

  //*****************************
  // Sorting config
  //*****************************
  struct SortingConfig {
      SortingConfig();
      bool  do_sortpositional    ;
      bool  do_sortenpassantbonus;
      bool  do_sortcheckbonus    ;
      bool  do_sortmvvlva        ;
      bool  do_sortsee           ;
      bool  do_qsortmvvlva       ;
      bool  do_qsortsee          ;
      bool  do_captureLastMovedBonus;
      bool  do_internaliterativeDeepening;
      bool  do_killerHeuristic   ;
      bool  do_counterHeuristic  ;
      bool  do_historyHeuristic  ;
  };

  static SortingConfig sortingConfig;

  //*****************************
  // Book config
  //*****************************
  struct BookConfig {
      BookConfig();
      bool         with_bigBook  ;
      bool         with_smallBook;
      int          bookSkip      ;
      std::string  bookFileName  ;
      int          maxBookMoves  ;
      bool         debugBook     ;
  };

  static BookConfig bookConfig;

  //*****************************
  // Algo config
  //*****************************
  struct Algorithms {
      Algorithms();
      bool  do_failsoftalphabeta ;
      bool  do_failsoftquiesce   ;
      bool  do_alphabetaminimax  ;
      bool  do_quiesceminimax    ;
      bool  do_quiesce           ;
      bool  do_futilityPruning   ;
      bool  do_qfutilityPruning  ;
      bool  do_razoring          ;
      bool  do_staticNullMove    ;
      bool  do_seePruning        ;
      bool  do_seePruningAlphaBeta;
      bool  do_deltaPruning      ;
      bool  do_nullMovePruning   ;
      bool  do_verifiedNullMove  ;
      bool  do_lmrAlphabeta      ;
      bool  do_lmrQuiesce        ;
      bool  do_lmrSearch         ;
      bool  do_moveCountPruning ;
      bool  do_aspirationwindow  ;
      bool  do_pvsRoot           ;
      bool  do_pvsAlphaBeta      ;
      bool  do_lazyEvaluation    ;
      bool  do_probCut           ;
      bool  do_trustedGenerator  ;
      bool  do_iterativeDeepening;
      bool  forceNoPonder; // pondering is worth 45elo (nearly a depth+1)
  };

  static Algorithms algo;

  //*****************************
  // SMP config
  //*****************************
  struct SMPConfig{
      SMPConfig();
      int threads;
      bool shuffleThreadMoves;
      bool shuffleThreadMovesAlphaBeta;
  };

  static SMPConfig smpConfig;

  //*****************************
  // Debug config
  //*****************************
  struct DebugConfig{
      DebugConfig();
      std::string nameSuffixe;
      bool modedebug;
      bool  do_analysisNodeRate;
      DepthType defaultDepth;
  };

  static DebugConfig debugConfig;


  static void InitKeys();
  static void ShowKeys();
  static bool ReadConfig(const std::string & confFile);

  static bool SetValue(const std::string & key, const std::string & value);
  static bool SetValue(const nlohmann::json & o);

private:
  enum KeyType : unsigned char {
    k_bool        = 0,
    k_depth       = 1,
    k_int         = 2,
    k_score       = 3,
    k_ull         = 4,
    k_float       = 5,
    k_string      = 6,
    k_table_score = 7,
    k_table_int   = 8,
    k_table_float = 9
  };

  struct KeyBase {
    template < typename T >
    KeyBase(KeyType t, const std::string & k, T * v, const std::function<void(void)> & cb = []{} ) :type(t), key(k), value((void*)v) {callBack = cb;}
    KeyType             type;
    std::string         key;
    void*               value;
    std::function<void(void)> callBack;
  };

  static std::vector<KeyBase> _keys;
};
