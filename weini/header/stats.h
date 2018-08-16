#pragma once

#include "definitions.h"
#include "logger.hpp"
#include <atomic>
#include <map>
#include <thread>
#include <assert.h>

namespace TheadingTools {
    std::thread::id ThreadId();
    size_t          IdFromThreadId();
    void            Register(size_t id);
}

/// no mutex in here (even in Reduce !)
/// should be fast ...
template< typename T,int N>
class ThreadCounter {
public :

    ThreadCounter() {
        for (int k = 0; k < N; ++k) {
            t[k] = (T)0;
        }
    }

    ThreadCounter(const T & v) {
        for (int k = 0; k < N; ++k) {
            t[k] = v;
        }
    }

    ThreadCounter<T, N> & operator++() {
        //assert(TheadingTools::IdFromThreadId() < N);
        ++t[TheadingTools::IdFromThreadId()];
        return *this;
    }

    // this one is too slow to be used...
    ThreadCounter<T, N> operator++(int) {
        //assert(TheadingTools::IdFromThreadId() < N);
        T temp = t[TheadingTools::IdFromThreadId()];
        ++t[TheadingTools::IdFromThreadId()];
        return temp;
    }

    ThreadCounter<T, N> & operator=(const ThreadCounter<T,N> & tc) {
        memcpy(&t[0], &(tc.t[0]), N * sizeof(T));
        return *this;
    }

    ThreadCounter<T, N> & operator+=(const ThreadCounter<T,N> & tc) {
        //for (int k = 0; k < N; ++k) {
        t[TheadingTools::IdFromThreadId()] += tc.t[TheadingTools::IdFromThreadId()];
        //}
        return *this;
    }

    ThreadCounter<T, N> & operator=(const T& v) {
        t[TheadingTools::IdFromThreadId()] = v;
        return *this;
    }

    T Reduce()const {
        T sum = 0;
        for (int k = 0; k < N; ++k) {
            sum += t[k];
        }
        return sum;
    }

    std::ostream& operator<<(std::ostream & os) {
        os << Reduce();
        return os;
    }

    /*
    inline bool operator< (const T & rhs) {
        return Reduce() < rhs
    }

    inline bool operator> (const T & rhs) {
        return rhs < Reduce();
    }

    inline bool operator<=(const T & rhs) {
        return Reduce() <= rhs;
    }

    inline bool operator>=(const T & rhs) {
        return Reduce() >= rhs;
    }
    */

    inline T operator()() {
        return Reduce();
    }

    inline operator T() const {
        return Reduce();
    }

    inline T load() const { // iso std::atomic
        return t[TheadingTools::IdFromThreadId()];
    }

    inline explicit operator float() const {
        return (float)Reduce();
    }

    inline ThreadCounter<T, N>& operator+=(const T & v) {
        t[TheadingTools::IdFromThreadId()] += v;
        return *this;
    }

private:

    T t[N];

};

class Stats{
   public:

   template < typename T >
   class CounterULL{
      template < typename U>
      friend std::ostream & operator<<(std::ostream & of, const CounterULL<U> & c);
   public:
      CounterULL():counter(0){}
      CounterULL(const T & t):counter(T(t)){}

      CounterULL & operator=(const T& t){ counter = t; return *this;}

      CounterULL & operator+=(const T& t){ counter += t; return *this;}
      CounterULL & operator-=(const T& t){ counter -= t; return *this;}
      CounterULL & operator*=(const T& t){ counter *= t; return *this;}
      CounterULL & operator/=(const T& t){ counter /= t; return *this;}

      CounterULL operator+(const CounterULL& t){ return T(counter+t.counter);}
      CounterULL operator-(const CounterULL& t){ return T(counter-t.counter);}
      CounterULL operator/(const CounterULL& t){ return T(counter/t.counter);}
      CounterULL operator*(const CounterULL& t){ return T(counter*t.counter);}

      template < typename U >
      CounterULL operator+(const U& u){ return T(counter+u);}
      template < typename U >
      CounterULL operator-(const U& u){ return T(counter-u);}
      template < typename U >
      CounterULL operator*(const U& u){ return T(counter*u);}
      template < typename U >
      CounterULL operator/(const U& u){ return T(counter/u);}

      template < typename U >
      bool operator>(const U& u){ return counter > u;}
      template < typename U >
      bool operator<(const U& u){ return counter < u;}
      template < typename U >
      bool operator==(const U& u){ return counter == (T)u;}

      CounterULL & operator++(){ ++counter; return *this;}

      const T & load()const{ return counter;}
      T & operator()()const{ return counter;}

      template < typename U >
      operator U(){ return U(counter);}

   private:
      T counter;
   };

   typedef CounterULL<unsigned long long int> AtomicCounter; // not thread safe
   //typedef std::atomic<unsigned long long> AtomicCounter; // too much spinlock !!!
   //typedef ThreadCounter<unsigned long long, 16> AtomicCounter; // too slow

   static void CutCount();

   class NodeCount{
   public:
     NodeCount():
         createdNodes(0ull),
         pseudoNodes(0ull),
         validNodes(0ull),
         captureNodes(0ull),
         epNodes(0ull),
         promotionNodes(0ull),
         checkNodes(0ull),
         checkMateNodes(0ull){
     }

     NodeCount & operator =(const NodeCount & c){
         createdNodes   = c.createdNodes.load();
         pseudoNodes    = c.pseudoNodes.load();
         validNodes     = c.validNodes.load();
         captureNodes   = c.captureNodes.load();
         epNodes        = c.epNodes.load();
         promotionNodes = c.promotionNodes.load();
         checkNodes     = c.checkNodes.load();
         checkMateNodes = c.checkMateNodes.load();
         return *this;
     }

     NodeCount(const NodeCount & c){
         createdNodes   = c.createdNodes.load();
         pseudoNodes    = c.pseudoNodes.load();
         validNodes     = c.validNodes.load();
         captureNodes   = c.captureNodes.load();
         epNodes        = c.epNodes.load();
         promotionNodes = c.promotionNodes.load();
         checkNodes     = c.checkNodes.load();
         checkMateNodes = c.checkMateNodes.load();
     }

     void Print();
     void Reset();

     Stats::AtomicCounter createdNodes;
     Stats::AtomicCounter pseudoNodes;
     Stats::AtomicCounter validNodes;
     Stats::AtomicCounter captureNodes;
     Stats::AtomicCounter epNodes;
     Stats::AtomicCounter promotionNodes;
     Stats::AtomicCounter checkNodes;
     Stats::AtomicCounter checkMateNodes;
   };

   static void Init();

   static ScoreType previousScore[2];

   static AtomicCounter alphacut               ;
   static AtomicCounter betacut                ;
   static AtomicCounter betacutroot            ;
   static AtomicCounter qalphacut              ;
   static AtomicCounter qbetacut               ;
   static AtomicCounter qearlyalphacut         ;
   static AtomicCounter qearlybetacut          ;
   static AtomicCounter qearlyalphadeltacut    ;
   static AtomicCounter qearlybetadeltacut     ;
   static AtomicCounter deltaalphacut          ;
   static AtomicCounter deltabetacut           ;
   static AtomicCounter seecut                 ;
   static AtomicCounter seecutAlphaBeta        ;
   static AtomicCounter nullmoveverification   ;
   static AtomicCounter nullmovecutafterverification;
   static AtomicCounter nullmovecutbeta        ;
   static AtomicCounter nullmovecuttry         ;
   static AtomicCounter nullmovecutskipped     ;
   static AtomicCounter nullmoveReset          ;
   static AtomicCounter razoringbetacut        ;
   static AtomicCounter razoringbetacutQtry    ;
   static AtomicCounter razoringbetacutQ       ;
   static AtomicCounter razoringbetacutH       ;
   static AtomicCounter staticNullMovebetacut  ;
   static AtomicCounter staticNullMovebetacutQtry;
   static AtomicCounter staticNullMovebetacutQ ;
   static AtomicCounter staticNullMovebetacutH ;
   static AtomicCounter failedReduction        ;
   static AtomicCounter futilitycut            ;
   static AtomicCounter qfutilitycut           ;
   static AtomicCounter futilityMovePruned     ;
   static AtomicCounter moveCountPruned        ;
   static AtomicCounter lmralphabeta           ;
   static AtomicCounter lmralphabetafail       ;
   static AtomicCounter windowfails            ;
   static AtomicCounter pvsRootfails           ;
   static AtomicCounter pvsAlphaBetafails      ;
   static AtomicCounter windowsuccess          ;
   static AtomicCounter pvsRootsuccess         ;
   static AtomicCounter pvsAlphaBetasuccess    ;
   static AtomicCounter lmrsearch              ;
   static AtomicCounter lmrsearchfail          ;
   static AtomicCounter ttARHitExact           ;
   static AtomicCounter ttARHitAlpha           ;
   static AtomicCounter ttARHitBeta            ;
   static AtomicCounter ttARHitAlphaOver       ;
   static AtomicCounter ttARHitBetaOver        ;
   static AtomicCounter ttARHitUsed            ;
   static AtomicCounter ttARCollision          ;
   static AtomicCounter ttAllHit               ;
   static AtomicCounter ttHitExact             ;
   static AtomicCounter ttHitAlpha             ;
   static AtomicCounter ttHitBeta              ;
   static AtomicCounter ttHitAlphaOver         ;
   static AtomicCounter ttHitBetaOver          ;
   static AtomicCounter ttHitUsed              ;
   static AtomicCounter ttCollision            ;
   static AtomicCounter ttARQHitExact          ;
   static AtomicCounter ttARQHitAlpha          ;
   static AtomicCounter ttARQHitBeta           ;
   static AtomicCounter ttARQHitAlphaOver      ;
   static AtomicCounter ttARQHitBetaOver       ;
   static AtomicCounter ttARQHitUsed           ;
   static AtomicCounter ttARQCollision         ;
   static AtomicCounter ttQAllHit              ;
   static AtomicCounter ttQHitExact            ;
   static AtomicCounter ttQHitAlpha            ;
   static AtomicCounter ttQHitBeta             ;
   static AtomicCounter ttQHitAlphaOver        ;
   static AtomicCounter ttQHitBetaOver         ;
   static AtomicCounter ttQHitUsed             ;
   static AtomicCounter ttQCollision           ;
   static AtomicCounter ttClear                ;
   static AtomicCounter ttQClear               ;
   static AtomicCounter ttEClear               ;
   static AtomicCounter ttELClear              ;
   static AtomicCounter ttEPClear              ;
   static AtomicCounter leafNodes              ;
   static AtomicCounter visitednodes           ;
   static AtomicCounter visitedqnodes          ;
   static AtomicCounter visitedrealqnodes      ;
   static AtomicCounter analysednodes          ;
   static AtomicCounter analysedlazynodes      ;
   static AtomicCounter ttEvalLazyHits         ;
   static AtomicCounter ttEvalHits             ;
   static AtomicCounter ttEvalPawnHits         ;
   static AtomicCounter ttEvalLazyCollision    ;
   static AtomicCounter ttEvalCollision        ;
   static AtomicCounter ttEvalPawnCollision    ;
   static AtomicCounter terminalnodes          ;
   static AtomicCounter openingBookHits        ;
   static AtomicCounter ttsort                 ;
   static AtomicCounter ttsorttry              ;
   static AtomicCounter ttQsort                ;
   static AtomicCounter ttQsorttry             ;
   static AtomicCounter pvsort                 ;
   static AtomicCounter pvsorttry              ;
   static AtomicCounter pvrootsort             ;
   static AtomicCounter pvrootsorttry          ;
   static AtomicCounter iidsort                ;
   static AtomicCounter probCutTry             ;
   static AtomicCounter probCutSuccess         ;

   static AtomicCounter positionCopy           ;

   static AtomicCounter currentEveryNodesCount ;
   static AtomicCounter currentPseudoNodesCount;
   static AtomicCounter currentNodesCount      ;
   static AtomicCounter currentqNodesCount     ;
   static AtomicCounter currentMaxDepth        ;

   static AtomicCounter countCheckExtensionRoot              ;
   static AtomicCounter countCheckExtensionSearch            ;
   static AtomicCounter countCheckExtensionAlphaBeta         ;
   static AtomicCounter countEndGameExtensionSearch          ;
   static AtomicCounter countVeryEndGameExtensionSearch      ;
   static AtomicCounter countPVExtensionSearch               ;
   static AtomicCounter countSingleReplyExtensionSearch      ;
   static AtomicCounter countSingleReplyExtensionAlphaBeta   ;
   static AtomicCounter countSingularExtensionAlphaBeta      ;
   static AtomicCounter countNearPromotionExtensionSearch    ;
   static AtomicCounter countNearPromotionExtensionAlphaBeta ;
   static AtomicCounter countReCaptureExtensionSearch        ;
   static AtomicCounter countReCaptureExtensionAlphaBeta     ;


   static AtomicCounter nodesCountByDepth[MAX_SEARCH_DEPTH];
   static AtomicCounter qnodesCountByDepth[MAX_SEARCH_DEPTH];

   static AtomicCounter minseldepth[MAX_SEARCH_DEPTH];
   static AtomicCounter maxseldepth[MAX_SEARCH_DEPTH];
   static AtomicCounter meanseldepth[MAX_SEARCH_DEPTH];
   static AtomicCounter meanseldepthcount[MAX_SEARCH_DEPTH];

   static AtomicCounter tcEBFAbort;
   static AtomicCounter tcForcedAbort;
   static AtomicCounter tcIdealAbort;

};

template < typename T >
std::ostream & operator<<(std::ostream & of, const Stats::CounterULL<T> & c){
    of << c.counter;
    return of;
}

template < typename U, typename T >
U operator/(const U u, const Stats::CounterULL<T>& t){ return u/t.load();}

template < typename U,typename T >
U operator*(const U u, const Stats::CounterULL<T>& t){ return u*t.load();}

template < typename U,typename T >
U operator-(const U u, const Stats::CounterULL<T>& t){ return u-t.load();}

template < typename U,typename T >
U operator+(const U u, const Stats::CounterULL<T>& t){ return u+t.load();}
