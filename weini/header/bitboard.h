#pragma once

#include "definitions.h"

#include <assert.h>
#include "Piece.h"
#include "square.h"

#ifdef _WIN32
typedef uint64_t u_int64_t;
#endif

typedef uint64_t BitBoard;

#ifdef _WIN32
//#define POPCOUNT(x) int(__popcnt64(x))

inline
int popcount64(uint64_t x) {
    static const uint64_t m1  = 0x5555555555555555; //binary: 0101...
    static const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
    static const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
    static const uint64_t m8  = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
    static const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
    static const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
    static const uint64_t hff = 0xffffffffffffffff; //binary: all ones
    static const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

#define POPCOUNT(x) popcount64(x)

#else // linux
#define POPCOUNT(x) int(__builtin_popcountll(x))
#endif

namespace BitScan {

#ifdef _WIN32
// array used for BitScanForward
const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
inline
int BitScanForward(u_int64_t bb) {
   static const u_int64_t debruijn64 = 0x03f79d71b4cb0a89;
   if (bb == 0){
      LOG(logFATAL) << "Empty bitboard";
   }
   return index64[((bb ^ (bb-1)) * debruijn64) >> 58];
}
#else
inline
int BitScanForward(u_int64_t bb) {
   if (bb == 0){
      LOG(logFATAL) << "Empty bitboard";
   }
   return __builtin_ctzll(bb);
}
#endif

#define SquareToBitboard(k) (1ull<<k)

inline
int CountBit(const BitBoard & b) {
    return POPCOUNT(b);
}

inline
void SetBit(BitBoard & b, Square::LightSquare k){
    b |= SquareToBitboard(k);
}

inline
void UnsetBit(BitBoard & b, Square::LightSquare k){
    b &= ~SquareToBitboard(k);
}

inline
bool IsSet(const BitBoard & b, Square::LightSquare k){
   return (SquareToBitboard(k) & b) != 0;
}

} // namespace BitScan

class BitBoards{

   friend class Position;

   static const BitBoard dummy; // = 0

 public:

   static const BitBoard whiteSquare;
   static const BitBoard blackSquare;
   static const BitBoard whiteSideSquare;
   static const BitBoard blackSideSquare;
   static const BitBoard white7thRank;
   static const BitBoard black7thRank;

   static const BitBoard whiteKingQueenSide;
   static const BitBoard whiteKingKingSide;
   static const BitBoard blackKingQueenSide;
   static const BitBoard blackKingKingSide;

   static inline
   bool IsWhiteSquare(Square::LightSquare k){
       return ((BitBoards::whiteSquare & SquareToBitboard(k)) != 0);
   }

   inline
   bool IsWhiteBishopWhite()const{
      switch(BitScan::CountBit(whiteBishop)){
      case 0:
          return false;
      case 1:
          return IsWhiteSquare(BitScan::BitScanForward(whiteBishop));
      case 2:
          return true;
      }
      return false;
   }

   inline
   bool IsBlackBishopWhite()const{
      switch(BitScan::CountBit(blackBishop)){
      case 0:
          return false;
      case 1:
          return IsWhiteSquare(BitScan::BitScanForward(blackBishop));
      case 2:
          return true;
      }
      return false;
   }

   BitBoards();

   BitBoards(const BitBoards & );
   BitBoards(BitBoards && );
   BitBoards& operator=(const BitBoards&);
   BitBoards& operator=(BitBoards&&);

   void Reset();

   const Piece::eType Get (Square::LightSquare k) const;

   void               SetPiece(Square::LightSquare k, Piece::eType t);

   void               UnSetPiece(Square::LightSquare k, Piece::eType t);

   bool IsTherePieceOnBoard(Piece::eType t)const;

   int NumberOf(Piece::eType t)const;

   inline
   Square::LightSquare WhiteKingSquare()const{ return BitScan::BitScanForward(whiteKing); }

   inline
   Square::LightSquare BlackKingSquare()const{ return BitScan::BitScanForward(blackKing); }

   inline
   void InvalidateWhiteKing(){ whiteKing = 0; }

   inline
   void InvalidateBlackKing(){ blackKing = 0; }

   void Display()const;

   const BitBoard & GetBitBoard(Piece::eType t)const;
   const BitBoard & GetWhitePieceBitBoard()const;
   const BitBoard & GetBlackPieceBitBoard()const;

   bool IsOfType(Piece::eType t, Square::LightSquare k)const;

private:

   BitBoard whitePawn;
   BitBoard whiteKnight;
   BitBoard whiteBishop;
   BitBoard whiteRook;
   BitBoard whiteQueen;
   BitBoard whiteKing;

   const BitBoard * allBB[15];

   BitBoard blackPawn;
   BitBoard blackKnight;
   BitBoard blackBishop;
   BitBoard blackRook;
   BitBoard blackQueen;
   BitBoard blackKing;

   BitBoard whitePiece;
   BitBoard blackPiece;

private:

    void initAll();
};
