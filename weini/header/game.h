#pragma once

#include "definitions.h"
#include "util.h"

class Position;

//#define DEBUG_GAME_HASH

class Game{
public:
  Game();

  inline const Util::Zobrist::HashType* GameHashes()const { return _gameHashes;}

  void SetHash(const Position & p, int ply = -1);

  void Display(Util::Zobrist::HashType h = 0, DepthType currentPly = -1)const;

  bool Is3RepDraw(const Position & p, DepthType nbRep, bool displayDebug = false)const;

private:
  Util::Zobrist::HashType _gameHashes[MAX_GAME_PLY];
#ifdef DEBUG_GAME_HASH
  std::string _gameFEN[MAX_GAME_PLY];
#endif
};
