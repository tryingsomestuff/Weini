#pragma once

#include <vector>
#include <deque>
#include <cstring>

#include "Piece.h"
#include "logger.hpp"

#define MAX_CAPTURED_PIECE 32

class Player {
public:

  enum eCastlingRight : unsigned char {
    cr_none = 0,
    cr_ks   = 1,
    cr_qs   = 2,
    cr_both = 3
  };

  inline const NSColor::eColor & Color()const { return _c; }
  inline const eCastlingRight & CastlingRight()const { return _cr; }

  void AddCastlingRight   (const eCastlingRight & cr);
  void RemoveCastlingRight(const eCastlingRight & cr);

#ifdef WITH_CAPTURED
  Piece::eType captured[MAX_CAPTURED_PIECE];
  unsigned char capturedN;
#endif

  inline
  void SetCastled(bool b){_hasCastled = b;}
  inline
  bool HasCastled()const{ return _hasCastled;}
  inline
  bool CanCastled()const{ return CastlingRight() != cr_none;}

protected:
  explicit Player(const NSColor::eColor & c) :_c(c), _cr(cr_none),_hasCastled(false)
#ifdef WITH_CAPTURED
  ,capturedN(0)
#endif
  {}

  Player & operator=(const Player & p) {
	  //_c = p._c; // const ...
	  _cr = p._cr;
	  _hasCastled = p._hasCastled;
#ifdef WITH_CAPTURED
	  capturedN = p.capturedN;
	  std::memcpy(captured, p.captured, MAX_CAPTURED_PIECE*sizeof(Piece::eType));
#endif
	  return *this;
  }

  const NSColor::eColor _c;
  eCastlingRight _cr;

  bool _hasCastled;

};

class White : public Player {
public:
  White() : Player(NSColor::c_white) {}
  White & operator=(const White & p){
     Player::operator=(p);
     return *this;
  }
};

class Black : public Player {
public:
  Black() : Player(NSColor::c_black) {}
  Black & operator=(const Black & p){
     Player::operator=(p);
     return *this;
  }
};

inline
void Player::AddCastlingRight   (const Player::eCastlingRight & cr) {
    _cr = static_cast<Player::eCastlingRight>(_cr | cr);
}

inline
void Player::RemoveCastlingRight(const Player::eCastlingRight & cr) {
    _cr  = static_cast<Player::eCastlingRight>(_cr & ~cr);
}
