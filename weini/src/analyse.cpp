#include "analyse.h"
#include "search.h"
#include "position.h"
#include "moveGenerator.h"
#include "util.h"
#include "sort.h"
#include "book.h"
#include "definitions.h"
#include "ttable.h"
#include "UtilPosition.h"
#include "search.h"

#include <stdlib.h>
#include <utility>

namespace{

struct EvalTransposition{
    EvalTransposition():
        hash(0),
        score(0)
    {}

    Util::Zobrist::HashType hash;
    ScoreType score;

    static void InitTT      (EvalTransposition * &          tt,
                             unsigned long long &           ttsize);

    static void ClearTT      (EvalTransposition * &         tt,
                             unsigned long long &           ttsize);

    static bool GetTT       (Util::Zobrist::HashType        hash,
                             EvalTransposition * &          tt,
                             unsigned long long &           ttsize,
                             ScoreType &                    score,
                             Stats::AtomicCounter &         collisionCount,
                             Stats::AtomicCounter &         hitCount);

    static void InsertTT    (Util::Zobrist::HashType        hash,
                             EvalTransposition * &          tt,
                             unsigned long long int &       ttsize,
                             const ScoreType &              score);
};

EvalTransposition * _evalCache = 0;
EvalTransposition * _evalCacheLazy = 0;
EvalTransposition * _evalCachePawn = 0;

//static std::mutex _mutexCache;
//static std::mutex _mutexCacheLazy;
//static std::mutex _mutexCachePawn;

void EvalTransposition::ClearTT(EvalTransposition * & tt,
                                        unsigned long long &   ttsize) {
    if ( !tt ) return;
    /*
    for( unsigned long long int k = 0 ; k < ttsize ; ++k){
       tt[k].hash = 0;
    }
    */
    memset(tt,0,sizeof(EvalTransposition)*ttsize);
}


void EvalTransposition::InitTT(EvalTransposition * & tt,
                               unsigned long long &  ttsize) {
    if (!tt) {
        //x modulo y = (x & (y ? 1)) if y is a power of 2
        unsigned long long ttsizeNb = TTMbToSize<EvalTransposition>(ttsize);
        Util::NextPowerOf2(ttsizeNb);
        ttsize = ttsizeNb; // convert Mb ttSize to number of bucket ttSize
        LOG(logINFO) << "Adapted ETT size " << ttsize;
        LOG(logINFO) << "Size of ETT " << int(ttsize * sizeof(EvalTransposition) / 1024. / 1024.) << "Mo";
        tt = new EvalTransposition[(int)ttsize];
    }
}

bool EvalTransposition::GetTT(Util::Zobrist::HashType        hash,
                                       EvalTransposition * & tt,
                                       unsigned long long &           ttsize,
                                       ScoreType &                    score,
                                       Stats::AtomicCounter &         collisionCount,
                                       Stats::AtomicCounter &         hitCount) {

    //InitTT(tt, ttsize);

    const EvalTransposition & t = tt[hash&(ttsize - 1)];
    if (t.hash != 0) {
        if (t.hash == hash) {
            score = t.score;
            ++hitCount;
            return true;
        }
        else {
            ++collisionCount;
            return false;
        }
    }
    return false;
}

void EvalTransposition::InsertTT(Util::Zobrist::HashType                 hash,
                                          EvalTransposition * &          tt,
                                          unsigned long long int &       ttsize,
                                          const ScoreType &              score){
    //InitTT(tt, ttsize);

    EvalTransposition & t = tt[hash&(ttsize - 1)];
    t.hash = hash;
    t.score = score;
}


} // anonymous namespace

///@todo square controlled by a pawn (in mobility) shall not be counted ?
///@todo knight outpost !

bool Analyse::_display = false;

void Analyse::InitTTE() {
    LOG(logINFO) << "Init TTE";
    EvalTransposition::InitTT(_evalCache    , Definitions::ttConfig.ttESize);
}

void Analyse::InitTTL() {
    LOG(logINFO) << "Init TTEL";
    EvalTransposition::InitTT(_evalCacheLazy, Definitions::ttConfig.ttELSize);
}

void Analyse::InitTTP() {
    LOG(logINFO) << "Init TTEP";
    EvalTransposition::InitTT(_evalCachePawn, Definitions::ttConfig.ttEPSize);
}

bool Analyse::GetTT(Util::Zobrist::HashType hash, ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCache);
    return EvalTransposition::GetTT(hash,_evalCache,Definitions::ttConfig.ttESize,score,Stats::ttEvalCollision,Stats::ttEvalHits);
}

void Analyse::InsertTT(Util::Zobrist::HashType hash, const ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCache);
    EvalTransposition::InsertTT(hash,_evalCache,Definitions::ttConfig.ttESize,score);
}

void Analyse::ClearTT() {
    //std::lock_guard<std::mutex> lock(_mutexCache);
    ++Stats::ttEClear;
    EvalTransposition::ClearTT(_evalCache,Definitions::ttConfig.ttESize);
}

bool Analyse::GetTTLazy(Util::Zobrist::HashType hash, ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCacheLazy);
    return EvalTransposition::GetTT(hash,_evalCacheLazy,Definitions::ttConfig.ttELSize,score,Stats::ttEvalLazyCollision,Stats::ttEvalLazyHits);
}

void Analyse::InsertTTLazy(Util::Zobrist::HashType hash, const ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCacheLazy);
    EvalTransposition::InsertTT(hash,_evalCacheLazy,Definitions::ttConfig.ttELSize,score);
}

void Analyse::ClearTTL() {
    //std::lock_guard<std::mutex> lock(_mutexCache);
    ++Stats::ttELClear;
    EvalTransposition::ClearTT(_evalCacheLazy,Definitions::ttConfig.ttELSize);
}

bool Analyse::GetTTPawn(Util::Zobrist::HashType hash, ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCachePawn);
    return EvalTransposition::GetTT(hash,_evalCachePawn,Definitions::ttConfig.ttEPSize,score,Stats::ttEvalPawnCollision,Stats::ttEvalPawnHits);
}

void Analyse::InsertTTPawn(Util::Zobrist::HashType hash, const ScoreType & score) {
    //std::lock_guard<std::mutex> lock(_mutexCachePawn);
    EvalTransposition::InsertTT(hash,_evalCachePawn,Definitions::ttConfig.ttEPSize,score);
}

void Analyse::ClearTTP() {
    //std::lock_guard<std::mutex> lock(_mutexCache);
    ++Stats::ttEPClear;
    EvalTransposition::ClearTT(_evalCachePawn,Definitions::ttConfig.ttEPSize);
}

void Analyse::ActivateAnalysisDisplay(bool b){
  _display = b;
}

ScoreType Analyse::RunLazy(Position & p) {

  ++Stats::analysedlazynodes;

  if ( Definitions::ttConfig.do_transpositionTableEval ){
    ScoreType ttscore = 0;
    if ( GetTTLazy(p.GetZHash(),ttscore)){
      return ttscore;
    }
  }

  ScoreType ret = EvalMaterial(p,1.f/*dummy*/);   // just material (im)balance

  if ( Definitions::ttConfig.do_transpositionTableEval ){
    InsertTTLazy(p.GetZHash(),ret);
  }

  return ret;
}

ScoreType Analyse::RunNoMaterial(Position & p, ScoreType materialEval, float taperedCoeff) {

  ///@todo in case of lazy eval, TT and node count shall be done here !
  if ( Definitions::algo.do_lazyEvaluation ){
    ++Stats::analysednodes;
  }

#ifdef RANDOM_TEST
  return 2*(rand()*1.f/RAND_MAX-0.5f) * 330;
#endif

  // force debug
  //_display = true;

  if ( ! _display ){
    ScoreType ret = materialEval;
    ret += 0
        + (p.WhiteToPlay() ? +1 :-1) * Definitions::evalBonus.tempoBonus                                    // tempo
        + (Definitions::evalConfig.evalWithInCheck            ? EvalInCheck       (p, taperedCoeff)  : 0)   // don't like to be in check (may loos a tempo)
        + (Definitions::evalConfig.evalWithPiecePair          ? EvalPiecePair     (p, taperedCoeff)  : 0)   // piece pair
        + (Definitions::evalConfig.evalWithMobility           ? EvalMobility      (p, taperedCoeff)  : 0)   // mobility // slow
        + (Definitions::evalConfig.evalWithPawnShield         ? EvalPawnShield    (p, taperedCoeff)  : 0)   // pawn shield (seems redoudant with position)
        + (Definitions::evalConfig.evalWithCastle             ? EvalCastle        (p, taperedCoeff)  : 0)   // like castling (seems redoundant with position)
        + (Definitions::evalConfig.evalWithConnectedRooks     ? EvalConnectedRooks(p, taperedCoeff)  : 0)   // rooks connected (seems redoundant with position ??)
        + (Definitions::evalConfig.evalWithCenterControl      ? EvalCenterControl (p, taperedCoeff)  : 0)   // center control (seems a little redoundant with position and slow)
        + (Definitions::evalConfig.evalWithKingTroppism       ? EvalKingTroppism  (p, taperedCoeff)  : 0)   // king troppism // slow ?
        + (Definitions::evalConfig.evalWithBlockedPiece       ? EvalBlockedPiece  (p, taperedCoeff)  : 0)   // blocked piece
        + EvalOptimDriver(p, taperedCoeff, false)                                                           // pawn structure, open file, isolated pawn, bad bishop and global space
        + (ScoreType)(Definitions::evalConfig.do_randomNoise?(2*(rand()*1.f/RAND_MAX-0.5f)*(p.IsEndGame()?2.f:5.f)):0.f) // random noise
        ;
    return ret;
  }
  else{

    LOG(logINFO) << "tapered coeff             " << taperedCoeff;

    ScoreType tmp = materialEval;
    LOG(logINFO) << "material                  " << tmp;
    ScoreType ret = materialEval;

    tmp = EvalInCheck(p, taperedCoeff);
    LOG(logINFO) << "don't like to be in check " << tmp;
    ret += tmp;

    tmp = EvalPiecePair(p, taperedCoeff);
    LOG(logINFO) << "Piece pair                " << tmp;
    ret += tmp;

    tmp = EvalMobility(p, taperedCoeff);
    LOG(logINFO) << "mobility                  " << tmp;
    ret += tmp;

    tmp = EvalPawnShield(p, taperedCoeff);
    LOG(logINFO) << "pawn shield               " << tmp;
    ret += tmp;

    tmp = EvalCastle(p, taperedCoeff);
    LOG(logINFO) << "like castling             " << tmp;
    ret += tmp;

    tmp = EvalConnectedRooks(p, taperedCoeff);
    LOG(logINFO) << "connected rooks           " << tmp;
    ret += tmp;

    tmp = EvalCenterControl(p, taperedCoeff);
    LOG(logINFO) << "center control            " << tmp;
    ret += tmp;

    tmp = EvalKingTroppism(p, taperedCoeff);
    LOG(logINFO) << "king troppism             " << tmp;
    ret += tmp;

    tmp = EvalBlockedPiece(p, taperedCoeff);
    LOG(logINFO) << "blocked piece             " << tmp;
    ret += tmp;

    tmp = EvalOptimDriver(p, taperedCoeff, true);
    LOG(logINFO) << "all other                 " << tmp;
    ret += tmp;

    tmp = (ScoreType)(Definitions::evalConfig.do_randomNoise?(2*(rand()*1.f/RAND_MAX-0.5f)*(p.IsEndGame()?2.f:5.f)):0.f);
    LOG(logINFO) << "random noise              " << tmp;
    ret += tmp;

    return ret;
  }
}

ScoreType Analyse::Run(Position & p, float taperedCoeff) {

  ++Stats::analysednodes;

  if ( Definitions::ttConfig.do_transpositionTableEval ){
    ScoreType ttscore = 0;
    if ( GetTT(p.GetZHash(),ttscore)){
      return ttscore;
    }
  }

  ScoreType ret = RunNoMaterial(p,
                                EvalMaterial(p,1.f/*dummy*/,Definitions::evalConfig.evalWithTradeOffBonus),
                                taperedCoeff);

  if ( Definitions::ttConfig.do_transpositionTableEval ){
    InsertTT(p.GetZHash(),ret);
  }

  return ret;
}

//PSQT (from stockfish 9)
namespace {
// Middle game positions
ScoreType pawnPosition[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,
   -2,   11,   -4,   -2,   -2,   -4,   11,   -2,
   -3,   -4,   -3,   -1,   -1,   -3,   -4,   -3,
   -3,    2,    1,   12,   12,    1,    2,   -3,
   -9,   -5,   11,   20,   20,   11,   -5,   -9,
  -10,   -1,   11,   14,   14,   11,   -1,  -10,
   -6,    3,    4,    1,    1,    4,    3,   -6,
    0,    0,    0,    0,    0,    0,    0,    0  };

ScoreType pawnPositionRev[BOARD_SIZE];

ScoreType knightPosition[BOARD_SIZE] = {
    -114,  -39,  -24,  -16,  -16,  -24,  -39, -114,
     -36,  -11,    2,    8,    8,    2,  -11,  -36,
      -6,   21,   32,   38,   38,   32,   21,   -6,
     -15,    9,   22,   29,   29,   22,    9,  -15,
     -14,   10,   25,   27,   27,   25,   10,  -14,
     -41,  -12,    0,    5,    5,    0,  -12,  -41,
     -48,  -25,  -12,   -5,   -5,  -12,  -25,  -48,
     -94,  -56,  -46,  -42,  -42,  -46,  -56,  -94  };

ScoreType knightPositionRev[BOARD_SIZE];

ScoreType bishopPosition[BOARD_SIZE] = {
    -20,   -6,  -11,  -16,  -16,  -11,   -6,  -20,
    -13,    9,    3,   -1,   -1,    3,    9,  -13,
     -9,    9,    7,    1,    1,    7,    9,   -9,
     -6,   15,    9,    5,    5,    9,   15,   -6,
     -6,   16,   12,    5,    5,   12,   16,   -6,
     -5,   15,   12,    6,    6,   12,   15,   -5,
    -11,   11,    7,    0,    0,    7,   11,  -11,
    -25,   -7,  -14,  -19,  -19,  -14,   -7,  -25 };

ScoreType bishopPositionRev[BOARD_SIZE];

ScoreType rookPosition[BOARD_SIZE] = {
    -13,   -8,   -6,   -2,   -2,   -6,   -8,  -13,
     -7,    2,    4,    7,    7,    4,    2,   -7,
    -12,   -4,    0,    1,    1,    0,   -4,  -12,
    -12,   -4,    0,    0,    0,    0,   -4,  -12,
    -12,   -3,    0,    1,    1,    0,   -3,  -12,
    -12,   -5,   -2,    1,    1,   -2,   -5,  -12,
    -12,   -4,   -1,    0,    0,   -1,   -4,  -12,
    -14,   -9,   -9,   -5,   -5,   -9,   -9,  -14  };

ScoreType rookPositionRev[BOARD_SIZE];

ScoreType queenPosition[BOARD_SIZE] = {
    0,   -2,    0,    0,    0,    0,   -2,    0,
   -1,    4,    4,    3,    3,    4,    4,   -1,
   -1,    3,    4,    5,    5,    4,    3,   -1,
   -1,    5,    4,    4,    4,    4,    5,   -1,
    0,    4,    5,    4,    4,    5,    4,    0,
   -1,    3,    5,    5,    5,    5,    3,   -1,
   -2,    3,    5,    4,    4,    5,    3,   -2,
    0,   -2,   -1,    0,    0,   -1,   -2,    0 };

ScoreType queenPositionRev[BOARD_SIZE];

ScoreType kingPosition[BOARD_SIZE] = {
    36,   51,   27,    0,    0,   27,   51,   36,
    50,   74,   36,   11,   11,   36,   74,   50,
    69,   92,   49,   23,   23,   49,   92,   69,
    87,  103,   67,   38,   38,   67,  103,   87,
   103,  108,   86,   64,   64,   86,  108,  103,
   116,  143,  102,   64,   64,  102,  143,  116,
   154,  177,  139,  105,  105,  139,  177,  154,
   156,  187,  157,  114,  114,  157,  187,  156 };

ScoreType kingPositionRev[BOARD_SIZE];

// Now EG position

ScoreType pawnPositionEG[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,
    1,   -3,    0,    7,    7,    0,   -3,    1,
    3,   -2,    0,    1,    1,    0,   -2,    3,
    3,    3,    2,   -2,   -2,    2,    3,    3,
    1,    1,   -3,   -1,   -1,   -3,    1,    1,
   -1,   -2,    2,    1,    1,    2,   -2,   -1,
    2,   -1,    3,    0,    0,    3,   -1,    2,
    0,    0,    0,    0,    0,    0,    0,    0 };

ScoreType pawnPositionEGRev[BOARD_SIZE];

ScoreType knightPositionEG[BOARD_SIZE] = {
    -45,  -37,  -20,   -5,   -5,  -20,  -37,  -45,
    -27,  -20,  -10,    5,    5,  -10,  -20,  -27,
    -22,  -15,   -2,   11,   11,   -2,  -15,  -22,
    -19,  -10,    1,   16,   16,    1,  -10,  -19,
    -17,  -10,    2,   15,   15,    2,  -10,  -17,
    -20,  -16,   -2,   11,   11,   -2,  -16,  -20,
    -28,  -22,   -7,    3,    3,   -7,  -22,  -28,
    -43,  -34,  -19,   -5,   -5,  -19,  -34,  -43 };

ScoreType knightPositionEGRev[BOARD_SIZE];

ScoreType bishopPositionEG[BOARD_SIZE] = {
    -22,  -13,  -15,   -7,   -7,  -15,  -13,  -22,
    -14,   -4,   -5,    2,    2,   -5,   -4,  -14,
    -10,    0,    0,    5,    5,    0,    0,  -10,
    -10,   -1,   -2,    5,    5,   -2,   -1,  -10,
    -10,   -1,   -2,    6,    6,   -2,   -1,  -10,
     -9,    0,   -1,    6,    6,   -1,    0,   -9,
    -14,   -3,   -5,    1,    1,   -5,   -3,  -14,
    -24,  -12,  -15,   -7,   -7,  -15,  -12,  -24 };

ScoreType bishopPositionEGRev[BOARD_SIZE];

ScoreType rookPositionEG[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0 };

ScoreType rookPositionEGRev[BOARD_SIZE];

ScoreType queenPositionEG[BOARD_SIZE] = {
    -30,  -22,  -17,  -12,  -12,  -17,  -22,  -30,
    -22,  -12,   -8,   -2,   -2,   -8,  -12,  -22,
    -16,   -6,   -4,    1,    1,   -4,   -6,  -16,
    -11,   -2,    4,    8,    8,    4,   -2,  -11,
    -12,   -2,    3,    7,    7,    3,   -2,  -12,
    -16,   -7,   -3,    2,    2,   -3,   -7,  -16,
    -23,  -12,   -8,   -2,   -2,   -8,  -12,  -23,
    -29,  -23,  -17,  -12,  -12,  -17,  -23,  -29 };

ScoreType queenPositionEGRev[BOARD_SIZE];

ScoreType kingPositionEG[BOARD_SIZE] = {
    3,   22,   33,   37,   37,   33,   22,    3,
   20,   41,   50,   57,   57,   50,   41,   20,
   39,   64,   73,   72,   72,   73,   64,   39,
   45,   67,   83,   84,   84,   83,   67,   45,
   44,   70,   70,   74,   74,   70,   70,   44,
   34,   57,   69,   68,   68,   69,   57,   34,
   17,   38,   59,   55,   55,   59,   38,   17,
    0,   20,   31,   35,   35,   31,   20,    0 };

ScoreType kingPositionEGRev[BOARD_SIZE];

ScoreType noPiecePosition[BOARD_SIZE] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0 };

ScoreType * allPosition[Piece::t_max] = {
  noPiecePosition,
  kingPosition,
  queenPosition,
  rookPosition,
  bishopPosition,
  knightPosition,
  pawnPosition,
  noPiecePosition,
  pawnPositionRev,
  knightPositionRev,
  bishopPositionRev,
  rookPositionRev,
  queenPositionRev,
  kingPositionRev,
  noPiecePosition
};

ScoreType * allPositionEG[Piece::t_max] = {
    noPiecePosition,
    kingPositionEG,
    queenPositionEG,
    rookPositionEG,
    bishopPositionEG,
    knightPositionEG,
    pawnPositionEG,
    noPiecePosition,
    pawnPositionEGRev,
    knightPositionEGRev,
    bishopPositionEGRev,
    rookPositionEGRev,
    queenPositionEGRev,
    kingPositionEGRev,
    noPiecePosition
};

Square::RankFile distance[BOARD_SIZE][BOARD_SIZE];

} // end anonymous namespace

void Analyse::InitConstants() {
    LOG(logINFO) << "Init constant";

    for (Square::RankFile k = 0; k < BOARD_SIZE; ++k) {
        pawnPositionRev    [k] = pawnPosition    [63 - k];
        knightPositionRev  [k] = knightPosition  [63 - k];
        bishopPositionRev  [k] = bishopPosition  [63 - k];
        rookPositionRev    [k] = rookPosition    [63 - k];
        queenPositionRev   [k] = queenPosition   [63 - k];
        kingPositionRev    [k] = kingPosition    [63 - k];

        pawnPositionEGRev  [k] = pawnPositionEG  [63 - k];
        knightPositionEGRev[k] = knightPositionEG[63 - k];
        bishopPositionEGRev[k] = bishopPositionEG[63 - k];
        rookPositionEGRev  [k] = rookPositionEG  [63 - k];
        queenPositionEGRev [k] = queenPositionEG [63 - k];
        kingPositionEGRev  [k] = kingPositionEG  [63 - k];
    }

    for (Square::LightSquare i = 0; i < BOARD_SIZE; ++i) {
        const Square::RankFile file1 = Square::file(i);
        const Square::RankFile rank1 = Square::rank(i);
        for (Square::LightSquare j = 0; j < BOARD_SIZE; ++j) {
            const Square::RankFile file2 = Square::file(j);
            const Square::RankFile rank2 = Square::rank(j);
            const Square::RankFile rankDistance = std::abs(rank2 - rank1);
            const Square::RankFile fileDistance = std::abs(file2 - file1);
            distance[i][j] = std::min((char)6,std::max(rankDistance, fileDistance));
        }
    }
}

void Analyse::EvalCache::InitMaterialCache(const Position & p){

    if ( cacheHash == p.GetZHash() ) return;

    const int nbWp = EvalCache::SetEvalCache(pieceNumber.nbWp,p.NumberOf(Piece::t_Wpawn));
    const int nbWk = EvalCache::SetEvalCache(pieceNumber.nbWk,p.NumberOf(Piece::t_Wknight));
    const int nbWb = EvalCache::SetEvalCache(pieceNumber.nbWb,p.NumberOf(Piece::t_Wbishop));
    const int nbWr = EvalCache::SetEvalCache(pieceNumber.nbWr,p.NumberOf(Piece::t_Wrook));
    const int nbWq = EvalCache::SetEvalCache(pieceNumber.nbWq,p.NumberOf(Piece::t_Wqueen));
    const int nbBp = EvalCache::SetEvalCache(pieceNumber.nbBp,p.NumberOf(Piece::t_Bpawn));
    const int nbBk = EvalCache::SetEvalCache(pieceNumber.nbBk,p.NumberOf(Piece::t_Bknight));
    const int nbBb = EvalCache::SetEvalCache(pieceNumber.nbBb,p.NumberOf(Piece::t_Bbishop));
    const int nbBr = EvalCache::SetEvalCache(pieceNumber.nbBr,p.NumberOf(Piece::t_Brook));
    const int nbBq = EvalCache::SetEvalCache(pieceNumber.nbBq,p.NumberOf(Piece::t_Bqueen));

    EvalCache::SetEvalCache(pieceNumber.nbW,nbWk + nbWb + nbWr + nbWq);
    EvalCache::SetEvalCache(pieceNumber.nbB,nbBk + nbBb + nbBr + nbBq);

    EvalCache::SetEvalCache(pieceNumber.nbWm,nbWk + nbWb);
    EvalCache::SetEvalCache(pieceNumber.nbBm,nbBk + nbBb);

    EvalCache::SetEvalCache(kingEval.wks,p.WhiteKingSquare());
    EvalCache::SetEvalCache(kingEval.bks,p.BlackKingSquare());

    cacheHash = p.GetZHash();
}

// material imbalance
ScoreType Analyse::EvalMaterial(const Position & p, float taperedCoeff, bool doTradeOffBonus) {

  _cache.InitMaterialCache(p); // only if not yet initialized

  // just material
  ScoreType count = Util::Count(p,taperedCoeff);

  const int nbWp = _cache.pieceNumber.nbWp;
  const int nbWk = _cache.pieceNumber.nbWk;
  const int nbWb = _cache.pieceNumber.nbWb;
  const int nbWr = _cache.pieceNumber.nbWr;
  const int nbWq = _cache.pieceNumber.nbWq;
  const int nbBp = _cache.pieceNumber.nbBp;
  const int nbBk = _cache.pieceNumber.nbBk;
  const int nbBb = _cache.pieceNumber.nbBb;
  const int nbBr = _cache.pieceNumber.nbBr;
  const int nbBq = _cache.pieceNumber.nbBq;

  const int nbW  = _cache.pieceNumber.nbW;
  const int nbB  = _cache.pieceNumber.nbB;

  const int nbWm = _cache.pieceNumber.nbWm;
  const int nbBm = _cache.pieceNumber.nbBm;

  // winning side want to exchange piece
  if (doTradeOffBonus) {
      // the side with advantage wants to trade piece and keep pawn
      // the side without wants to trade pawn and keep piece
      if ( nbWq == nbBq && nbWm == nbBm){ ///@todo is this condition necessary ???
         if (count > Definitions::evalBonus.tradeOffLimit) { // if white is ahead
            count -= nbW  * Definitions::evalBonus.tradeOffPieceBonusFactor;
            count += nbWp * Definitions::evalBonus.tradeOffPawnBonusFactor;
         }
         else if (count < -Definitions::evalBonus.tradeOffLimit) { // if black is ahead
            count += nbB  * Definitions::evalBonus.tradeOffPieceBonusFactor;
            count -= nbBp * Definitions::evalBonus.tradeOffPawnBonusFactor;
         }
      }

      if (count > Definitions::evalBonus.tradeOffLimit) { // if white is ahead
         if ( nbW < nbB ){ // ahead but with less piece
             count /= 3;
         }
      }
      else if (count < -Definitions::evalBonus.tradeOffLimit) { // if black is ahead
         if ( nbB < nbW ){ // ahead but with less piece
             count /= 3;
         }
      }

  }

  // penalty if no pawn in endGame.
  if (Definitions::evalConfig.evalWithNoPawnPenalty) {
      if (_cache.pieceNumber.nbWp == 0) {
          count += Definitions::evalBonus.noPawnMalus;
      }
      if (_cache.pieceNumber.nbBp == 0) {
          count -= Definitions::evalBonus.noPawnMalus;
      }
  }

  // penalties for near draws

  // KBPsKB with unlike B
  if ( nbW == 1 && nbB == 1 && nbWb == 1 && nbBb == 1 && nbWp <= 2 && nbBp <= 2 ){
     bool isWbW = p.BitBoard().IsWhiteBishopWhite();
     bool isBbW = p.BitBoard().IsBlackBishopWhite();
     if ( ! (isWbW && isBbW) ){
         count /= 2;
     }
  }

  ///@todo more !

  return count;
}

ScoreType Analyse::HelperPosition(const Position & p, Square::LightSquare k, Piece::eType type, float taperedCoeff){
  return (ScoreType) (Piece::ColorSign(type) * (( COMPLEMENT_TAPERED(taperedCoeff) * allPositionEG[TypeToInt(type)][k]) + taperedCoeff * (allPosition[TypeToInt(type)][k]) ));
}

void EvalPositionHelper(const Position & p, const Square::LightSquare k, ScoreType & c, const Piece::eType t, float taperedCoeff){
    c += Analyse::HelperPosition(p,k,t,taperedCoeff);
}

// malus if king is checked (the idea is to not lose a tempo ...)
ScoreType Analyse::EvalInCheck(const Position & p, float /*taperedCoeff*/)  {
  if ( p.IsInCheck() ){
    return (p.WhiteToPlay() ? -1 : +1 ) * Definitions::evalBonus.kingInCheckMalus;
  }
  return 0;
}

// little bonus for bishop pair and other minor adjustement
ScoreType Analyse::EvalPiecePair(const Position & p, float taperedCoeff)  {

  const int nbWp = _cache.pieceNumber.nbWp;
  const int nbBp = _cache.pieceNumber.nbBp;

  ScoreType ret = 0;
  ret += _cache.pieceNumber.nbWr * Definitions::evalBonus.adjRook  [nbWp];
  ret -= _cache.pieceNumber.nbBr * Definitions::evalBonus.adjRook  [nbBp];
  ret += _cache.pieceNumber.nbWk * Definitions::evalBonus.adjKnight[nbWp];
  ret -= _cache.pieceNumber.nbBk * Definitions::evalBonus.adjKnight[nbBp];

  // bishop pair
  ret += ( (_cache.pieceNumber.nbWb > 1 ? Definitions::evalBonus.bishopPairBonus : 0)
          -(_cache.pieceNumber.nbBb > 1 ? Definitions::evalBonus.bishopPairBonus : 0) );
  // knight pair
  ret += ( (_cache.pieceNumber.nbWk > 1 ? Definitions::evalBonus.knightPairMalus : 0)
          -(_cache.pieceNumber.nbBk > 1 ? Definitions::evalBonus.knightPairMalus : 0) );
  // rook pair
  ret += ( (_cache.pieceNumber.nbWr > 1 ? Definitions::evalBonus.rookPairMalus   : 0)
          -(_cache.pieceNumber.nbBr > 1 ? Definitions::evalBonus.rookPairMalus   : 0) );

  return ret;
}

#define INDIVIDUAL_MOBILITY

// bonus for mobility
ScoreType Analyse::EvalMobility(Position & p, float taperedCoeff)  {
  std::map<Piece::eType, FastContainer<Move> > maxMoves;
  MoveGenerator(false).GeneratorPiece(p,maxMoves);

#ifndef INDIVIDUAL_MOBILITY
  int maxMobility = 0;
  for(auto it = maxMoves.begin() ; it != maxMoves.end() ; ++it){
    if ( Piece::IsPawn(it->first) ) continue;
    maxMobility += (int)(*it).second.size();
  }
#endif

#ifdef WITH_MEMORY_POOL_BUILDER
  ScopeClone<Position, _default_block_size> psc(p);
  Position & p2 = psc.Get();
#else
  Position p2(p);
#endif

  p2.SwitchPlayer();

  std::map<Piece::eType, FastContainer<Move> > minMoves;
  MoveGenerator(false).GeneratorPiece(p2,minMoves);

#ifndef INDIVIDUAL_MOBILITY
  int minMobility = 0;
  for (auto it = minMoves.begin(); it != minMoves.end(); ++it) {
    if ( Piece::IsPawn(it->first) ) continue;
    minMobility += (int)(*it).second.size();
  }
#endif

#ifndef INDIVIDUAL_MOBILITY
  // global mobility
  ScoreType mob = (maxMobility-minMobility) * 2;
#else
  ScoreType mob = 0;
#endif

#ifdef INDIVIDUAL_MOBILITY
  // individual mobility
  for(auto it = maxMoves.begin() ; it != maxMoves.end() ; ++it){
      if ( Piece::IsPawn(it->first) ) continue;
      if ( Piece::IsK(it->first)) continue;
      const ScoreType value = Piece::Value(it->first);
      const int pmob = (int)it->second.size();
      //std::cout << value << " " << pmob << std::endl;
      mob += (ScoreType) (value/Definitions::evalBonus.mobilityValueDivisor * pmob);
  }
  for(auto it = minMoves.begin() ; it != minMoves.end() ; ++it){
      if ( Piece::IsPawn(it->first) ) continue;
      if ( Piece::IsK(it->first)) continue;
      const ScoreType value = Piece::Value(it->first);
      const int pmob = (int)it->second.size();
      //std::cout << value << " " << pmob << std::endl;
      mob += (ScoreType)(value/Definitions::evalBonus.mobilityValueDivisor * pmob);
  }
#endif

  return mob;
}

void EvalGlobalSpaceHelper(Square::LightSquare k, ScoreType & c, const Piece::eType t ){
  if ( Piece::IsWhite(t) ){
    c+= RANK(k);
  }
  else /*if ( Piece::IsBlack(t) )*/{
    c-= 7-RANK(k);
  }
}

// king safety pawn shield
ScoreType Analyse::EvalPawnShield(const Position & p, float taperedCoeff)  {
  if ( p.IsEndGame()) return 0;

  float ret = 0;
  const Square::LightSquare wks = _cache.kingEval.wks;
  const Square::LightSquare bks = _cache.kingEval.bks;

  // white pawn shield
  if ((p.Info().white.HasCastled() || p.CurrentPly() > 16) &&
      (wks == Square::squarea1 ||
       wks == Square::squareb1 ||
       wks == Square::squarec1 )){
    if      ( p.IsWhitePawn(Square::squarea2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsWhitePawn(Square::squarea3) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus2;
    if      ( p.IsWhitePawn(Square::squareb2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if (p.IsWhitePawn(Square::squareb3) ){
        if  (p.IsWhiteBishop(Square::squareb2) || p.IsWhiteKnight(Square::squareb2) ) ret += Definitions::evalBonus.kingSafetyFiancietto; // fanchietto
        else ret += Definitions::evalBonus.kingSafetyNoFianciettoMalus; // no fanchietto
    }
    if      ( p.IsWhitePawn(Square::squarec2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsWhitePawn(Square::squarec3) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus2;
  }
  if ((p.Info().white.HasCastled() || p.CurrentPly() > 16) &&
      (wks == Square::squaref1 ||
       wks == Square::squareg1 ||
       wks == Square::squareh1 )){
    if      ( p.IsWhitePawn(Square::squaref2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsWhitePawn(Square::squaref3) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus2;
    if      ( p.IsWhitePawn(Square::squareg2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if (p.IsWhitePawn(Square::squareg3) ) {
        if  (p.IsWhiteBishop(Square::squareg2) || p.IsWhiteKnight(Square::squareg2) ) ret += Definitions::evalBonus.kingSafetyFiancietto;
        else ret += Definitions::evalBonus.kingSafetyNoFianciettoMalus;
    }
    if      ( p.IsWhitePawn(Square::squareh2) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsWhitePawn(Square::squareh3) ) ret += Definitions::evalBonus.kingSafetyUnitaryBonus2;
  }

  // black pawn shield
  if ((p.Info().black.HasCastled() || p.CurrentPly() > 16) &&
      ( bks == Square::squarea8 ||
        bks == Square::squareb8 ||
        bks == Square::squarec8 )){
    if      ( p.IsBlackPawn(Square::squarea7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsBlackPawn(Square::squarea6) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus2;
    if      ( p.IsBlackPawn(Square::squareb7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if (p.IsBlackPawn(Square::squareb6) ) {
        if  (p.IsBlackBishop(Square::squareb7) || p.IsBlackKnight(Square::squareb7) ) ret -= Definitions::evalBonus.kingSafetyFiancietto;
        else ret -= Definitions::evalBonus.kingSafetyNoFianciettoMalus;
    }
    if      ( p.IsBlackPawn(Square::squarec7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsBlackPawn(Square::squarec6) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus2;
  }
  if ((p.Info().black.HasCastled() || p.CurrentPly() > 16) &&
      ( bks == Square::squaref8 ||
        bks == Square::squareg8 ||
        bks == Square::squareh8 )){
    if      ( p.IsBlackPawn(Square::squaref7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsBlackPawn(Square::squaref6) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus2;
    if      ( p.IsBlackPawn(Square::squareg7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if (p.IsBlackPawn(Square::squareg6) ) {
        if  (p.IsBlackBishop(Square::squareg7) || p.IsBlackKnight(Square::squareg7) ) ret -= Definitions::evalBonus.kingSafetyFiancietto;
        else ret -= Definitions::evalBonus.kingSafetyNoFianciettoMalus;
    }
    if      ( p.IsBlackPawn(Square::squareh7) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus;
    else if ( p.IsBlackPawn(Square::squareh6) ) ret -= Definitions::evalBonus.kingSafetyUnitaryBonus2;
  }

  return (ScoreType)(ret * Definitions::evalBonus.kingSafetyFactor * taperedCoeff);
}

// king safety want to castle
ScoreType Analyse::EvalCastle(const Position & p, float taperedCoeff)  {
  if ( p.IsEndGame()) return 0;

  ScoreType ret = 0;

  ret += (p.Info().white.HasCastled()? Definitions::evalBonus.hasCastleBonus :0) + (p.Info().white.CanCastled()? Definitions::evalBonus.canCastleBonus :0);
  ret -= (p.Info().black.HasCastled()? Definitions::evalBonus.hasCastleBonus :0) + (p.Info().black.CanCastled()? Definitions::evalBonus.canCastleBonus :0);

  return (ScoreType)(ret * (p.CurrentPly()>Definitions::evalBonus.castlePlyLimit ? Definitions::evalBonus.castleBonusMiddleGameFactor : 1.f) * taperedCoeff);
}

// development, connected rooks
ScoreType Analyse::EvalConnectedRooks(const Position & p, float taperedCoeff)  {
  if ( p.IsEndGame()) return 0;

  ScoreType wb = 0;
  if ( _cache.pieceNumber.nbWr == 2 ){
    const Square::RankFile r = 0;
    bool firstRookFound = false;
    for(Square::RankFile k = 0 ; k < 8 ; ++k){
      if ( p.IsWhiteRook(SQUARE(k,r)) ){
        if ( firstRookFound ){
          wb = Definitions::evalBonus.connectedRooksBonus;
          break;
        }
        else{
          firstRookFound = true;
        }
      }
      else{
        if ( firstRookFound && p.IsNotEmpty(SQUARE(k, r)) ){
          break;
        }
      }
    }
  }
  ScoreType bb = 0;
  if ( _cache.pieceNumber.nbBr == 2 ){
    const Square::RankFile r = 7;
    bool firstRookFound = false;
    for(Square::RankFile k = 0 ; k < 8 ; ++k){
      if ( p.IsBlackRook(SQUARE(k, r))){
        if ( firstRookFound ){
          bb = Definitions::evalBonus.connectedRooksBonus;
          break;
        }
        else{
          firstRookFound = true;
        }
      }
      else{
        if ( firstRookFound && p.IsNotEmpty(SQUARE(k, r)) ){
          break;
        }
      }
    }
  }
  return (ScoreType)((wb - bb) * taperedCoeff);
}

// center control
ScoreType Analyse::EvalCenterControl(const Position & p, float taperedCoeff)  {
  if ( p.IsEndGame()) return 0;

  FastContainer<Square::LightSquare> threats;
  int countd4 = 0;
  int countd5 = 0;
  int counte4 = 0;
  int counte5 = 0;
  UtilMove::GetThreads(p,Square::squared4,false,&threats);
  countd4 -= (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p,Square::squaree4,false,&threats);
  counte4 -= (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p,Square::squared5,false,&threats);
  countd5 -= (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p,Square::squaree5,false,&threats);
  counte5 -= (int)threats.size();
  threats.clear();

#ifdef WITH_MEMORY_POOL_BUILDER
  ScopeClone<Position,_default_block_size> psc(p);
  Position & p2 = psc.Get();
#else
  Position p2(p);
#endif
  p2.SwitchPlayer();
  UtilMove::GetThreads(p2,Square::squared4,false,&threats);
  countd4 += (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p2,Square::squaree4,false,&threats);
  counte4 += (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p2,Square::squared5,false,&threats);
  countd5 += (int)threats.size();
  threats.clear();
  UtilMove::GetThreads(p2,Square::squaree5,false,&threats);
  counte5 += (int)threats.size();
  threats.clear();

  return (ScoreType)((p.WhiteToPlay()?+1:-1)*(countd4 + countd5 + counte4 + counte5) * Definitions::evalBonus.centerControlFactor * taperedCoeff);
}

/*
king safety : troppism (evaluate thread on square around king Position, this shall trigger attack ?)
*/
ScoreType Analyse::EvalKingTroppism(const Position & p, float taperedCoeff){

    /*
     *    b  b2 b
     *    s  s  s
     *    f  f  f
     *    f2 f  f2
     */

   static const float coeffW[][4] = { { Definitions::evalBonus.kingTroppismBackRankFactor , Definitions::evalBonus.kingTroppismSameRankFactor, Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismFrontFactor2 },
                                      { Definitions::evalBonus.kingTroppismBackRankFactor2, Definitions::evalBonus.kingTroppismSameRankFactor, Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismFrontFactor  },
                                      { Definitions::evalBonus.kingTroppismBackRankFactor , Definitions::evalBonus.kingTroppismSameRankFactor, Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismFrontFactor2 } };

   static const float coeffB[][4] = { { Definitions::evalBonus.kingTroppismFrontFactor2, Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismSameRankFactor,Definitions::evalBonus.kingTroppismBackRankFactor  },
                                      { Definitions::evalBonus.kingTroppismFrontFactor , Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismSameRankFactor,Definitions::evalBonus.kingTroppismBackRankFactor2 },
                                      { Definitions::evalBonus.kingTroppismFrontFactor2, Definitions::evalBonus.kingTroppismFrontFactor, Definitions::evalBonus.kingTroppismSameRankFactor,Definitions::evalBonus.kingTroppismBackRankFactor  } };

   ScoreType w = 0;
   ScoreType b = 0;

   std::set<Square::LightSquare> attackingWhite;
   std::set<Square::LightSquare> attackingBlack;

#ifdef WITH_MEMORY_POOL_BUILDER
   ScopeClone<Position,_default_block_size> psc(p);
   Position & p2 = psc.Get();
#else
   Position p2(p);
#endif

   if (!p.WhiteToPlay()) p2.SwitchPlayer();

   const Square::LightSquare wks = p2.WhiteKingSquare();
   FastContainer<Square::LightSquare> threats;
   for(int f = -1 ; f <= 1 ; ++f){
       for(int r = -1 ; r <= 2 ; ++r){
#ifdef USE_Incr120
           if (IsOffTheBoard(wks, f + 10 * r)) continue;
#else
           if (IsOffTheBoard(wks, f + 16 * r)) continue;
#endif
           const Square::LightSquare s = (Square::LightSquare)(wks + f+8*r);
           threats.clear();
           UtilMove::GetThreads(p2,s,false,&threats);

           /*
           std::cout << "Found threats " << threats.size() << " " << Square(s).position() << std::endl;
           for(int k = 0 ; k < threats.size() ; ++k){
              std::cout << "w thread at " << Square(s).position()
                        << " from " << Square(threats[k]).position()
                        << " " << coeffW[f+1][r+1]
                        << " " << Piece::Value((p.Get(threats[k])))
                        << " " << (9 - distance[s][threats[k]])/8.f << std::endl;
           }
           */

           attackingWhite.insert(threats.begin(),threats.end());

           if ( ! threats.empty()){
              const float coeff = Definitions::evalBonus.kingTroppismValueFactor*coeffW[f+1][r+1];

              for(size_t k = 0 ; k < threats.size() ; ++k){
                const Piece::eType & pp = p.Get(threats[k]);
                const ScoreType v = Piece::IsK(pp) ? -200 : Piece::Value(pp);
                w += (ScoreType)((9 - distance[s][threats[k]])/8.f * coeff * v / 1000.f); // value of threat is negative here !! so this is bad for white
              }
           }
       }
   }

   p2.SwitchPlayer();

   const Square::LightSquare bks = p2.BlackKingSquare();
   for(int f = -1 ; f <= 1 ; ++f){
       for(int r = -2 ; r <= 1 ; ++r){
#ifdef USE_Incr120
           if (IsOffTheBoard(bks, f+10*r)) continue;
#else
           if (IsOffTheBoard(bks, f+16*r)) continue;
#endif
           const Square::LightSquare s = (Square::LightSquare)(bks + f+8*r);
           threats.clear();
           UtilMove::GetThreads(p2,s,false,&threats);

           /*
           std::cout << "Found threats " << threats.size() << " " << Square(s).position() << std::endl;
           for(int k = 0 ; k < threats.size() ; ++k){
              std::cout << "b thread at " << Square(s).position()
                        << " from " << Square(threats[k]).position()
                        << " " << coeffB[f+1][r+2]
                        << " " << Piece::Value((p.Get(threats[k])))
                        << " " << (9 - distance[s][threats[k]])/8.f << std::endl;
           }
           */

           attackingBlack.insert(threats.begin(), threats.end());

           if ( ! threats.empty()){
              const float coeff = Definitions::evalBonus.kingTroppismValueFactor*coeffB[f+1][r+2];
              for(size_t k = 0 ; k < threats.size() ; ++k){
                 const Piece::eType & pp = p.Get(threats[k]);
                 const ScoreType v = Piece::IsK(pp) ? 200 : Piece::Value(pp);
                 b += (ScoreType)((9 - distance[s][threats[k]])/8.f * coeff * v / 1000.f); // value of threat is positive here !! so this is bad for black
              }
           }
       }
   }

   //std::cout << attackingWhite.size() << std::endl;
   //std::cout << attackingBlack.size() << std::endl;

   //std::cout << w << std::endl;
   //std::cout << b << std::endl;

   const ScoreType ww = (ScoreType)(Definitions::evalBonus.kingTroppismAttackWeight[std::min(8,(int)attackingWhite.size())] * w);
   const ScoreType bw = (ScoreType)(Definitions::evalBonus.kingTroppismAttackWeight[std::min(8,(int)attackingBlack.size())] * b);

   //std::cout << ww << std::endl;
   //std::cout << bw << std::endl;

   return (p.WhiteToPlay() ? +1 : -1) * std::min((ScoreType)(std::fabs(ww+bw)*COMPLEMENT_TAPERED(taperedCoeff)),150); // not too soon ! (early queen attack is BAD)
}

ScoreType Analyse::EvalBlockedPiece(const Position & p, float taperedCoeff){
    ScoreType ret = 0;

    // -------
    // white
    // -------

    // bishop blocked by own pawn
    if (    p.IsWhiteBishop(Square::squarec1)
         && p.IsWhitePawn(Square::squared2)
         && p.IsNotEmpty(Square::squared3) ) ret += Definitions::evalBonus.blockedBishopByPawn;
    if (    p.IsWhiteBishop(Square::squaref1)
         && p.IsWhitePawn(Square::squaree2)
         && p.IsNotEmpty(Square::squaree3) ) ret += Definitions::evalBonus.blockedBishopByPawn;

    // trapped knight
    if (    p.IsWhiteKnight(Square::squarea8)
         &&(p.IsBlackPawn(Square::squarea7)
         || p.IsBlackPawn(Square::squarec7) )) ret += Definitions::evalBonus.blockedKnight;
    if (    p.IsWhiteKnight(Square::squareh8)
         &&(p.IsBlackPawn(Square::squareh7)
         || p.IsBlackPawn(Square::squaref7) )) ret += Definitions::evalBonus.blockedKnight;
    if (    p.IsWhiteKnight(Square::squarea7)
         && p.IsBlackPawn(Square::squarea6)
         && p.IsBlackPawn(Square::squareb7) ) ret += Definitions::evalBonus.blockedKnight2;
    if (    p.IsWhiteKnight(Square::squareh7)
         && p.IsBlackPawn(Square::squareh6)
         && p.IsBlackPawn(Square::squareg7) ) ret += Definitions::evalBonus.blockedKnight2;

    // trapped bishop
    if (    p.IsWhiteBishop(Square::squarea7)
         && p.IsBlackPawn(Square::squareb6) ) ret += Definitions::evalBonus.blockedBishop;
    if (    p.IsWhiteBishop(Square::squareh7)
         && p.IsBlackPawn(Square::squareg6) ) ret += Definitions::evalBonus.blockedBishop;
    if (    p.IsWhiteBishop(Square::squareb8)
         && p.IsBlackPawn(Square::squarec7) ) ret += Definitions::evalBonus.blockedBishop2;
    if (    p.IsWhiteBishop(Square::squareg8)
         && p.IsBlackPawn(Square::squaref7) ) ret += Definitions::evalBonus.blockedBishop2;
    if (    p.IsWhiteBishop(Square::squarea6)
         && p.IsBlackPawn(Square::squareb5) ) ret += Definitions::evalBonus.blockedBishop3;
    if (    p.IsWhiteBishop(Square::squareh6)
         && p.IsBlackPawn(Square::squareg5) ) ret += Definitions::evalBonus.blockedBishop3;

    // bishop near castled king (bonus)
    if (    p.IsWhiteBishop(Square::squaref1)
         && p.IsWhiteKing(Square::squareg1) ) ret += Definitions::evalBonus.returningBishopBonus;
    if (    p.IsWhiteBishop(Square::squarec1)
         && p.IsWhiteKing(Square::squareb1) ) ret += Definitions::evalBonus.returningBishopBonus;

    // king blocking rook
    if ( (  p.IsWhiteKing(Square::squaref1)
         || p.IsWhiteKing(Square::squareg1) )
         &&(p.IsWhiteRook(Square::squareh1)
         || p.IsWhiteRook(Square::squareg1) )) ret += Definitions::evalBonus.blockedRookByKing;
    if ( (  p.IsWhiteKing(Square::squarec1)
         || p.IsWhiteKing(Square::squareb1) )
         &&(p.IsWhiteRook(Square::squarea1)
         || p.IsWhiteRook(Square::squareb1) )) ret += Definitions::evalBonus.blockedRookByKing;

    // -------
    // black
    // -------

    // bishop blocked by own pawn
    if (    p.IsBlackBishop(Square::squarec8)
         && p.IsBlackPawn(Square::squared7)
         && p.IsNotEmpty(Square::squared6) )     ret -= Definitions::evalBonus.blockedBishopByPawn;
    if (    p.IsBlackBishop(Square::squaref8)
         && p.IsBlackPawn(Square::squaree7)
         && p.IsNotEmpty(Square::squaree6) )     ret -= Definitions::evalBonus.blockedBishopByPawn;

    // trapped knight
    if (    p.IsBlackKnight(Square::squarea1)
         &&(p.IsWhitePawn(Square::squarea2)
         || p.IsWhitePawn(Square::squarec2) )) ret -= Definitions::evalBonus.blockedKnight;
    if (    p.IsBlackKnight(Square::squareh1)
         &&(p.IsWhitePawn(Square::squareh2)
         || p.IsWhitePawn(Square::squaref2) )) ret -= Definitions::evalBonus.blockedKnight;
    if (    p.IsBlackKnight(Square::squarea2)
         && p.IsWhitePawn(Square::squarea3)
         && p.IsWhitePawn(Square::squareb2) )  ret -= Definitions::evalBonus.blockedKnight2;
    if (    p.IsBlackKnight(Square::squareh2)
         && p.IsWhitePawn(Square::squareh3)
         && p.IsWhitePawn(Square::squareg2) )  ret -= Definitions::evalBonus.blockedKnight2;

    // trapped bishop
    if (    p.IsBlackBishop(Square::squarea2)
         && p.IsWhitePawn(Square::squareb3) )  ret -= Definitions::evalBonus.blockedBishop;
    if (    p.IsBlackBishop(Square::squareh2)
         && p.IsWhitePawn(Square::squareg3) )  ret -= Definitions::evalBonus.blockedBishop;
    if (    p.IsBlackBishop(Square::squareb1)
         && p.IsWhitePawn(Square::squarec2) )  ret -= Definitions::evalBonus.blockedBishop2;
    if (    p.IsBlackBishop(Square::squareg1)
         && p.IsWhitePawn(Square::squaref2) )  ret -= Definitions::evalBonus.blockedBishop2;
    if (    p.IsBlackBishop(Square::squarea3)
         && p.IsWhitePawn(Square::squareb4) )  ret -= Definitions::evalBonus.blockedBishop3;
    if (    p.IsBlackBishop(Square::squareh3)
         && p.IsWhitePawn(Square::squareg4) )  ret -= Definitions::evalBonus.blockedBishop3;

    // bishop near castled king (bonus)
    if (    p.IsBlackBishop(Square::squaref8)
         && p.IsBlackKing(Square::squareg8) )  ret -= Definitions::evalBonus.returningBishopBonus;
    if (    p.IsBlackBishop(Square::squarec8)
         && p.IsBlackKing(Square::squareb8) )  ret -= Definitions::evalBonus.returningBishopBonus;

    // king blocking rook
    if ( (  p.IsBlackKing(Square::squaref8)
         || p.IsBlackKing(Square::squareg8) )
         &&(p.IsBlackRook(Square::squareh8)
         || p.IsBlackRook(Square::squareg8) ))  ret -= Definitions::evalBonus.blockedRookByKing;
    if ( (  p.IsBlackKing(Square::squarec8)
         || p.IsBlackKing(Square::squareb8) )
         &&(p.IsBlackRook(Square::squarea8)
         || p.IsBlackRook(Square::squareb8) ))  ret -= Definitions::evalBonus.blockedRookByKing;

    return ret;

}

#define IsWhitePassed(f) wp[f] != _defaut_pawn_loc \
                         && ((f) == 0 || wpMax[f] >= bp[f-1] || bp[f-1] == _defaut_pawn_loc) \
                         && ((f) == 7 || wpMax[f] >= bp[f+1] || bp[f+1] == _defaut_pawn_loc) \
                         && (bp[f] == _defaut_pawn_loc || wpMax[f] > bp[f])

#define IsBlackPassed(f) bp[f] != _defaut_pawn_loc \
                         && ((f) == 0 || bpMax[f] <= wp[f-1] || wp[f-1] == _defaut_pawn_loc) \
                         && ((f) == 7 || bpMax[f] <= wp[f+1] || wp[f+1] == _defaut_pawn_loc) \
                         && (wp[f] == _defaut_pawn_loc || bpMax[f] < wp[f])

ScoreType Analyse::EvalOptimDriver(const Position & p, float taperedCoeff, bool display){

  ScoreType retPosition = 0;
  ScoreType retSpace = 0;
  ScoreType malusBadBishop = 0;
  ScoreType retPawnStructure1 = 0;
  bool pawnStructure1TTHit = false;

  // badBishop
  int wpws = 0;
  int wpbs = 0;
  int bpws = 0;
  int bpbs = 0;
  bool wbw = false;
  bool wbb = false;
  bool bbw = false;
  bool bbb = false;

  // pawn structure1
  if (Definitions::ttConfig.do_transpositionTableEvalPawn) {
      if (GetTTPawn(p.GetZHashPawn(), retPawnStructure1)) {
          pawnStructure1TTHit = true;
      }
  }
  static const char _defaut_pawn_loc = 10;
  char wp[8] = { _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,
      _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc };
  char bp[8] = { _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,
      _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc };
  char wpMax[8] = { _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,
      _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc };
  char bpMax[8] = { _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,
      _defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc,_defaut_pawn_loc };

  // openfile and pawn structure2
  ScoreType bonusOpenFile = 0;
  ScoreType nearKingOpenFileMalus = 0;
  ScoreType bonusCandidate = 0;
  ScoreType malusIsolatedPawn = 0;
  ScoreType malusDoublePawn = 0;
  int whitePawn[8] = { 0,0,0,0,0,0,0,0 };
  int blackPawn[8] = { 0,0,0,0,0,0,0,0 };
  int whiteRook[8] = { 0,0,0,0,0,0,0,0 };
  int blackRook[8] = { 0,0,0,0,0,0,0,0 };
  int otherWhitePiece[8] = { 0,0,0,0,0,0,0,0 };
  int otherBlackPiece[8] = { 0,0,0,0,0,0,0,0 };

  Square::RankFile wks = p.WhiteKingSquare();
  Square::RankFile bks = p.BlackKingSquare();

  Square::RankFile wkf = FILE(wks);
  Square::RankFile bkf = FILE(bks);

  const bool condPawnStructureA = (Definitions::evalConfig.evalWithPawnStructure || Definitions::evalConfig.evalWithPawnStructure3) && !pawnStructure1TTHit;
  const bool condPawnStructureB = Definitions::evalConfig.evalWithPawnStructure2 || Definitions::evalConfig.evalWithRookOnOpenFile;
  const bool condPawnStructureC = Definitions::evalConfig.evalWithPawnStructure && ! pawnStructure1TTHit;

  // first loop every square
  for(Square::RankFile rank = 0 ; rank < 8 ; ++rank){
    for (Square::RankFile file = 0 ; file < 8 ; ++file){

      const Square::RankFile k = file+8*rank;

      if ( p.IsEmpty(k)) continue;

      const Piece::eType & pp = p.Get(k);

      EvalPositionHelper(p, k, retPosition, pp, taperedCoeff); // position
      if (Definitions::evalConfig.evalWithSpace && ! p.IsPawn(k) ) EvalGlobalSpaceHelper(k,retSpace,pp);   // global space (for piece, not for pawn)

      if (condPawnStructureA) {
          if (p.IsWhitePawn(k)) {
              if (wp[file] == _defaut_pawn_loc) {
                  wp[file] = rank;
                  wpMax[file] = rank;
              }
              else {
                  wpMax[file] = rank; // "max" is higher rank
              }
          }
          if (p.IsBlackPawn(k)) {
              if (bp[file] == _defaut_pawn_loc) {
                  bp[file] = rank;
                  bpMax[file] = rank;
              }
              else {
                  bp[file] = rank; // "max" is smaller rank ...
              }
          }
      }

      if (Definitions::evalConfig.evalWithBadBishop) {
          const bool isWS = BitBoards::IsWhiteSquare(k);
          //std::cout << "Square " << (int)k << " is " << (isWS ? "white" : "black") << std::endl;
          if (Piece::IsWhitePawn(pp)) {
              if (isWS) {
                  //std::cout << "white pawn, white square" << std::endl;
                  ++wpws;
              }
              else {
                  //std::cout << "white pawn, black square" << std::endl;
                  ++wpbs;
              }
          }
          else if (Piece::IsBlackPawn(pp)) {
              if (isWS) {
                  //std::cout << "black pawn, white square" << std::endl;
                  ++bpws;
              }
              else {
                  //std::cout << "black pawn, black square" << std::endl;
                  ++bpbs;
              }
          }
          else if (pp == Piece::t_Wbishop) {
              if (isWS) {
                  //std::cout << "white bishop, white square" << std::endl;
                  wbw = true;
              }
              else {
                  //std::cout << "white bishop, black square" << std::endl;
                  wbb = true;
              }
          }
          else if (pp == Piece::t_Bbishop) {
              if (isWS) {
                  //std::cout << "black bishop, white square" << std::endl;
                  bbw = true;
              }
              else {
                  //std::cout << "black bishop, black square" << std::endl;
                  bbb = true;
              }
          }
      }

      if (condPawnStructureB) {
          switch (pp) {
          case Piece::t_Bqueen:
              blackRook[file] += 1;
              break;
          case Piece::t_Wqueen:
              whiteRook[file] += 1;
              break;
          case Piece::t_Brook:
              blackRook[file] += 1;
              break;
          case Piece::t_Wrook:
              whiteRook[file] += 1;
              break;
          case Piece::t_Bpawn:
              blackPawn[file] += 1;
              break;
          case Piece::t_Wpawn:
              whitePawn[file] += 1;
              break;
          default:
              if (Piece::IsWhite(pp)) {
                  ++otherWhitePiece[file];
              }
              else /* if ( Piece::IsBlack(pp) )*/ {
                  ++otherBlackPiece[file];
              }
          }
      }
    }
  } // end of loop over all square

  // for pawn structure bonus/malus accumulator
  ScoreType wbps = 0;
  ScoreType bbps = 0;

  // second loop over file
  for (Square::RankFile file = 0 ; file < 8 ; ++file){
      if (Definitions::evalConfig.evalWithOpenFileNearKing) {
          if (whitePawn[file] == 0 && blackPawn[file] == 0) { // open file
              if (std::abs(wkf - file) <= 1) {
                  nearKingOpenFileMalus -= Definitions::evalBonus.openFileNearKing;
              }
              if (std::abs(bkf - file) <= 1) {
                  nearKingOpenFileMalus += Definitions::evalBonus.openFileNearKing;
              }
          }
          else if (whitePawn[file] == 0 && blackPawn[file] != 0) { // semi open
              if (std::abs(wkf - file) <= 1) {
                  nearKingOpenFileMalus -= Definitions::evalBonus.openFileNearKing/2;
              }
          }
          else if (whitePawn[file] != 0 && blackPawn[file] == 0) { // semi open
              if (std::abs(bkf - file) <= 1) {
                  nearKingOpenFileMalus += Definitions::evalBonus.openFileNearKing/2;
              }
          }
      }

      if (Definitions::evalConfig.evalWithRookOnOpenFile) {
          if (whiteRook[file] != 0 || blackRook[file] != 0) {
              if (whitePawn[file] == 0 && blackPawn[file] == 0) { // open file
                  if (whiteRook[file] > blackRook[file]) {
                      bonusOpenFile += Definitions::evalBonus.rookOnOpenFile;
                  }
                  else if (whiteRook[file] < blackRook[file]) {
                      bonusOpenFile -= Definitions::evalBonus.rookOnOpenFile;
                  }
              }
              else if (whitePawn[file] == 0 && blackPawn[file] != 0) { // semi open
                  if (whiteRook[file] > blackRook[file]) {
                      bonusOpenFile += Definitions::evalBonus.rookOnSemiOpenFile;
                  }
                  else if (whiteRook[file] < blackRook[file]) {
                      bonusOpenFile -= Definitions::evalBonus.rookOnSemiOpenFile;
                  }
              }
              else if (whitePawn[file] != 0 && blackPawn[file] == 0) { // semi open
                  if (whiteRook[file] > blackRook[file]) {
                      bonusOpenFile += Definitions::evalBonus.rookOnSemiOpenFile;
                  }
                  else if (whiteRook[file] < blackRook[file]) {
                      bonusOpenFile -= Definitions::evalBonus.rookOnSemiOpenFile;
                  }
              }
          }
      } // end rook on open file

      if (condPawnStructureC) {
            float wb_loc = 0;
            // passed pawn
            if (IsWhitePassed(file)) {
                wb_loc += Definitions::evalBonus.passedPawn[wp[file]];
                // protected
                if ((file != 0 && p.IsWhitePawn(SQUARE(file - 1, wp[file] - 1)))
                 || (file != 7 && p.IsWhitePawn(SQUARE(file + 1, wp[file] - 1)))) {
                    wb_loc *= Definitions::evalBonus.protectedPassedPawnFactor;
                }
                // connected
                if ((file != 0 && IsWhitePassed(file-1))
                 || (file != 7 && IsWhitePassed(file+1)) ) {
                    wb_loc *= Definitions::evalBonus.connectedPassedPawnFactor;
                }
                // outside passed
                if ((file == 0 || file == 1)
                    && bp[0] == _defaut_pawn_loc
                    && bp[1] == _defaut_pawn_loc
                    && bp[2] == _defaut_pawn_loc
                    && bp[3] == _defaut_pawn_loc) {
                    wb_loc *= Definitions::evalBonus.outsidePassedPawnFactor;
                }
                if ((file == 6 || file == 7)
                    && bp[7] == _defaut_pawn_loc
                    && bp[6] == _defaut_pawn_loc
                    && bp[5] == _defaut_pawn_loc
                    && bp[4] == _defaut_pawn_loc) {
                    wb_loc *= Definitions::evalBonus.outsidePassedPawnFactor;
                }
                // king too far
                if (p.IsEndGame() &&
                    7 - wp[file] // distance to promotion
                    - std::max(
                        std::abs(FILE(p.BlackKingSquare()) - file),    // distance to king (file)
                        std::abs(RANK(p.BlackKingSquare()) - wp[file])  // distance to king (rank)
                    ) < 0) {
                    ///@todo (in case of recapture, this is wrong  6k1/2r1P1p1/7p/1B1pP2b/5P2/P7/1PP3R1/2K5 w - - 1 42            wb_loc+=500;
                }
            }
            //std::cout << " w " << file << " " << wb_loc << std::endl;
            wbps += (ScoreType)wb_loc;

            float bb_loc = 0;
            // passed pawn
            if (IsBlackPassed(file)) {
                bb_loc += Definitions::evalBonus.passedPawn[7-bp[file]];
                // protected
                if ((file != 0 && p.IsBlackPawn(SQUARE(file - 1, bp[file] + 1)))
                 || (file != 7 && p.IsBlackPawn(SQUARE(file + 1, bp[file] + 1)))) {
                    bb_loc *= Definitions::evalBonus.protectedPassedPawnFactor;
                }
                // connected
                if ((file != 0 && IsBlackPassed(file-1))
                 || (file != 7 && IsBlackPassed(file+1))) {
                    bb_loc *= Definitions::evalBonus.connectedPassedPawnFactor;
                }
                // outside passed
                if ((file == 0 || file == 1)
                    && wp[0] == _defaut_pawn_loc
                    && wp[1] == _defaut_pawn_loc
                    && wp[2] == _defaut_pawn_loc
                    && wp[3] == _defaut_pawn_loc) {
                    bb_loc *= Definitions::evalBonus.outsidePassedPawnFactor;
                }
                if ((file == 6 || file == 7)
                    && wp[7] == _defaut_pawn_loc
                    && wp[6] == _defaut_pawn_loc
                    && wp[5] == _defaut_pawn_loc
                    && wp[4] == _defaut_pawn_loc) {
                    bb_loc *= Definitions::evalBonus.outsidePassedPawnFactor;
                }
                // king too far
                if (p.IsEndGame() &&
                    7 - bp[file] // distance to promotion
                    - std::max(
                        std::abs(FILE(p.WhiteKingSquare()) - file),    // distance to king (file)
                        std::abs(RANK(p.WhiteKingSquare()) - bp[file])  // distance to king (rank)
                    ) < 0) {
                    ///@todo in case of recapture this is wrong              bb+=500;
                }
            }
            //std::cout << " b " << file << " " << bb_loc << std::endl;
            bbps += (ScoreType)bb_loc;

      } // end pawn structure 1

      if (Definitions::evalConfig.evalWithPawnStructure2) {
          // white doubled pawn
          if (whitePawn[file] > 1) {
              malusDoublePawn += Definitions::evalBonus.doublePawnMalus;
          }
          // black doubled pawn
          if (blackPawn[file] > 1) {
              malusDoublePawn -= Definitions::evalBonus.doublePawnMalus;
          }
          // Isolated pawn
          if (file > 0 && file < 7) {
              if (whitePawn[file] >= 1 && whitePawn[file - 1] == 0 && whitePawn[file + 1] == 0) {
                  malusIsolatedPawn += Definitions::evalBonus.isolatedPawnMalus;
              }
              if (blackPawn[file] >= 1 && blackPawn[file - 1] == 0 && blackPawn[file + 1] == 0) {
                  malusIsolatedPawn -= Definitions::evalBonus.isolatedPawnMalus;
              }
          }
      } // end pawn structure 2

      if (Definitions::evalConfig.evalWithPawnStructure3) {
          //candidate passed pawn white
          if (wp[file] != _defaut_pawn_loc && (bp[file] == _defaut_pawn_loc || bp[file] < wpMax[file] )) { // no black pawn ahead

              /*
              std::cout << "Candidate white " << (int)file << " " << (int)wp[file] << " " << (int)wpMax[file] << " " << (int)bp[file] << " " << (int)bpMax[file] << std::endl;
              if (file != 0 ) std::cout << "w      file-1    " << (int)wp[file-1] << std::endl;
              if (file != 7 ) std::cout << "w      file+1    " << (int)wp[file+1] << std::endl;
              if (file != 0 ) std::cout << "wm     file-1    " << (int)wpMax[file-1] << std::endl;
              if (file != 7 ) std::cout << "wm     file+1    " << (int)wpMax[file+1] << std::endl;
              if (file != 0 ) std::cout << "b      file-1    " << (int)bp[file-1] << std::endl;
              if (file != 7 ) std::cout << "b      file+1    " << (int)bp[file+1] << std::endl;
              if (file != 0 ) std::cout << "bm     file-1    " << (int)bpMax[file-1] << std::endl;
              if (file != 7 ) std::cout << "bm     file+1    " << (int)bpMax[file+1] << std::endl;
              */

              int nbSupport = 0;
              int nbGuard   = 0;
              if (file != 0 &&                                wp[file-1]    != _defaut_pawn_loc && wp[file-1]    <= wpMax[file]) ++nbSupport;
              if (file != 7 &&                                wp[file+1]    != _defaut_pawn_loc && wp[file+1]    <= wpMax[file]) ++nbSupport;
              if (file != 0 && wpMax[file-1] != wp[file-1] && wpMax[file-1] != _defaut_pawn_loc && wpMax[file-1] <= wpMax[file]) ++nbSupport;
              if (file != 7 && wpMax[file+1] != wp[file+1] && wpMax[file+1] != _defaut_pawn_loc && wpMax[file+1] <= wpMax[file]) ++nbSupport;

              if (file != 0 &&                                bpMax[file-1] != _defaut_pawn_loc && bpMax[file-1] > wpMax[file]) ++nbGuard;
              if (file != 7 &&                                bpMax[file+1] != _defaut_pawn_loc && bpMax[file+1] > wpMax[file]) ++nbGuard;
              if (file != 0 && bp[file-1] != bpMax[file-1] && bp[file-1]    != _defaut_pawn_loc && bp[file-1]    > wpMax[file]) ++nbGuard;
              if (file != 7 && bp[file+1] != bpMax[file+1] && bp[file+1]    != _defaut_pawn_loc && bp[file+1]    > wpMax[file]) ++nbGuard;

              //std::cout << nbSupport << " " << nbGuard << std::endl;

              if (nbGuard > 0 && nbSupport >= nbGuard) {
                  bonusCandidate += Definitions::evalBonus.bonusCandidatePassedPawn;
              }
          }
          //candidate passed pawn black
          if (bp[file] != _defaut_pawn_loc && (wp[file] == _defaut_pawn_loc || wp[file] > bpMax[file] )) { // no white pawn ahead

              /*
              std::cout << "Candidate black " << (int)file << " " << (int)wp[file] << " " << (int)wpMax[file] << " " << (int)bp[file] << " " << (int)bpMax[file] << std::endl;
              if (file != 0 ) std::cout << "w      file-1    " << (int)wp[file-1] << std::endl;
              if (file != 7 ) std::cout << "w      file+1    " << (int)wp[file+1] << std::endl;
              if (file != 0 ) std::cout << "wm     file-1    " << (int)wpMax[file-1] << std::endl;
              if (file != 7 ) std::cout << "wm     file+1    " << (int)wpMax[file+1] << std::endl;
              if (file != 0 ) std::cout << "b      file-1    " << (int)bp[file-1] << std::endl;
              if (file != 7 ) std::cout << "b      file+1    " << (int)bp[file+1] << std::endl;
              if (file != 0 ) std::cout << "bm     file-1    " << (int)bpMax[file-1] << std::endl;
              if (file != 7 ) std::cout << "bm     file+1    " << (int)bpMax[file+1] << std::endl;
              */

              int nbSupport = 0;
              int nbGuard = 0;
              if (file != 0 &&                                    bp[file - 1]    != _defaut_pawn_loc && bp[file-1]    >= bpMax[file]) ++nbSupport;
              if (file != 7 &&                                    bp[file + 1]    != _defaut_pawn_loc && bp[file+1]    >= bpMax[file]) ++nbSupport;
              if (file != 0 && bpMax[file - 1] != bp[file - 1] && bpMax[file - 1] != _defaut_pawn_loc && bpMax[file-1] >= bpMax[file]) ++nbSupport;
              if (file != 7 && bpMax[file + 1] != bp[file + 1] && bpMax[file + 1] != _defaut_pawn_loc && bpMax[file+1] >= bpMax[file]) ++nbSupport;

              if (file != 0 &&                                    wpMax[file - 1] != _defaut_pawn_loc && wpMax[file-1] < bpMax[file]) ++nbGuard;
              if (file != 7 &&                                    wpMax[file + 1] != _defaut_pawn_loc && wpMax[file+1] < bpMax[file]) ++nbGuard;
              if (file != 0 && wp[file - 1] != wpMax[file - 1] && wp[file - 1]    != _defaut_pawn_loc && wp[file-1]    < bpMax[file]) ++nbGuard;
              if (file != 7 && wp[file + 1] != wpMax[file + 1] && wp[file + 1]    != _defaut_pawn_loc && wp[file+1]    < bpMax[file]) ++nbGuard;

              //std::cout << nbSupport << " " << nbGuard << std::endl;

              if (nbGuard > 0 && nbSupport >= nbGuard) {
                  bonusCandidate -= Definitions::evalBonus.bonusCandidatePassedPawn;
              }
          }
      }

  } // end loop on file

  if (Definitions::evalConfig.evalWithBadBishop) {
      // bad bishop

      //std::cout << "wpws " << wpws << " wpbs " << wpbs << std::endl;
      //std::cout << "bpws " << bpws << " bpbs " << bpbs << std::endl;
      //std::cout << "wbw  " << wbw  << " wbb  " << wbb  << std::endl;
      //std::cout << "bbw  " << bbw  << " bbb  " << bbb  << std::endl;

      if (wpws > wpbs && wpws > 3 && wbw) malusBadBishop += Definitions::evalBonus.badBishopMalus;
      if (wpws < wpbs && wpbs > 3 && wbb) malusBadBishop += Definitions::evalBonus.badBishopMalus;
      if (bpws > bpbs && bpws > 3 && bbw) malusBadBishop -= Definitions::evalBonus.badBishopMalus;
      if (bpws < bpbs && bpbs > 3 && bbb) malusBadBishop -= Definitions::evalBonus.badBishopMalus;
  }

  // pawn storm
  ScoreType stormScore = 0;
  if ( !p.IsEndGame() && Definitions::evalConfig.evalWithPawnStorm) {
      const Square::LightSquare wks = p.WhiteKingSquare();
      const Square::RankFile wkr = Square::rank(wks);
      const Square::RankFile wkf = Square::file(wks);
      ScoreType wMalus = 0; // pawn storm against white
      if (wkr < 2 && wkf != 4 && _cache.pieceNumber.nbBq != 0 )  {
          for (int k = std::max(0, wkf - 2); k <= std::min(7, wkf + 2); ++k) {
              if (bp[k] != _defaut_pawn_loc) {
                  wMalus -= 7-bp[k];
              }
          }
      }
      const Square::LightSquare bks = p.BlackKingSquare();
      const Square::RankFile bkr = Square::rank(bks);
      const Square::RankFile bkf = Square::file(bks);
      ScoreType bMalus = 0; // pawn storm against black
      if (bkr > 5 && bkf != 4 && _cache.pieceNumber.nbWq != 0 ) {
          for (int k = std::max(0, bkf - 2); k <= std::min(7, bkf + 2); ++k) {
              if (wp[k] != _defaut_pawn_loc) {
                  bMalus += wp[k];
              }
          }
      }
      stormScore = (ScoreType) ((wMalus + bMalus) * Definitions::evalBonus.stormFactor * taperedCoeff);
  }

  if (Definitions::evalConfig.evalWithPawnStructure && !pawnStructure1TTHit) {
      retPawnStructure1 = wbps - bbps;
      if (Definitions::ttConfig.do_transpositionTableEvalPawn) {
          InsertTTPawn(p.GetZHashPawn(), retPawnStructure1);
      }
  }

  if (Definitions::evalConfig.evalWithPawnStructure ){
     // penalty for passed pawn with less material
     if ( _cache.pieceNumber.nbW != _cache.pieceNumber.nbB ){
        retPawnStructure1 /= 3;
     }
  }

  if (display) {
      LOG(logINFO) << "+ Open File               " << bonusOpenFile;
      LOG(logINFO) << "+ Open File near king     " << nearKingOpenFileMalus;
      LOG(logINFO) << "+ Double pawn             " << malusDoublePawn;
      LOG(logINFO) << "+ Isolated pawn           " << malusIsolatedPawn;
      LOG(logINFO) << "+ Space                   " << Definitions::evalBonus.spaceFactor*retSpace*PARABOL_TAPERED(taperedCoeff);
      LOG(logINFO) << "+ Positionnal             " << retPosition;
      LOG(logINFO) << "+ Bad bishop              " << malusBadBishop;
      LOG(logINFO) << "+ Pawn storm              " << stormScore;
      LOG(logINFO) << "+ Pawn (candidate)        " << bonusCandidate;
      LOG(logINFO) << "+ Pawn (passed)           " << retPawnStructure1;
      LOG(logINFO) << "   => white               " << wbps;
      LOG(logINFO) << "   => black               " << bbps;
  }

  return bonusOpenFile + nearKingOpenFileMalus +
      + malusDoublePawn + malusIsolatedPawn + bonusCandidate
      + (ScoreType)(Definitions::evalBonus.spaceFactor*retSpace*PARABOL_TAPERED(taperedCoeff))
      + retPosition + malusBadBishop + stormScore + retPawnStructure1;

}
