#pragma once

#include "analyse.h"
#include "logger.hpp"
#include "move.h"
#include "position.h"
#include "ttable.h"
#include "thread.h"
#include "timeman.h"
#include "line.h"
#include "game.h"

#include <float.h>
#include <map>

struct Line;

//#define TT_DEBUG_HASH_COL // look for real hash collision ... should not append very often !

inline
int ReductionId(DepthType depth, int moveCount, int maxi){
    double r = sqrt((depth*moveCount)/8);
    return std::min(maxi,int(std::round(r)));
}

inline
bool IsSilent(const Move & m, const Position & p){
    return !m.IsCapture()
        && !m.IsCheck()
        && !m.IsNearPromotion(p)
        && !m.IsPromotion()
        && !m.IsCastling();
}

inline
bool IsVerySilent(const Move & m, const Position & p){
     return !m.IsCapture()
         && !m.IsCheck()
       //&& !m.IsNearPromotion(p)
       //&& !m.IsPromotion()
         && !m.IsCastling()
         && !m.IsAdvancedPawnPush(p);
}

inline
bool IsSilentCapture(const Move & m, const Position & p){
      return m.IsCapture()
         && !m.IsCheck()
         //&& !m.IsNearPromotion(p)
         //&& !m.IsPromotion()
         && !m.IsAdvancedPawnPush(p);
}

class Searcher : public SearcherBase {
public:

    Searcher(size_t n = 0) : SearcherBase(n), _allowedThinkTime(777){
        ResetKiller(false);
        ResetHistory();
        ResetCounter();
    }
    virtual ~Searcher() {}

    void Search() override;

    static void Reset();

    SearchedMove Search(Position & p,
                 DepthType  depth,
                 Line *     pv = NULL,
                 bool       allowNullMove = true);

    SearchedMove SearchRoot  (ScoreType                   alpha,
                              ScoreType                   beta,
                              const FastContainer<Move> & moves,
                              Position &                  p,
                              DepthType                   iterativeDeepeningDepth,
                              Line *                      pv = NULL,
                              bool                        allowNullMove = true,
                              bool                        timeControl = true,
                              bool                        forbidExtension = false,
                              bool                        trustedMoves = true);

    ScoreType Negamax  (ScoreType       alpha,
                        ScoreType       beta,
                        DepthType       depthleft,
                        Position &      p,
                        NSColor::eColor color,
                        DepthType       iterativeDeepeningDepth,
                        bool            fromPV,
                        Line *          pv = NULL,
                        //DepthType       depthSinceLastNullMove = -1,
                        bool            allowNullMove = true,
                        bool            timeControl = true,
                        bool            forbidExtension = false);

    ScoreType NegaQuiesce(ScoreType       alpha,
                          ScoreType       beta,
                          DepthType       depthleft,
                          Position &      p,
                          NSColor::eColor color,
                          bool            fromPV,
                          bool            timeControl);

    ScoreType Analyze(Position & p, bool lazy = false);

    void     ApplyMove            (Position & p, const Move & m, bool verifyKingCapture = true);
    Position ApplyMoveCopyPosition(Position & p, const Move & m, bool verifyKingCapture = true);

    // draw
    static
    bool IsForcedDraw(const Game & g, const Position & p, Analyse::EvalCache & cache, DepthType nbRep, bool displayDebug = false);

    static
    bool SEE(const Position & p, const Move & m, ScoreType threshold = 0);

    #ifdef TT_DEBUG_HASH_COL
    static std::map<std::string, TTHASHTYPE> fenToZ;
    static std::map<TTHASHTYPE, std::string> ZToFen;
    static
    bool CheckFenHashCol(const std::string & fen, const TTHASHTYPE & hash, const Position & p);
    #endif // TT_DEBUG_HASH_COL

    Util::Zobrist::HashType killerMovesWhite[2][MAX_GAME_PLY];
    Util::Zobrist::HashType killerMovesBlack[2][MAX_GAME_PLY];

    void ResetKiller(bool copySlot = true);

    int searchHistory[15][BOARD_SIZE];
    static const unsigned int historyMin = 0;
    static const unsigned int historyMax = 1800;

    void ResetHistory();

    Util::Zobrist::HashType counterMove[BOARD_SIZE][BOARD_SIZE];

    void ResetCounter();

    static std::atomic_bool stopFlag;

    Game game;

    static bool display;
    static bool displayDebug;

    Analyse analyze;

private:

    static DepthType lmr_reduction[MAX_REDUCTION_DEPTH];
    static DepthType razoring_reduction[MAX_REDUCTION_DEPTH];

    unsigned long long int _allowedThinkTime;

};

