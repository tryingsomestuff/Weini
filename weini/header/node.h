#pragma once

#include "definitions.h"
#include "util.h"

#include <string>

///@todo Node structure is not used anymore (was used with tree.h). Can be removed...
#define NODE_DEBUG

#ifdef NODE_DEBUG
    struct Node{
        Node(): move(""), movez(0), score(-Definitions::scores.infScore), zhash(0) {}
        Node( const std::string & m, Util::Zobrist::HashType mz, ScoreType s, Util::Zobrist::HashType h) : move(m), movez(mz), score(s), zhash(h) {}

        std::string move;
        Util::Zobrist::HashType zhash;
        Util::Zobrist::HashType movez;
        ScoreType score;

        bool IsValid(){return movez != 0;}

        bool operator==(const Node & n)const{
               return zhash == n.zhash && movez == n.movez;
        }
        bool operator< (const Node & n)const{
               return ( zhash ^ movez ) < ( n.zhash ^ n.movez ) ;
        }
    };
#else
    struct Node{
        Node(): movez(0), score(-Definitions::infScore) {}
        Node( Util::Zobrist::HashType mz, ScoreType s) : movez(mz), score(s) {}

        Util::Zobrist::HashType movez;
        ScoreType score;

        bool IsValid(){return movez != 0;}

        bool operator==(const Node & n)const{
               return movez == n.movez;
        }
        bool operator< (const Node & n)const{
               return movez < n.movez;
        }
    };
#endif

static inline
bool SortNodeListFunctor(const Node & n1, const Node & n2){
   return n1.score < n2.score;
}

inline
std::ostream & operator<<(std::ostream & of, const Node & o){
#ifdef NODE_DEBUG
    of << o.move << ":" << o.score;
#else
    of << o.movez << ":" << o.score;
#endif
    return of;
}
