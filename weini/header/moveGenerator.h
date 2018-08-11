#pragma once

#include "move.h"
#include "stats.h"
#include "timeman.h"

#include <list>
#include <set>

#include <map>

class Position;

class MoveGenerator {

public:

    MoveGenerator(bool trusted = true, bool onlyCap = false, bool withStats = false):_trusted(trusted),_withStats(withStats),_onlyCap(onlyCap){}

    struct GeneratorStats{

        GeneratorStats():
            createdNodesAllTime(0ull),validNodesAllTime(0ull),
            createdNodes(0ull),pseudoNodes(0ull),validNodes(0ull),captureNodes(0ull),
            epNodes(0ull),promotionNodes(0ull),checkNodes(0ull),checkMateNodes(0ull)
        {}

        Stats::AtomicCounter createdNodesAllTime;
        Stats::AtomicCounter validNodesAllTime;
        Stats::AtomicCounter createdNodes;
        Stats::AtomicCounter pseudoNodes;
        Stats::AtomicCounter validNodes;
        Stats::AtomicCounter captureNodes;
        Stats::AtomicCounter epNodes;
        Stats::AtomicCounter promotionNodes;
        Stats::AtomicCounter checkNodes;
        Stats::AtomicCounter checkMateNodes;

        void Reset(){
            createdNodes   = 0;
            validNodes     = 0;
            pseudoNodes    = 0;
            captureNodes   = 0;
            epNodes        = 0;
            promotionNodes = 0;
            checkNodes     = 0;
            checkMateNodes = 0;
        }

        void Probe(){
            static TimeMan::TimeMSec start = TimeMan::Now();

            if ( createdNodesAllTime > 1ull ){
                double nodesPerSecond = createdNodesAllTime/(double)(TimeMan::Now()-start)*1000.f;
                LOG(logINFO) << "knps : " << nodesPerSecond/1000 ;
                createdNodesAllTime = 0;
                nodesPerSecond = validNodesAllTime/(double)(double)(TimeMan::Now()-start)*1000.f;
                LOG(logINFO) << "knps (valid) : " << nodesPerSecond/1000 ;
                validNodesAllTime   = 0;
                start = TimeMan::Now();
            }
        }
    };


    void GeneratorPiece  (Position & p, std::map<Piece::eType, FastContainer<Move> > & moves, Stats::NodeCount * nodes = 0);
    void Generator       (Position & p, FastContainer<Move> & moves                         , Stats::NodeCount * nodes = 0);

    void GeneratorSquare       (Position & p, FastContainer<Move> & moves, Square::LightSquare k  , Stats::NodeCount * nodes = 0);
    void GeneratorSquareNoStat (Position & p, FastContainer<Move> & moves, Square::LightSquare k  );

    static GeneratorStats stats;

private:

    bool _trusted;
    bool _withStats;
    bool _onlyCap;

    void GeneratorHelperCachePiece       (Position & p, std::map<Piece::eType, FastContainer<Move> > &moves,  Stats::NodeCount * nodes = 0);
    void GeneratorHelperCachePieceNoStat (Position & p, std::map<Piece::eType, FastContainer<Move> > &moves   );
    void GeneratorHelperCache            (Position & p, FastContainer<Move> & moves,                          Stats::NodeCount * nodes = 0);
    void GeneratorHelperCacheNoStat      (Position & p, FastContainer<Move> & moves                           );
    void GeneratorHelperCacheSquare      (Position & p, FastContainer<Move> & moves, Square::LightSquare k,   Stats::NodeCount * nodes = 0);
    void GeneratorHelperCacheSquareNoStat(Position & p, FastContainer<Move> & moves, Square::LightSquare k);

};
