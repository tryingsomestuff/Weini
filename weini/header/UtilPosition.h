#pragma once

#include "Piece.h"
#include "move.h"
#include "color.h"
#include "thread.h"

#include <deque>

class Position;

namespace Util{

/// material imbalance ( >0 for white, <0 for black )
ScoreType Count(const Position & p);
/// current white material >0
ScoreType CountWhite(const Position & p);
/// current black material <0
ScoreType CountBlack(const Position & p);

/// number of white piece without pawn
int NumberOfWhitePiece(const Position & p);
/// number of black piece without pawn
int NumberOfBlackPiece(const Position & p);

/// number of white minor piece without pawn
int NumberOfWhiteMinorPiece(const Position & p);
/// number of black minor piece without pawn
int NumberOfBlackMinorPiece(const Position & p);

/// number of white major piece without pawn
int NumberOfWhiteMajorPiece(const Position & p);
/// number of black major piece without pawn
int NumberOfBlackMajorPiece(const Position & p);

/// return the Square index where the king is now
Square::LightSquare FindKing(const Position & p, NSColor::eColor c);

/// return the vector of Square index where the piece is
std::deque<Square::LightSquare> FindPiece(const Position & p, NSColor::eColor c, Piece::eType t);

void DisplayPGN    (const std::string & fen, const std::deque<SearcherBase::SearchedMove> & game);
std::string JsonPGN(const std::string & fen, const std::deque<SearcherBase::SearchedMove> & game);

}
