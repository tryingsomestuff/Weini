#include "UtilPosition.h"
#include "position.h"

namespace Util{

ScoreType CountWhite(const Position & p, float taperedCoeff) {
    ScoreType c = 0;
    for( int k = Piece::t_Wmin ; k <= Piece::t_Wmax ; ++k){
        c += ScoreType(p.NumberOf(Piece::eType(k)) * (COMPLEMENT_TAPERED(taperedCoeff) * Piece::ValueEG(Piece::eType(k))
                                            + taperedCoeff * Piece::Value(Piece::eType(k))));

    }
    return c;
}

ScoreType CountBlack(const Position & p, float taperedCoeff) {
    ScoreType c = 0;
    for( int k = Piece::t_Bmin ; k <= Piece::t_Bmax ; ++k){
        c += ScoreType(p.NumberOf(Piece::eType(k))*(COMPLEMENT_TAPERED(taperedCoeff) * Piece::ValueEG(Piece::eType(k))
                                          + taperedCoeff *  Piece::Value(Piece::eType(k))));
    }
    return c;
}

ScoreType Count(const Position & p, float taperedCoeff) {
    return CountWhite(p,taperedCoeff) + CountBlack(p,taperedCoeff);
}

}
