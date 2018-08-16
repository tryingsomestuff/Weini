#pragma once

#include "logger.hpp"
#include "Piece.h"
#include "square.h"
#include "util.h"
#include "float.h"
#include "stats.h"

#include <map>
#include <mutex>

class Position;
class Searcher;

// this class is fully static ...
class Analyse {
public:

    // init PSQT
    static void InitConstants();

    static void ActivateAnalysisDisplay(bool b);

    // also used for move sorting
    static ScoreType HelperPosition(const Position & p, Square::LightSquare index , Piece::eType type, float taperedCoeff);

    // return > 0 if white is better, < 0 if black is better
    ScoreType Run(Position & p, float taperedCoeff);

    // return > 0 if white is better, < 0 if black is better
    ScoreType RunNoMaterial(Position & p, ScoreType materialEval, float taperedCoeff);

    // return > 0 if white is better, < 0 if black is better
    ScoreType RunLazy(Position & p);

    static void InitTTE();
    static void InitTTL();
    static void InitTTP();

    static void ClearTT ();
    static void ClearTTL();
    static void ClearTTP();

    struct EvalCache{

        EvalCache():cacheHash(0){}

        struct PieceNumber{
            PieceNumber():nbWp(-1),nbWk(-1),nbWb(-1),nbWr(-1),nbWq(-1),
                          nbBp(-1),nbBk(-1),nbBb(-1),nbBr(-1),nbBq(-1),
                          nbW(-1),nbB(-1),nbWm(-1),nbBm(-1){}
            int nbWp;
            int nbWk;
            int nbWb;
            int nbWr;
            int nbWq;
            int nbBp;
            int nbBk;
            int nbBb;
            int nbBr;
            int nbBq;
            int nbW;
            int nbB;
            int nbWm;
            int nbBm;
        };

        PieceNumber pieceNumber;

        struct KingEval{
           KingEval():wks(Square::Invalide),bks(Square::Invalide){}
           Square::LightSquare wks;
           Square::LightSquare bks;
        };
        KingEval kingEval;

        // init material cache
        void InitMaterialCache(const Position & p);

        Util::Zobrist::HashType cacheHash;

        template < typename T >
        static inline
        T & SetEvalCache(T & v, const T t){
            v = t;
            return v;
        }

    };

    inline EvalCache & MaterialCache(){return _cache;}

private:

    static bool GetTT       (Util::Zobrist::HashType  hash,
                             ScoreType &              score);

    static void InsertTT    (Util::Zobrist::HashType  hash,
                             const ScoreType &        score);

    static bool GetTTLazy   (Util::Zobrist::HashType  hash,
                             ScoreType &              score);

    static void InsertTTLazy(Util::Zobrist::HashType  hash,
                             const ScoreType &        score);

    static bool GetTTPawn   (Util::Zobrist::HashType  hash,
                             ScoreType &              score);

    static void InsertTTPawn(Util::Zobrist::HashType  hash,
                             const ScoreType &        score);

private:

    // number of piece on the board
    ScoreType EvalMaterial(const Position & p, float taperedCoeff, bool doTradeOffBonus = false) ;

    // little bonus for not being in check
    ScoreType EvalInCheck(const Position & p, float taperedCoeff) ;

    // little bonus/malus for piece pair
    ScoreType EvalPiecePair(const Position & p, float taperedCoeff) ;

    // bonus for mobility
    ScoreType EvalMobility(Position & p, float taperedCoeff) ;

    // king safety pawn shield
    ScoreType EvalPawnShield(const Position & p, float taperedCoeff) ;

    // king safety, want to castle
    ScoreType EvalCastle(const Position & p, float taperedCoeff) ;

    // development, connected rook
    ScoreType EvalConnectedRooks(const Position & p, float taperedCoeff) ;

    // center control
    ScoreType EvalCenterControl(const Position & p, float taperedCoeff) ;

    // king troppism (piece attack)
    ScoreType EvalKingTroppism(const Position & p, float taperedCoeff) ;

    // blocked piece (developpement)
    ScoreType EvalBlockedPiece(const Position & p, float taperedCoeff) ;

    // optimized pack
    ScoreType EvalOptimDriver(const Position & p, float taperedCoeff, bool display) ;

    // used to activate static analysis detailed output
    static bool _display;

    EvalCache _cache;
};

