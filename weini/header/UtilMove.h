#pragma once

#include "bitboard.h"
#include "definitions.h"
#include "Piece.h"
#include "square.h"
#include "fastContainer.h"
#include "util.h"

class Position;
class Move;

namespace UtilMove{

enum eSpecialMove : char {
  sm_invalide = -2,
  sm_ep = -1,
  sm_mini = sm_invalide,
  sm_standard = 0,
  sm_Ks   = 1, // white king side castling
  sm_Qs   = 2, // white queen side castling
  sm_ks   = 3, // black king side castling
  sm_qs   = 4, // black queen side castling
  sm_castling_max = sm_qs,
  sm_pq   = 5, // promotion to queen
  sm_pr   = 6, // promotion to rook
  sm_pb   = 7, // promotion to bishop
  sm_pn   = 8  // promotion to knight
};

struct Direction{

    typedef unsigned int EncodedDirectionType;

    EncodedDirectionType _enc;

    inline Square::RankFile FileInc()const{ return (_enc >> 24) & 0xFF;}
    inline Square::RankFile RankInc()const{ return (_enc >> 16) & 0xFF;}
    inline Square::RankFile Incr()   const{ return (_enc >>  8) & 0xFF;}
    inline Square::RankFile IncrSpe()const{ return  _enc        & 0xFF;}

    static inline EncodedDirectionType PACKU(unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3) {
        return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
    }

    static inline EncodedDirectionType PACK(Square::RankFile c0, Square::RankFile c1, Square::RankFile c2, Square::RankFile c3) {
        return PACKU( (EncodedDirectionType)c0, (EncodedDirectionType)c1, (EncodedDirectionType)c2, (EncodedDirectionType)c3 );
    }
};

void DisplayList(const FastContainer<Move> & moves, const std::string & message = "");

bool GetThreadsHelperFast(const Position & p, Square::LightSquare index, bool earlyExit = true, FastContainer<Square::LightSquare> * threats = NULL);

bool GetThreadsHelperFastWhite(const Position & p, Square::LightSquare index, bool earlyExit = true, FastContainer<Square::LightSquare> *threats = NULL);
bool GetThreadsHelperFastBlack(const Position & p, Square::LightSquare index, bool earlyExit = true, FastContainer<Square::LightSquare> *threats = NULL);

#ifdef USE_Incr120
static const Direction legalRook         [4] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-10), Direction::PACK( +0, +1, +8,+10) };
static const Direction legalBishop       [4] = { Direction::PACK( -1, -1, -9,-11), Direction::PACK( -1, +1, +7, +9),
                                                 Direction::PACK( +1, -1, -7, -9), Direction::PACK( +1, +1, +9,+11) };
static const Direction legalKing         [8] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-10), Direction::PACK( +0, +1, +8,+10) ,
                                                 Direction::PACK( -1, -1, -9,-11), Direction::PACK( -1, +1, +7, +9),
                                                 Direction::PACK( +1, -1, -7, -9), Direction::PACK( +1, +1, +9,+11) };
static const Direction legalQueen        [8] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-10), Direction::PACK( +0, +1, +8,+10) ,
                                                 Direction::PACK( -1, -1, -9,-11), Direction::PACK( -1, +1, +7, +9),
                                                 Direction::PACK( +1, -1, -7, -9), Direction::PACK( +1, +1, +9,+11) };
static const Direction legalKnight       [8] = { Direction::PACK( +1, +2,+17,+21), Direction::PACK( +1, -2,-15,-19),
                                                 Direction::PACK( -1, +2,+15,+19), Direction::PACK( -1, -2,-17,-21) ,
                                                 Direction::PACK( +2, +1,+10,+12), Direction::PACK( +2, -1, -6, -8),
                                                 Direction::PACK( -2, +1, +6, +8), Direction::PACK( -2, -1,-10,-12) };
static const Direction legalWPawn        [4] = { Direction::PACK( +0, +1, +8,+10), Direction::PACK( -1, +1, +7, +9),
                                                 Direction::PACK( +1, +1, +9,+11), Direction::PACK( +0, +2,+16,+20) };
static const Direction legalBPawn        [4] = { Direction::PACK( +0, -1, -8,-10), Direction::PACK( -1, -1, -9,-11),
                                                 Direction::PACK( +1, -1, -7, -9), Direction::PACK( +0, -2,-16,-20) };
static const Direction legalWPawnStdCap  [2] = { Direction::PACK( -1, +1, +7, +9), Direction::PACK( +1, +1, +9,+11) };
static const Direction legalBPawnStdCap  [2] = { Direction::PACK( -1, -1, -9,-11), Direction::PACK( +1, -1, -7, -9) };
#else // INCR88
static const Direction legalRook         [4] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-16), Direction::PACK( +0, +1, +8,+16) };
static const Direction legalBishop       [4] = { Direction::PACK( -1, -1, -9,-17), Direction::PACK( -1, +1, +7,+15),
                                                 Direction::PACK( +1, -1, -7,-15), Direction::PACK( +1, +1, +9,+17) };
static const Direction legalKing         [8] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-16), Direction::PACK( +0, +1, +8,+16) ,
                                                 Direction::PACK( -1, -1, -9,-17), Direction::PACK( -1, +1, +7,+15),
                                                 Direction::PACK( +1, -1, -7,-15), Direction::PACK( +1, +1, +9,+17) };
static const Direction legalQueen        [8] = { Direction::PACK( -1, +0, -1, -1), Direction::PACK( +1, +0, +1, +1),
                                                 Direction::PACK( +0, -1, -8,-16), Direction::PACK( +0, +1, +8,+16) ,
                                                 Direction::PACK( -1, -1, -9,-17), Direction::PACK( -1, +1, +7,+15),
                                                 Direction::PACK( +1, -1, -7,-15), Direction::PACK( +1, +1, +9,+17) };
static const Direction legalKnight       [8] = { Direction::PACK( +1, +2,+17,+33), Direction::PACK( +1, -2,-15,-31),
                                                 Direction::PACK( -1, +2,+15,+31), Direction::PACK( -1, -2,-17,-33) ,
                                                 Direction::PACK( +2, +1,+10,+18), Direction::PACK( +2, -1, -6,-14),
                                                 Direction::PACK( -2, +1, +6,+14), Direction::PACK( -2, -1,-10,-18) };
static const Direction legalWPawn        [4] = { Direction::PACK( +0, +1, +8,+16), Direction::PACK( -1, +1, +7,+15),
                                                 Direction::PACK( +1, +1, +9,+17), Direction::PACK( +0, +2,+16,+32) };
static const Direction legalBPawn        [4] = { Direction::PACK( +0, -1, -8,-16), Direction::PACK( -1, -1, -9,-17),
                                                 Direction::PACK( +1, -1, -7,-15), Direction::PACK( +0, -2,-16,-32) };
static const Direction legalWPawnStdCap  [2] = { Direction::PACK( -1, +1, +7,+15), Direction::PACK( +1, +1, +9,+17) };
static const Direction legalBPawnStdCap  [2] = { Direction::PACK( -1, -1, -9,-17), Direction::PACK( +1, -1, -7,-15) };
#endif

const ScoreType MvvLvaScores[15][15] = {
        // must be inside [ -inf ; 500 ]
        // because captureLastMovedBonus=600
        //         killer1SortScore=4000
        //         killer2SortScore=2000
        //         captureSortScore=6000
                          // victim down        attacker ->  K, K, Q, R, B, N, P, 0, P, N, B, R, Q, K, K
        /*MvvLvaScores[TypeToInt(Piece::t_BkingFalse)] =*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Bking)]      =*/ { 0, 0, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 0, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Bqueen)]     =*/ { 0, 3, 4, 5, 6, 7, 8, 0, 8, 7, 6, 5, 4, 3, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Brook)]      =*/ { 0, 1,-5, 2, 3, 4, 5, 0, 5, 4, 3, 2,-5, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Bbishop)]    =*/ { 0, 1,-6,-2, 1, 1, 3, 0, 3, 1, 1,-2,-6, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Bknight)]    =*/ { 0, 1,-6,-2, 0, 1, 2, 0, 2, 1, 0,-2,-6, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Bpawn)]      =*/ { 0, 1,-9,-5,-3,-2, 0, 0, 0,-2,-3,-5,-9, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_empty)]      =*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wpawn)]      =*/ { 0, 1,-9,-5,-3,-2, 0, 0, 0,-2,-3,-5,-9, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wknight)]    =*/ { 0, 1,-6,-2, 0, 1, 2, 0, 2, 1, 0,-2,-6, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wbishop)]    =*/ { 0, 1,-6,-2, 1, 1, 3, 0, 3, 1, 1,-2,-6, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wrook)]      =*/ { 0, 1,-5, 2, 3, 4, 5, 0, 5, 4, 3, 2,-5, 1, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wqueen)]     =*/ { 0, 3, 4, 5, 6, 7, 8, 0, 8, 7, 6, 5, 4, 3, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_Wking)]      =*/ { 0, 0, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 0, 0},
        /*MvvLvaScores[TypeToInt(Piece::t_WkingFalse)] =*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                                       };

void InitMvvLva();

bool IsTherePieceOnBoard(const Position & p, const Piece::eType & t);

/// be carefull, threats are not necessary legal moves (pinned piece, king, ...)

bool GetThreads(const Position & p, Square::LightSquare index, bool earlyExit = true,FastContainer<Square::LightSquare> * threats = NULL);

bool IsThreatened (const Position & p, Square::LightSquare index);

bool GetChecks(const Position & p, bool earlyExit = true, FastContainer<Square::LightSquare> * threats = NULL);

bool KingIsChecked(const Position & p);


/// This one is very slow. Do not use it deep inside search tree...
bool ValidateLegal         (const Position & p, const Direction * legal, int nDir, const Square::LightSquare & from, const Square::LightSquare & to, int maxdepth = 7); // 7 means infinity

inline
bool ValidateBoardSides    (const Square::RankFile file){ return (file >= 0 ) && (file < 8 );}

bool ValidatePieceInTheWay (const Position & p, const Square::LightSquare & from, int depth, int dir, int startDepth, NSColor::eColor c);

bool ValidateMove          (const Move &m, Position & p);

bool ValidateCheckStatus   (const Move &m, Position & p);

bool ValidateIsCheck       (const Move & m, const Position & p);

bool ValidatePawn          (const Move & m, const Position & p);

bool PossibleEnPassant     (const Move & m, const Position & p);

bool ValidateEnPassant     (const Move & m, const Position & p);

bool ValidateCastling      (const Move & m, const Position & p);

bool Read                  (const NSColor::eColor & c, const std::string & s, Square::LightSquare & from, Square::LightSquare & to, UtilMove::eSpecialMove & moveType);

bool DetectStandardCapture (const Move & m, const Position &p);

void SetIsCapture          (const Move & m, bool b);

std::string GetAlgAlt(const std::string & sAlgAbrInit, Position & p);

struct SortThreatsFunctor {
    const Position & _p;
    SortThreatsFunctor(const Position & p);
    bool operator()(const Square::LightSquare & s1,const Square::LightSquare & s2);
};

struct ReverseSortThreatsFunctor {
    const Position & _p;
    ReverseSortThreatsFunctor(const Position & p);
    bool operator()(const Square::LightSquare & s1, const Square::LightSquare & s2);
};

}
