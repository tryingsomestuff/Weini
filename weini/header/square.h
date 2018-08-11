#pragma once

#include <string>
#include <sstream>
#include <assert.h>

#include "logger.hpp"
#include "definitions.h"

//#define USE_Incr120 // opposite is INCR88

class Square {
public:

  typedef char LightSquare;
  typedef char RankFile;

  static const LightSquare Invalide;

  static inline bool IsValid(LightSquare sq){return sq >= 0;}

  static inline void InvalidateLightSquare(LightSquare & sq){ sq = Invalide; }

  explicit Square(const std::string & s = "" ) :_s(s), _k(-1) { Init(); }
  explicit Square(LightSquare k) : _k(k) { Init(); }

  inline
  bool operator ==(const Square & s)const {
    return _k == s._k;
  }

  inline
  void Reset(int k){
      _s.clear();
      _k = k;
      Init();
  }

  static const char mailbox[120];
  static const char mailbox64[64];
  static bool offBoard[64][43]; // in case of USE_Incr120, then IncrSpe min max is +/- 21

  static void InitOffBoard();

#ifdef USE_Incr120
  #define IsOffTheBoard(s,i120) (Square::mailbox[Square::mailbox64[s]+i120] < 0)
  //#define IsOffTheBoard(s,i120) (Square::offBoard[s][i120+21])
#else
#define conv8x8to0x88(s8x8) (s8x8 + (s8x8 & ~7))
#define conv0x88to8x8(s0x88) ((s0x88 + (s0x88 & 7)) >> 1)
#define IsOffTheBoard(s,i88) ((conv8x8to0x88(s)+i88) & 0x88)
#endif

  static const LightSquare squarea1;
  static const LightSquare squareb1;
  static const LightSquare squarec1;
  static const LightSquare squared1;
  static const LightSquare squaree1;
  static const LightSquare squaref1;
  static const LightSquare squareg1;
  static const LightSquare squareh1;

  static const LightSquare squarea2;
  static const LightSquare squareb2;
  static const LightSquare squarec2;
  static const LightSquare squared2;
  static const LightSquare squaree2;
  static const LightSquare squaref2;
  static const LightSquare squareg2;
  static const LightSquare squareh2;

  static const LightSquare squarea3;
  static const LightSquare squareb3;
  static const LightSquare squarec3;
  static const LightSquare squared3;
  static const LightSquare squaree3;
  static const LightSquare squaref3;
  static const LightSquare squareg3;
  static const LightSquare squareh3;

  static const LightSquare squarea4;
  static const LightSquare squareb4;
  static const LightSquare squarec4;
  static const LightSquare squared4;
  static const LightSquare squaree4;
  static const LightSquare squaref4;
  static const LightSquare squareg4;
  static const LightSquare squareh4;

  static const LightSquare squarea5;
  static const LightSquare squareb5;
  static const LightSquare squarec5;
  static const LightSquare squared5;
  static const LightSquare squaree5;
  static const LightSquare squaref5;
  static const LightSquare squareg5;
  static const LightSquare squareh5;

  static const LightSquare squarea6;
  static const LightSquare squareb6;
  static const LightSquare squarec6;
  static const LightSquare squared6;
  static const LightSquare squaree6;
  static const LightSquare squaref6;
  static const LightSquare squareg6;
  static const LightSquare squareh6;

  static const LightSquare squarea7;
  static const LightSquare squareb7;
  static const LightSquare squarec7;
  static const LightSquare squared7;
  static const LightSquare squaree7;
  static const LightSquare squaref7;
  static const LightSquare squareg7;
  static const LightSquare squareh7;

  static const LightSquare squarea8;
  static const LightSquare squareb8;
  static const LightSquare squarec8;
  static const LightSquare squared8;
  static const LightSquare squaree8;
  static const LightSquare squaref8;
  static const LightSquare squareg8;
  static const LightSquare squareh8;

  static
  inline RankFile    file(Square::LightSquare s) { return FILE(s); }
  static
  inline RankFile    rank(Square::LightSquare s) { return RANK(s); }

  inline RankFile    file()    const { return _file; }
  static
  inline char        fileStr(Square::RankFile file){ return file + 97; }
  inline char        fileStr() const { return fileStr(_file); }
  inline RankFile    rank()    const { return _rank; }
  static
  inline char        rankStr(Square::RankFile rank){ return rank + 49; }
  inline char        rankStr() const { return rankStr(_rank); }
  inline std::string position()const { return _s; }
  inline LightSquare index()   const { return _k; }
  inline bool        IsValid() const { return _k>=0;}

  inline void        Invalidate()    {
      _file = Invalide;
      _rank = Invalide;
      _k = Invalide;
      _s.clear();
  }

private:
  inline
  void Init() {
    if (!_s.empty()) {
      _file = _s.at(0) - 97; // ASCII 'a' = 97
      assert(_file >= 0 && _file < 8);
      _rank = _s.at(1) - 49; // ASCII '1' = 49
      assert(_rank >= 0 && _rank < 8);
      _k = _rank * 8 + _file;
      assert(_k >= 0 && _k < BOARD_SIZE);
    }
    else if (_k >= 0 && _k < BOARD_SIZE) {
      _file = FILE(_k);
      _rank = RANK(_k);
      _s.push_back(_file + 97);
      _s.push_back(_rank + 49);
    }
    else {
       Invalidate();
    }
  }

  std::string _s; // this is heavy, so never use Square, use LightSquare instead
  RankFile _file;
  RankFile _rank;
  LightSquare _k;

};
