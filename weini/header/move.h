#pragma once

#include <string>
#include <list>
#include <utility>
#include "square.h"
#include "Piece.h"
#include <float.h>
#include "util.h"
#include "fastContainer.h"
#include "definitions.h"
#include "logger.hpp"
#include "UtilMove.h"

class Position;
class Searcher;
struct Line;
struct Transposition;

struct MiniMove {
    Square::LightSquare from;
    Square::LightSquare to;
    UtilMove::eSpecialMove moveType;
};

#pragma pack(push, 2)
class Move {
  friend class MoveGenerator;
  friend bool UtilMove::ValidatePawn     (const Move & m, const Position & p);
  friend bool UtilMove::ValidateIsCheck  (const Move & m, Position & p);
  friend bool UtilMove::DetectStandardCapture  (const Move & m, const Position & p);
  friend void UtilMove::SetIsCapture     (const Move & m, bool b);
  friend bool UtilMove::ValidateEnPassant(const Move & m, const Position & p);

public:

  static bool Ordering(FastContainer<Move> & moves, Position & p, Searcher &searcher, bool isPV,
                       DepthType DepthType, const Transposition * ttt, const Line * pv, bool trustedMove = false);

  typedef unsigned short int HashType;

  static inline Square::LightSquare     Hash2From(HashType h) { return (h >> 10) & 0x3F ;}
  static inline Square::LightSquare     Hash2To  (HashType h) { return (h >>  4) & 0x3F ;}
  static inline UtilMove::eSpecialMove  Hash2Type(HashType h) { return UtilMove::eSpecialMove( (h & 0xF)+UtilMove::sm_mini);}
  static inline HashType                ToHash(Square::LightSquare     from,
                                               Square::LightSquare     to,
                                               UtilMove::eSpecialMove  type){
      if (type == UtilMove::sm_invalide) {
          LOG(logFATAL) << "Hashing an invalid move";
          from = 0;
          to = 0;
      }
      return (from << 10) | (to << 4) | (type-UtilMove::sm_mini);
  }
  static inline MiniMove                HashToMini(HashType h) { return { Hash2From(h), Hash2To(h), Hash2Type(h) }; }

  explicit Move(UtilMove::eSpecialMove moveType = UtilMove::sm_invalide);
  explicit Move(const Square::LightSquare & from, const Square::LightSquare & to, UtilMove::eSpecialMove moveType = UtilMove::sm_standard );
  explicit Move(const std::string & s, const NSColor::eColor &c, bool alt = true);
  explicit Move(const std::string & sAlgAbr, Position & p);
  explicit Move(const MiniMove & mini);

  /*
  Move(const Move & m);
  Move(Move && m);
  Move& operator=(const Move&);
  Move& operator=(Move&&);
  */

  ~Move();

  MiniMove GetMini()const;

  bool operator<(const Move &rhs) const {
      return ZHash() < rhs.ZHash();
  }
  bool operator==(const Move &rhs) const {
      return ZHash() == rhs.ZHash();
  }

  // This one is slow, do not use it deep inside the search
  Position ValidateAndApply(const Position & p, bool *success = NULL);

  // This one is slow, do not use it deep inside the search
  Position & ValidateAndApplyInPlace(Position & p, bool *success = NULL);

  inline bool IsValid()const{
      return _moveType != UtilMove::sm_invalide;
  }

  void Reset();
  void Set(const Square::LightSquare & from, const Square::LightSquare & to, UtilMove::eSpecialMove moveType = UtilMove::sm_standard);
  void Set(UtilMove::eSpecialMove moveType = UtilMove::sm_standard);

  inline bool IsCapture()const{
      return _isCapture;
  }

  inline bool IsCheck()const{
      return _isCheck;
  }

  bool IsPawnPush(const Position & p)const;

  bool IsAdvancedPawnPush(const Position & p)const;

  bool IsNearPromotion(const Position & p)const;

  static
  bool IsNearPromotion(const Position & p, const Square::LightSquare & s);

  inline bool IsCastling() const{ return ( _moveType >= UtilMove::sm_Ks && _moveType <= UtilMove::sm_qs) ;}

  inline bool IsPromotion()const{ return _moveType > UtilMove::sm_castling_max;}

  std::string Show      () const;

  // use position to know piece type
  std::string Show      (const Position &p) const;

  // slow !
  std::string ShowAlg   (const Position &p, bool withSep = true, bool specialRoque = false, bool withCheck = true) const;

  // very very slow (only used to generate PGN
  std::string ShowAlgAbr(Position &p) const;

  HashType ZHash()const;

  inline const Square::LightSquare & From()const{ return _from;}
  inline const Square::LightSquare & To()  const{ return _to;}
  inline UtilMove::eSpecialMove      Type()const{ return _moveType;}

  inline
  void Revert(){ Square::LightSquare tmp(_from); _from = _to ; _to = tmp; }

  bool Validate(Position & p, bool validateCheckState = true, bool checkValidity = true)/*const*/;

  inline
  ScoreType SortScore()const{ return _sortScore; }

  inline
  bool CanBeReduced()const{return SortScore() < Definitions::scores.reductibleSortScore;}

  inline
  void SetSortScore(ScoreType s)const{ _sortScore = s; }

private:

  // this one is updated in sort routines
  mutable ScoreType              _sortScore;

  Square::LightSquare            _from;
  Square::LightSquare            _to;

  // those are updated in Validation routines
  mutable HashType               _zhash;
  mutable UtilMove::eSpecialMove _moveType;
  mutable bool                   _isCapture;
  mutable bool                   _isCheck;

};
#pragma pack(pop)

inline
std::ostream & operator<<(std::ostream & of, const Move & o){
    of << o.Show();
    if ( o.SortScore() != Definitions::scores.defaultMoveScore ){
       of << "(" << o.SortScore() << ")";
    }
    of << " |" ;
    return of;
}



