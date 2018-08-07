#include "bitboard.h"

#include <bitset>

const BitBoard BitBoards::dummy = 0;

const BitBoard BitBoards::whiteSquare     = 0x55AA55AA55AA55AA;
const BitBoard BitBoards::blackSquare     = 0xAA55AA55AA55AA55;
const BitBoard BitBoards::whiteSideSquare = 0x00000000FFFFFFFF;
const BitBoard BitBoards::blackSideSquare = 0xFFFFFFFF00000000;
const BitBoard BitBoards::white7thRank    = 0x000000000000FF00;
const BitBoard BitBoards::black7thRank    = 0x00FF000000000000;

void BitBoards::initAll() {
	allBB[0] = &dummy;
	allBB[1] = &blackKing;
	allBB[2] = &blackQueen;
	allBB[3] = &blackRook;
	allBB[4] = &blackBishop;
	allBB[5] = &blackKnight;
	allBB[6] = &blackPawn;
	allBB[7] = &dummy;
	allBB[8] = &whitePawn;
	allBB[9] = &whiteKnight;
	allBB[10] = &whiteBishop;
	allBB[11] = &whiteRook;
	allBB[12] = &whiteQueen;
	allBB[13] = &whiteKing;
	allBB[14] = &dummy;
}

BitBoards::BitBoards() :
	whitePiece(0),
	whitePawn(0),
	whiteKnight(0),
	whiteBishop(0),
	whiteRook(0),
	whiteQueen(0),
	whiteKing(0),
	blackPiece(0),
	blackPawn(0),
	blackKnight(0),
	blackBishop(0),
	blackRook(0),
	blackQueen(0),
	blackKing(0)
{
	initAll();
}

void BitBoards::Reset() {
	whitePiece = 0;
	whitePawn = 0;
	whiteKnight = 0;
	whiteBishop = 0;
	whiteRook = 0;
	whiteQueen = 0;
	whiteKing = 0;
	blackPiece = 0;
	blackPawn = 0;
	blackKnight = 0;
	blackBishop = 0;
	blackRook = 0;
	blackQueen = 0;
	blackKing = 0;
}

BitBoards::BitBoards(const BitBoards & bbs){
    whitePiece  = bbs.whitePiece;
    whitePawn   = bbs.whitePawn;
    whiteKnight = bbs.whiteKnight;
    whiteBishop = bbs.whiteBishop;
    whiteRook   = bbs.whiteRook;
    whiteQueen  = bbs.whiteQueen;
    whiteKing   = bbs.whiteKing;
    blackPiece  = bbs.blackPiece;
    blackPawn   = bbs.blackPawn;
    blackKnight = bbs.blackKnight;
    blackBishop = bbs.blackBishop;
    blackRook   = bbs.blackRook;
    blackQueen  = bbs.blackQueen;
    blackKing   = bbs.blackKing;

    initAll();
}

BitBoards::BitBoards(BitBoards && bbs){
    whitePiece  = bbs.whitePiece;
    whitePawn   = bbs.whitePawn;
    whiteKnight = bbs.whiteKnight;
    whiteBishop = bbs.whiteBishop;
    whiteRook   = bbs.whiteRook;
    whiteQueen  = bbs.whiteQueen;
    whiteKing   = bbs.whiteKing;
    blackPiece  = bbs.blackPiece;
    blackPawn   = bbs.blackPawn;
    blackKnight = bbs.blackKnight;
    blackBishop = bbs.blackBishop;
    blackRook   = bbs.blackRook;
    blackQueen  = bbs.blackQueen;
    blackKing   = bbs.blackKing;

    initAll();
}

BitBoards& BitBoards::operator=(const BitBoards& bbs ){
    whitePiece  = bbs.whitePiece;
    whitePawn   = bbs.whitePawn;
    whiteKnight = bbs.whiteKnight;
    whiteBishop = bbs.whiteBishop;
    whiteRook   = bbs.whiteRook;
    whiteQueen  = bbs.whiteQueen;
    whiteKing   = bbs.whiteKing;
    blackPiece  = bbs.blackPiece;
    blackPawn   = bbs.blackPawn;
    blackKnight = bbs.blackKnight;
    blackBishop = bbs.blackBishop;
    blackRook   = bbs.blackRook;
    blackQueen  = bbs.blackQueen;
    blackKing   = bbs.blackKing;

    initAll();

	return *this;
}

BitBoards& BitBoards::operator=(BitBoards&& bbs){
    whitePiece  = bbs.whitePiece;
    whitePawn   = bbs.whitePawn;
    whiteKnight = bbs.whiteKnight;
    whiteBishop = bbs.whiteBishop;
    whiteRook   = bbs.whiteRook;
    whiteQueen  = bbs.whiteQueen;
    whiteKing   = bbs.whiteKing;
    blackPiece  = bbs.blackPiece;
    blackPawn   = bbs.blackPawn;
    blackKnight = bbs.blackKnight;
    blackBishop = bbs.blackBishop;
    blackRook   = bbs.blackRook;
    blackQueen  = bbs.blackQueen;
    blackKing   = bbs.blackKing;

    initAll();

	return *this;
}

// this is slow !
const Piece::eType BitBoards::Get (Square::LightSquare k) const{
   const BitBoard b = (1ull << k);
   return Piece::eType(Piece::t_empty // must be 0 for this to work !!!!
        + ((b & whitePawn   )==0 ? 0 : Piece::t_Wpawn   )
        + ((b & whiteKnight )==0 ? 0 : Piece::t_Wknight )
        + ((b & whiteBishop )==0 ? 0 : Piece::t_Wbishop )
        + ((b & whiteRook   )==0 ? 0 : Piece::t_Wrook   )
        + ((b & whiteQueen  )==0 ? 0 : Piece::t_Wqueen  )
        + ((b & whiteKing   )==0 ? 0 : Piece::t_Wking   )
        + ((b & blackPawn   )==0 ? 0 : Piece::t_Bpawn   )
        + ((b & blackKnight )==0 ? 0 : Piece::t_Bknight )
        + ((b & blackBishop )==0 ? 0 : Piece::t_Bbishop )
        + ((b & blackRook   )==0 ? 0 : Piece::t_Brook   )
        + ((b & blackQueen  )==0 ? 0 : Piece::t_Bqueen  )
        + ((b & blackKing   )==0 ? 0 : Piece::t_Bking   ));
}

// this is fast
void BitBoards::SetPiece(Square::LightSquare k, Piece::eType t){
   if (t == Piece::t_empty ) return;
   BitScan::SetBit(*const_cast<BitBoard*>(allBB[TypeToInt(t)]),k);
}

// this is fast
void BitBoards::UnSetPiece(Square::LightSquare k, Piece::eType t) {
    if (t == Piece::t_empty ) return;
    BitScan::UnsetBit(*const_cast<BitBoard*>(allBB[TypeToInt(t)]),k);
}

// this is fast
bool BitBoards::IsTherePieceOnBoard(Piece::eType t)const{
    return (*(allBB[TypeToInt(t)])) != 0;
}

// this is fast
int BitBoards::NumberOf(Piece::eType t)const{
    if (t == Piece::t_empty ) return 0;
    return POPCOUNT(*(allBB[TypeToInt(t)]));
}

const BitBoard & BitBoards::GetBitBoard(Piece::eType t)const {
    return *(allBB[TypeToInt(t)]);
}

const BitBoard & BitBoards::GetWhitePieceBitBoard()const{
    return whitePiece;
}

const BitBoard & BitBoards::GetBlackPieceBitBoard()const{
    return blackPiece;
}

bool BitBoards::IsOfType(Piece::eType t, Square::LightSquare k)const {
    return BitScan::IsSet(*(allBB[TypeToInt(t)]),k);
}

namespace {
    std::string ShowBitBoard(const BitBoard & b) {
		std::bitset<64> bs(b);
		std::stringstream ss;
		ss << std::endl;
		for (int j = 7; j >= 0; --j) {
			ss << "+-+-+-+-+-+-+-+-+" << std::endl;
			ss << "|";
			for (int i = 0; i < 8; ++i) {
				ss << (bs[i+j*8]?"X":" ") << '|';
			}
			ss << std::endl; ;
		}
		ss << "+-+-+-+-+-+-+-+-+";
		return ss.str();
	}
}

void BitBoards::Display()const {
	LOG(logINFO) << "Bitboard";
	LOG(logINFO) << "WPawn   " << std::bitset<64>(whitePawn);
    LOG(logINFO) << ShowBitBoard(whitePawn);
	LOG(logINFO) << "WKnight " << std::bitset<64>(whiteKnight);
    LOG(logINFO) << ShowBitBoard(whiteKnight);
	LOG(logINFO) << "WBishop " << std::bitset<64>(whiteBishop);
    LOG(logINFO) << ShowBitBoard(whiteBishop);
	LOG(logINFO) << "WRook   " << std::bitset<64>(whiteRook);
    LOG(logINFO) << ShowBitBoard(whiteRook);
	LOG(logINFO) << "WQueen  " << std::bitset<64>(whiteQueen);
    LOG(logINFO) << ShowBitBoard(whiteQueen);
	LOG(logINFO) << "WKing   " << std::bitset<64>(whiteKing);
    LOG(logINFO) << ShowBitBoard(whiteKing);
	LOG(logINFO) << "BPawn   " << std::bitset<64>(blackPawn);
    LOG(logINFO) << ShowBitBoard(blackPawn);
	LOG(logINFO) << "BKnight " << std::bitset<64>(blackKnight);
    LOG(logINFO) << ShowBitBoard(blackKnight);
	LOG(logINFO) << "BBishop " << std::bitset<64>(blackBishop);
    LOG(logINFO) << ShowBitBoard(blackBishop);
	LOG(logINFO) << "BRook   " << std::bitset<64>(blackRook);
    LOG(logINFO) << ShowBitBoard(blackRook);
	LOG(logINFO) << "BQueen  " << std::bitset<64>(blackQueen);
    LOG(logINFO) << ShowBitBoard(blackQueen);
	LOG(logINFO) << "BKing   " << std::bitset<64>(blackKing);
    LOG(logINFO) << ShowBitBoard(blackKing);
}
