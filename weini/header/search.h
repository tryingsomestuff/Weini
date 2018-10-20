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
         && !m.IsCastling()
         && !m.IsAdvancedPawnPush(p);
}

inline
bool IsSilentCapture(const Move & m, const Position & p){
      return m.IsCapture()
         && !m.IsCheck()
         && !m.IsAdvancedPawnPush(p);
}

class Searcher : public SearcherBase {
public:

    enum ePruningAndReductionAllowed {
        eRedA_off = 0,
        eRedA_on  = 1
    };

    enum eExtensionAllowed {
        eExtA_off = 0,
        eExtA_on = 1
    };

    Searcher(size_t n = 0) : SearcherBase(n), _allowedThinkTime(777){
        ResetKiller();
        ResetHistory();
        ResetCounter();
    }
    virtual ~Searcher() {}

    void Search() override;

    static void Reset();

    SearchedMove Search(Position &       p,
                        const DepthType  depth,
                        const PlyType    ply,
                        Line *           pv = NULL);

    SearchedMove SearchRoot  (ScoreType                   alpha,
                              ScoreType                   beta,
                              const FastContainer<Move> & moves,
                              Position &                  p,
                              const DepthType             iterativeDeepeningDepth,
                              const PlyType               ply,
                              Line *                      pv = NULL,
                              TimeMan::eTC                timeControl = TimeMan::eTC_on,
                              Searcher::ePruningAndReductionAllowed  allowPruningAndReduction = Searcher::eRedA_on,
                              Searcher::eExtensionAllowed            allowExtension = Searcher::eExtA_on,
                              Move::HashType              skipMove = 0,
                              bool                        trustedMoves = true);

    ScoreType Negamax  (ScoreType       alpha,
                        ScoreType       beta,
                        DepthType       depthleft,
                        Position &      p,
                        NSColor::eColor color,
                        const DepthType iterativeDeepeningDepth,
                        const PlyType   ply,
                        bool            fromPV,
                        Line *          pv = NULL,
                        TimeMan::eTC    timeControl = TimeMan::eTC_on,
                        Searcher::ePruningAndReductionAllowed  allowPruningAndReduction = Searcher::eRedA_on,
                        Searcher::eExtensionAllowed            allowExtension = Searcher::eExtA_on);

    ScoreType NegaQuiesce(ScoreType       alpha,
                          ScoreType       beta,
                          DepthType       depthleft,
                          Position &      p,
                          NSColor::eColor color,
                          const DepthType iterativeDeepeningDepth,
                          const PlyType   ply,
                          bool            fromPV,
                          TimeMan::eTC    timeControl = TimeMan::eTC_on);

    ScoreType Analyze(Position & p, bool lazy = false, bool fromPV = true);

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

    // Killers
    Util::Zobrist::HashType killerMovesWhite[2][MAX_GAME_PLY];
    Util::Zobrist::HashType killerMovesBlack[2][MAX_GAME_PLY];
    void ResetKiller();

    // History
    int searchHistory[15][BOARD_SIZE];
    static const unsigned int historyMin = 0;
    static const unsigned int historyMax = 1800;
    void ResetHistory();

    // Counter
    Util::Zobrist::HashType counterMove[BOARD_SIZE][BOARD_SIZE];
    void ResetCounter();

    // All game stuff
    Game game;

    // Position analyzer
    Analyse analyze;

    static volatile eVerbosity       verbosity;
    static volatile std::atomic_bool stopFlag;

private:

    unsigned long long int _allowedThinkTime;

    static DepthType lmr_reduction     [MAX_REDUCTION_DEPTH];

};

