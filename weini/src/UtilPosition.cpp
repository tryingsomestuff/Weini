#include "UtilPosition.h"
#include "position.h"

namespace Util{

ScoreType CountWhite(const Position & p) {
    ScoreType c = 0;
    for( int k = Piece::t_Wmin ; k <= Piece::t_Wmax ; ++k){
        c += p.NumberOf(Piece::eType(k))*(p.IsEndGame() ? Piece::ValueEG(Piece::eType(k))
                                                        : Piece::Value(Piece::eType(k)));

    }
    return c;
}

ScoreType CountBlack(const Position & p) {
    ScoreType c = 0;
    for( int k = Piece::t_Bmin ; k <= Piece::t_Bmax ; ++k){
        c += p.NumberOf(Piece::eType(k))*(p.IsEndGame() ? Piece::ValueEG(Piece::eType(k))
                                                        : Piece::Value(Piece::eType(k)));
    }
    return c;
}

ScoreType Count(const Position & p) {
    return CountWhite(p) + CountBlack(p);
}

int NumberOfWhitePiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetWhitePieceBitBoard())
            - BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Wpawn));
}

int NumberOfBlackPiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetBlackPieceBitBoard())
            - BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Bpawn));
}

int NumberOfWhiteMinorPiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Wknight))
            + BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Wbishop));
}

int NumberOfBlackMinorPiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Bknight))
            + BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Bbishop));
}

int NumberOfWhiteMajorPiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Wqueen))
            + BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Wrook));
}

int NumberOfBlackMajorPiece(const Position & p) {
    return BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Bqueen))
            + BitScan::CountBit(p.BitBoard().GetBitBoard(Piece::t_Brook));
}

}
