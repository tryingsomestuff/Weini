#pragma once

#include "Piece.h"
#include "move.h"
#include "color.h"
#include "thread.h"

#include <deque>

class Position;

namespace Util{

/// material imbalance ( >0 for white, <0 for black )
ScoreType Count(const Position & p, float taperedCoeff);
/// current white material >0
ScoreType CountWhite(const Position & p, float taperedCoeff);
/// current black material <0
ScoreType CountBlack(const Position & p, float taperedCoeff);

/// return the Square index where the king is now
Square::LightSquare FindKing(const Position & p, NSColor::eColor c);

/// return the vector of Square index where the piece is
std::deque<Square::LightSquare> FindPiece(const Position & p, NSColor::eColor c, Piece::eType t);

void DisplayPGN    (const std::string & fen, const std::deque<SearcherBase::SearchedMove> & game);
std::string JsonPGN(const std::string & fen, const std::deque<SearcherBase::SearchedMove> & game);

}
