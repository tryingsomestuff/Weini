#pragma once

#include "definitions.h"
#include "move.h"
#include "util.h"

class Position;

//#define DEBUG_GAME_HASH

class Game{
public:
  Game();

  inline const Util::Zobrist::HashType* GameHashes()  const { return _gameHashes;}
  inline const bool*                    GameCaptures()const { return _isCapture;}

  void SetHash(const Position & p, int ply = -1, bool moveWasCapture = false);
  void SetHash(const Util::Zobrist::HashType & h, int ply, bool moveWasCapture = false);

  void Display(Util::Zobrist::HashType h = 0, DepthType currentPly = -1)const;

  bool Is3RepDraw(const Position & p, DepthType nbRep, bool displayDebug = false)const;

  struct AdditionalPositionInfo {

      MiniMove            lastMove;          // the move that leads to this position
      Piece::eType        lastCapturedPiece; // the piece eventually taken during this move

      AdditionalPositionInfo();
      AdditionalPositionInfo(const AdditionalPositionInfo & i);
      AdditionalPositionInfo(const AdditionalPositionInfo && i);
      void operator=(const AdditionalPositionInfo & i);

      inline bool lastMoveWasCapture()const { return lastCapturedPiece != Piece::t_empty;}

  };

  const AdditionalPositionInfo & Info(PlyType ply)const;

  AdditionalPositionInfo & InfoRef(PlyType ply);

private:
  Util::Zobrist::HashType _gameHashes[MAX_GAME_PLY];
  bool _isCapture[MAX_GAME_PLY];

  mutable AdditionalPositionInfo _info[MAX_GAME_PLY];

#ifdef DEBUG_GAME_HASH
  std::string _gameFEN[MAX_GAME_PLY];
#endif
};



inline
Game::AdditionalPositionInfo::AdditionalPositionInfo() :
    lastMove({ -1,-1,UtilMove::sm_invalide }),
    lastCapturedPiece(Piece::t_empty)
{

}

inline
Game::AdditionalPositionInfo::AdditionalPositionInfo(const AdditionalPositionInfo & i) :
    lastMove(i.lastMove),
    lastCapturedPiece(i.lastCapturedPiece)
{

}

inline
Game::AdditionalPositionInfo::AdditionalPositionInfo(const AdditionalPositionInfo && i) :
    lastMove(i.lastMove),
    lastCapturedPiece(i.lastCapturedPiece)
{

}

inline
void Game::AdditionalPositionInfo::operator=(const AdditionalPositionInfo & i) {
    lastMove          = i.lastMove;
    lastCapturedPiece = i.lastCapturedPiece;
}


inline
const Game::AdditionalPositionInfo & Game::Info(PlyType ply)const {
    return _info[ply];
}

inline
Game::AdditionalPositionInfo & Game::InfoRef(PlyType ply) {
    return _info[ply];
}
