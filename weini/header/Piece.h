#pragma once

#include <assert.h>

#ifdef DEBUG_PIECE
#define assert__(x) for ( ; !(x) ; assert(x) )
#else
#define assert__(x)
#endif

#include "color.h"
#include "definitions.h"
#include "logger.hpp"

#define TypeToInt(t) (t+Piece::t_shift)

#define IntToType(t) eType(t-Piece::t_shift)

class Piece {
public:
  enum eType : char {
    t_empty = 0,
    t_Wmin = 1,
    t_Wpawn = 1,
    t_Wknight = 2,
    t_Wbishop = 3,
    t_Wrook = 4,
    t_Wqueen = 5,
    t_Wking = 6,
    t_Wmax = 6,
    t_WkingFalse = 7,
    t_Bmax = -1,
    t_Bpawn = -1,
    t_Bknight = -2,
    t_Bbishop = -3,
    t_Brook = -4,
    t_Bqueen = -5,
    t_Bking = -6,
    t_Bmin = -6,
    t_BkingFalse = -7,
    t_max = 15
  };

  static constexpr int t_shift = t_max/2;

  explicit Piece(eType t = t_empty) :_t(t) {
      assert(t >= -7);
      assert(t <= 7);
  }

  inline
  bool IsEmpty()const{
    return _t == t_empty;
  }

  inline
  eType Type()const {
    return _t;
  }

  inline
  void Set(eType t){
#ifdef DEBUG_PIECE
    assert(t >= -7);
    assert(t <= 7);
#endif
    _t = t;
  }

  static
  inline NSColor::eColor Color(eType t){
#ifdef DEBUG_PIECE
      assert(t >= -7);
      assert(t <= 7);
#endif
      ///@todo any optim here would be great
      static const NSColor::eColor color[3] = { NSColor::c_black,NSColor::c_none,NSColor::c_white };
      return color[ 1 + (t > 0) - (t < 0) ];
  }

  inline
  NSColor::eColor Color()const {
    return Color(_t);
  }

  static
  inline bool IsWhite(eType t){
      return t > 0;
  }

  static
  inline char ColorSign(eType t){
    return (t > 0) - (t < 0);
  }

  static
  inline bool IsBlack(eType t){
      return t < 0;
  }

  static
  inline bool IsEmpty(eType t){
      return t == 0;
  }

  static
  inline bool IsPawn(eType t){
      ///@todo any optim here would be great
      return std::abs(t) == Piece::t_Wpawn;
  }

  static
  inline bool IsWhitePawn(eType t) {
      return t == Piece::t_Wpawn;
  }

  static
  inline bool IsBlackPawn(eType t) {
      return t == Piece::t_Bpawn;
  }

  inline
  std::string Name()const {
    return Name(_t);
  }

  static inline
  std::string Name(Piece t) {
    return Name(t.Type());
  }

  static inline
  const std::string & Name(eType t) {
#ifdef DEBUG_PIECE
    assert(t >= -7);
    assert(t <= 7);
#endif
    static const std::string symbol[t_max] = { "falseKing" , "king", "queen", "rook", "bishop", "knight", "pawn",
                                               "empty",
                                               "Pawn", "Knight", "Bishop", "Rook", "Queen", "King", "FalseKing" };
    return symbol[TypeToInt(t)];
  }

  inline
  std::string Symbol()const {
    return Symbol(_t);
  }

  static inline
  std::string Symbol(Piece t) {
    return Symbol(t.Type());
  }

  static inline
  const std::string & Symbol(eType t) {
#ifdef DEBUG_PIECE
    assert(t >= -7);
    assert(t <= 7);
#endif
    static const std::string symbol[t_max] = { "x", "k", "q", "r", "b", "n", "p", " ", "P", "N", "B", "R", "Q", "K", "X" };
    return symbol[TypeToInt(t)];
  }

  static inline
  const std::string & SymbolAlg(eType t) {
#ifdef DEBUG_PIECE
    assert(t >= -7);
    assert(t <= 7);
#endif
    static const std::string symbol[t_max] = { "X", "K", "Q", "R", "B", "N", "P", " ", "P", "N", "B", "R", "Q", "K", "X" };
    return symbol[TypeToInt(t)];
  }

  static inline
  bool IsRQ(eType t){
      static const bool isrq[t_max] = { false, false, true, true, false, false, false, false, false, false, false, true, true, false, false };
      return isrq[TypeToInt(t)];
  }

  static inline
  bool IsBQ(eType t){
      static const bool isbq[t_max] = { false, false, true, false, true, false, false, false, false, false, true, false, true, false, false };
      return isbq[TypeToInt(t)];
  }

  static inline
  bool IsK(eType t){
      return std::abs(t) == t_Wking;
  }

  inline
  ScoreType Value()const {
    return Value(*this);
  }

  static inline
  ScoreType Value(Piece t) {
    return Value(t.Type());
  }

  static inline
  ScoreType Value(eType t) {
#ifdef DEBUG_PIECE
    assert(t >= -7);
    assert(t <= 7);
#endif
    static const ScoreType nullScore = 0;
    static const ScoreType * value[t_max] = { &nullScore,
                                              &Definitions::pieceValue.kingValueB,
                                              &Definitions::pieceValue.queenValueB,
                                              &Definitions::pieceValue.rookValueB,
                                              &Definitions::pieceValue.bishopValueB,
                                              &Definitions::pieceValue.knightValueB,
                                              &Definitions::pieceValue.pawnValueB,
                                              &nullScore,
                                              &Definitions::pieceValue.pawnValueW,
                                              &Definitions::pieceValue.knightValueW,
                                              &Definitions::pieceValue.bishopValueW,
                                              &Definitions::pieceValue.rookValueW,
                                              &Definitions::pieceValue.queenValueW,
                                              &Definitions::pieceValue.kingValueW,
                                              &nullScore };
    return *(value[TypeToInt(t)]);
  }

  inline
      ScoreType ValueAbs()const {
      return ValueAbs(*this);
  }

  static inline
      ScoreType ValueAbs(Piece t) {
      return ValueAbs(t.Type());
  }

  static inline
      ScoreType ValueAbs(eType t) {
#ifdef DEBUG_PIECE
      assert(t >= -7);
      assert(t <= 7);
#endif
      static const ScoreType nullScore = 0;
      static const ScoreType * value[t_max] = { &nullScore,
                                                &Definitions::pieceValue.kingValueW,
                                                &Definitions::pieceValue.queenValueW,
                                                &Definitions::pieceValue.rookValueW,
                                                &Definitions::pieceValue.bishopValueW,
                                                &Definitions::pieceValue.knightValueW,
                                                &Definitions::pieceValue.pawnValueW,
                                                &nullScore,
                                                &Definitions::pieceValue.pawnValueW,
                                                &Definitions::pieceValue.knightValueW,
                                                &Definitions::pieceValue.bishopValueW,
                                                &Definitions::pieceValue.rookValueW,
                                                &Definitions::pieceValue.queenValueW,
                                                &Definitions::pieceValue.kingValueW,
                                                &nullScore };
      return *(value[TypeToInt(t)]);
  }


  static inline
      ScoreType ValueEG(eType t) {
#ifdef DEBUG_PIECE
      assert(t >= -7);
      assert(t <= 7);
#endif
      static const ScoreType nullScore = 0;
      static const ScoreType * value[t_max] = { &nullScore,
                                                &Definitions::pieceValue.kingEGValueB,
                                                &Definitions::pieceValue.queenEGValueB,
                                                &Definitions::pieceValue.rookEGValueB,
                                                &Definitions::pieceValue.bishopEGValueB,
                                                &Definitions::pieceValue.knightEGValueB,
                                                &Definitions::pieceValue.pawnEGValueB,
                                                &nullScore,
                                                &Definitions::pieceValue.pawnEGValueW,
                                                &Definitions::pieceValue.knightEGValueW,
                                                &Definitions::pieceValue.bishopEGValueW,
                                                &Definitions::pieceValue.rookEGValueW,
                                                &Definitions::pieceValue.queenEGValueW,
                                                &Definitions::pieceValue.kingEGValueW,
                                                &nullScore };
      return *(value[TypeToInt(t)]);
  }

private:
  eType _t;
};
