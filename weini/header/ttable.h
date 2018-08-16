#pragma once

#include "definitions.h"
#include "fastContainer.h"
#include "move.h"
#include "util.h"

#include <map>
#include <mutex>
#include <unordered_map>

class Searcher;

//#define DEBUG_TTHASH

//#define DEBUG_TT_SORT_FAIL

#ifdef DEBUG_TTHASH
#define PHASH(p) p.GetFENShort2() // short fen as hash
#else
#define PHASH(p) p.GetZHash()     // standard zobrist hash
#endif

#ifdef DEBUG_TTHASH
   #define TTHASHTYPE std::string
#else
   #define TTHASHTYPE Util::Zobrist::HashType
#endif

#define TTMOVETYPE Move::HashType

//#define DEBUG_PV_TT
//#define DEBUG_PV_TT_A_LOT

struct Bucket;

#pragma pack(push, 2)
struct Transposition{

    friend void UpdateAge_(Bucket * e, unsigned long long int n);
    friend void Clear_(Bucket * e, unsigned long long int n);

    enum TT_Type : unsigned char{
        tt_exact = 0,
        tt_alpha = 1,
        tt_beta  = 2
    };

    enum TT_Aging : unsigned char {
        ta_new    = 0,
        ta_old    = 1,
        ta_elder  = 2,
        ta_max    = 2
    };

    Transposition():
        depth(0),
        score(0),
        t_type(tt_exact),
        hash(0),
        move(0),
        age(ta_new)
    {}

    Transposition(DepthType d,
                  ScoreType s,
                  TT_Type t,
                  const TTMOVETYPE & m,
                  Util::Zobrist::HashType h,
                  bool whiteIsMax):
        depth(d),
        score(s),
        t_type(t),
        move(m),
        hash(h),
        age(ta_new)
    {}

    static void InitTT();
    static void InitTTQ();

    static void InsertTT (const TTHASHTYPE &     zhash,
                          ScoreType              score,
                          DepthType              depth,
                          Transposition::TT_Type t_type,
                          const TTMOVETYPE &     m
#ifdef DEBUG_TT_SORT_FAIL
                         ,const std::string &    fen
#endif
                         );

    static bool GetTT    (const TTHASHTYPE & zhash,
                          DepthType          mindepth,
                          ScoreType          alpha,
                          ScoreType          beta,
                          Transposition &    tt);

    static void InsertTTQ(const TTHASHTYPE &     zhash,
        ScoreType              score,
        DepthType              depth,
        Transposition::TT_Type t_type,
        const TTMOVETYPE &     m
#ifdef DEBUG_TT_SORT_FAIL
        , const std::string &    fen
#endif
                         );

    static bool GetTTQ   (const TTHASHTYPE & zhash,
                          DepthType          mindepth,
                          ScoreType          alpha,
                          ScoreType          beta,
                          Transposition &    tt);

    static bool Sort(const Transposition & ttt, FastContainer<Move> & moves, const Position & p);

    static void UpdateAge();
    static void UpdateAgeQ();

    static void ClearTT();
    static void ClearTTQ();

    static
    std::string GetPV(const Position & pp, int depth, bool abr = false);

    Transposition & operator=(const Transposition & t){
        depth = t.depth;
        t_type  = t.t_type;
        hash  = t.hash;
        age   = t.age;
        move  = t.move;
        score = t.score;
#ifdef DEBUG_TT_SORT_FAIL
        fen   = t.fen;
#endif
        if ( hash == 0 ) {
           LOG(logFATAL) << "0 hash in =";
        }
        return *this;
    }

    Transposition & operator=(const Transposition && t){
        depth = t.depth;
        t_type= t.t_type;
        hash  = t.hash;
        age   = t.age;
        move  = t.move;
        score = t.score;
#ifdef DEBUG_TT_SORT_FAIL
        fen   = t.fen;
#endif
        if ( hash == 0 ) {
           LOG(logFATAL) << "0 hash in =";
        }
        return *this;
    }

    DepthType          depth;
    TT_Type            t_type;
    TTHASHTYPE         hash;
    TT_Aging           age;
    TTMOVETYPE         move;
    ScoreType          score;
#ifdef DEBUG_TT_SORT_FAIL
    std::string        fen;
#endif

};
#pragma pack(pop)

struct Bucket{
    Transposition t[2];

    Bucket & operator=(const Bucket & b){
        t[0] = b.t[0];
        t[1] = b.t[1];
        return *this;
    }

    Bucket & operator=(const Bucket && b){
        t[0] = b.t[0];
        t[1] = b.t[1];
        return *this;
    }
};

