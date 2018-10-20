#include "CLI.h"

#include "sort.h"
#include "line.h"
#include "UtilPosition.h"
#include "stats.h"

#include <sstream>
#include <fstream>
#include <iterator>
#include <thread>
#include <chrono>
#include <math.h>

void GeneratorWrapperLoop(std::vector<Stats::NodeCount> & nodesV,
                          std::vector<NodeStatType> & perft,
                          Position & p,
                          const FastContainer<Move> & moves,
                          DepthType currentDepth,
                          bool display,
                          bool pseudolegal){

   int g = 0;
   for (auto it = moves.begin() ; it != moves.end() ; ++it){
      if ( display ) {
         LOG(logINFO) <<  perft.size()-2-currentDepth << "         " <<  (*it).Show();
      }
      Position p2(p);
      // No check on move !
      p2.ApplyMove(*it);
      FastContainer<Move> moves2;
      //ScopePoolObject<FastContainer<Move>, _default_block_size> moves_;
      //FastContainer<Move> & moves2 = moves_.Get();
      MoveGenerator(!pseudolegal,false,true).Generator(p2,moves2,&nodesV[perft.size()-1-currentDepth]);
      g += (int)moves2.size();
      if ( currentDepth > 0 ) GeneratorWrapperLoop(nodesV,perft,p2,moves2,currentDepth-1,display,pseudolegal);
   }

   perft[perft.size()-1-currentDepth] += g;
}

std::vector<NodeStatType> GeneratorWrapper(const std::string & fen,
                                            DepthType depth,
                                            bool display,
                                            bool pseudolegal){
  Position p(fen);

  if (display) p.Display();

  std::vector<NodeStatType> ret(depth+1,0);
  std::vector<Stats::NodeCount> nodesV(depth+1,Stats::NodeCount());
  FastContainer<Move> moves;
  MoveGenerator(!pseudolegal, false, true).Generator(p,moves,&nodesV[0]);
  ret[0] = moves.size();
  if ( depth > 0){
     GeneratorWrapperLoop(nodesV,ret,p,moves,depth-1,display,pseudolegal);
  }

  int k = 0;
  for(auto it = ret.begin() ; it != ret.end() ; ++it,++k){
      LOG(logINFO) << "=== Generation " << k  << " " << *it;
      nodesV[k].Print();
  }

  return ret;

}

bool TestGenerator(const std::string & fen,
                   std::vector<NodeStatType> expected,
                   DepthType depth,
                   bool display,
                   bool pseudolegal){

  LOG(logINFO) << "Testing generator for " << fen << " depth " << (int)depth;

  MoveGenerator::stats.Probe();

  std::vector<NodeStatType> l = GeneratorWrapper(fen,depth,display,pseudolegal);

  MoveGenerator::stats.Probe();

  bool ok = true;

  for(DepthType k = 0 ; k < depth && k < (DepthType)expected.size(); ++ k){
    if ( expected[k] != l[k]){
      LOG(logFAIL) << "Error for generator, expected " << expected[k] << " got " << l[k];
      ok = false;
    }
  }

  if ( ok && !expected.empty()){
    LOG(logINFO) << "=> OK";
  }

  if ( expected.empty() ){
    LOG(logWARNING) << "No expected given";
  }

  return ok;

}

bool TestThreats(const std::string & fen,
                 const std::string & square,
                 std::vector<Square> expected,
                 bool display){
    Square sq(square);
    Position p(fen);

    LOG(logINFO) << "Testing threats for " << fen << " on square " << square;

    if ( display ) p.Display();
    FastContainer<Square::LightSquare> threats;
    UtilMove::GetThreads(p,sq.index(),false,&threats);
    //if ( display ){
      if ( threats.empty()){
         LOG(logINFO) << "No threats for " << sq.position() << " from " << (p.WhiteToPlay()? "black":"white");
      }
      else{
         LOG(logINFO) << "Threats for " << sq.position() << " from " << (p.WhiteToPlay()? "black":"white");
         for(auto it = threats.begin() ; it != threats.end() ; ++it){
             LOG(logINFO) << Square(*it).position();
         }
      }
    //}

    bool ok = true;

    if ( expected.size() != threats.size() ){
      ok = false;
      LOG(logFAIL) << "Number of threats differ : " << expected.size() << " != " << threats.size();
    }

    for ( size_t k = 0 ; k < expected.size() ; ++k){
        if ( std::find(threats.begin(), threats.end(), expected[k].index()) == threats.end()){
           ok = false;
        }
    }

    if ( ok){
      LOG(logINFO) << "=> OK";
    }
    else{
      LOG(logFAIL) << "=> BAD";
    }

    return ok;
}

bool TestMoveHash(){
    Position p("start");
    {
        std::cout << "**********************" << std::endl;
        Move m("g8 h8", p.Turn());
        std::cout << m.Show() << std::endl;
        std::cout << "from " << (int)m.From() << std::endl;
        std::cout << "to   " << (int)m.To() << std::endl;
        std::cout << "type " << (int)m.Type() << std::endl;
        Move::HashType h = m.ZHash();
        std::cout << h << std::endl;
        std::cout << std::hex << (m.From() << 10) << std::endl;
        std::cout << std::hex << (m.To() << 4) << std::endl;
        std::cout << std::hex << (m.Type()) << std::endl;
        std::cout << std::hex << h << std::endl;
        MiniMove mini = Move::HashToMini(h);
        std::cout << "h from " << (int)Move::Hash2From(h) << std::endl;
        std::cout << "h to   " << (int)Move::Hash2To(h) << std::endl;
        std::cout << "h type " << (int)Move::Hash2Type(h) << std::endl;
        Move m2(mini);
        std::cout << m2.Show() << std::endl;
        if (m2.Show() != m.Show()) {
            return false;
        }
    }
    {
        std::cout << "**********************" << std::endl;
        Move m("0-0-0", p.Turn());
        std::cout << m.Show() << std::endl;
        std::cout << "from " << (int)m.From() << std::endl;
        std::cout << "to   " << (int)m.To() << std::endl;
        std::cout << "type " << (int)m.Type() << std::endl;
        Move::HashType h = m.ZHash();
        std::cout << h << std::endl;
        std::cout << std::hex << (m.From() << 10) << std::endl;
        std::cout << std::hex << (m.To() << 4) << std::endl;
        std::cout << std::hex << (m.Type()) << std::endl;
        std::cout << std::hex << h << std::endl;
        MiniMove mini = Move::HashToMini(h);
        std::cout << "h from " << (int)Move::Hash2From(h) << std::endl;
        std::cout << "h to   " << (int)Move::Hash2To(h) << std::endl;
        std::cout << "h type " << (int)Move::Hash2Type(h) << std::endl;
        Move m2(mini);
        std::cout << m2.Show() << std::endl;
        if (m2.Show() != m.Show()) {
            return false;
        }
    }
    return true;
}

ScoreType TestStaticAnalysis(const std::string & fen){
    Position pAnal(fen);
    pAnal.Display();
    Analyse::ActivateAnalysisDisplay(true);
    return Analyse().Run(pAnal,1.f);
}

SearcherBase::SearchedMove TestAnalysis(Position & pAnal, DepthType depth, Searcher::eVerbosity verbosity){
    //if ( display) pAnal.Display();
    SearcherBase::Data _data(pAnal,depth,verbosity);
    TimeMan::Instance().Tic();
    if (verbosity >= Searcher::eVerb_verbose) LOG(logINFO) << "Next search time " << Definitions::timeControl.currentMoveMs;
    SearcherBase::SearchedMove best = ThreadPool<Searcher>::Instance().SearchSync(_data);
    //if ( display ) pAnal.Display();
    if (verbosity >= Searcher::eVerb_verbose) {
        LOG(logINFO) << "Best move is " << SearcherBase::GetMove(best).Show()
            << " with score " << (pAnal.Turn() == NSColor::c_white ? +1 : -1) * SearcherBase::GetScore(best)
            << " (depth " << (int)SearcherBase::GetDepth(best) << "/" << (int)depth << ")";
       Line pv = ThreadPool<Searcher>::Instance().Main().GetData().pv;
       if ( pv.n > 0 ){
          LOG(logINFO) << "PV:     " << pv.GetPV(pAnal,true);
       }
    }
    // Getting the PV using TT requieres games hashes to check for draw
    if (verbosity >= Searcher::eVerb_verbose) Stats::CutCount();
    return best;
}

SearcherBase::SearchedMove TestAnalysis(const std::string & fen, DepthType depth, Searcher::eVerbosity verbosity){
    Position pAnal(fen);
    pAnal.Display();
    return TestAnalysis(pAnal,depth,verbosity);
}

namespace Texel {

    class TexelInput {
    public:
        TexelInput(Position * p, int r) :p(p), result(r) {}
        ~TexelInput() {}
        Position * p;
        int result;
        TexelInput(const TexelInput & t) {
            p = t.p;
            result = t.result;
        }
        TexelInput & operator =(const TexelInput & t) {
            p = t.p;
            result = t.result;
            return *this;
        }
        TexelInput & operator =(TexelInput && t) {
            p = t.p;
            result = t.result;
            return *this;
        }
    };

    template < typename T >
    class TexelParam {
    public:
        TexelParam(const std::string & a, const T& inf, const T& sup) :accessor(a), inf(inf), sup(sup) {}
        std::string accessor;
        T inf;
        T sup;
        T& operator()() {
            return Definitions::GetValue<T>(accessor);
        }
        void Set(const T & value) {
            Definitions::SetValue(accessor, std::to_string(value));
        }
    };

    template<class Iter >
    Iter random_unique(Iter begin, Iter end, size_t num_random) {
        size_t left = std::distance(begin, end);
        while (num_random--) {
            Iter r = begin;
            std::advance(r, rand() % left);
            std::swap(*begin, *r);
            ++begin;
            --left;
        }
        return begin;
    }

    double Sigmoid(Position * p) {
        static Searcher searcher;
        const double s = searcher.NegaQuiesce(-Definitions::scores.infScore,Definitions::scores.infScore,50,*p,p->Turn(),0,0,true,TimeMan::eTC_off);
        static const double K = 0.75;
        return 1. / (1. + std::pow(10, -K * s / 400.));
    }

    double E(const std::vector<Texel::TexelInput> &data, size_t miniBatchSize) {
        double e = 0;
        for (size_t k = 0; k < miniBatchSize; ++k) {
            e += std::pow(double((data[k].result+1)*0.5 - Sigmoid(data[k].p)),2);
        }
        e /= data.size();
        return e;
    }

    void Randomize(std::vector<Texel::TexelInput> & data, size_t miniBatchSize) {
        random_unique(data.begin(), data.end(), miniBatchSize);
    }

    std::vector<double> ComputeGradient(std::vector<TexelParam<ScoreType> > x0, std::vector<Texel::TexelInput> &data, size_t gradientBatchSize) {
        LOG(logINFO) << "Computing gradient";
        //Randomize(data, gradientBatchSize);
        std::vector<double> g;
        const ScoreType delta = 1;
        for (size_t k = 0; k < x0.size(); ++k) {
            LOG(logINFO) << "... " << k;
            double grad = 0;
            ScoreType oldvalue = x0[k]();
            x0[k].Set(oldvalue + delta);
            grad = E(data, gradientBatchSize);
            x0[k].Set(oldvalue - delta);
            grad += E(data, gradientBatchSize);
            x0[k].Set(oldvalue);
            grad -= 2*E(data, gradientBatchSize);
            g.push_back(grad/(2*delta));
            LOG(logINFO) << "Gradient " << k << " " << grad;
        }
        double norm = 0;
        for (size_t k = 0; k < x0.size(); ++k) {
            norm += g[k] * g[k];
        }
        norm = sqrt(norm);
        for (size_t k = 0; k < x0.size(); ++k) {
            g[k] /= norm;
            LOG(logINFO) << "Gradient normalized " << k << " " << g[k];
        }
        return g;
    }

    std::vector<TexelParam<ScoreType> > TexelOptimizeLocalMin(const std::vector<TexelParam<ScoreType> >& initialGuess, std::vector<Texel::TexelInput> &data) {

        Definitions::ttConfig.do_transpositionTableEval = false;
        Definitions::ttConfig.do_transpositionTableEvalPawn = false;
        Definitions::ttConfig.do_transpositionTableQuiesce = false;

        const size_t nParams = initialGuess.size();
        double bestE = E(data,data.size());
        std::vector<TexelParam<ScoreType> > bestParValues = initialGuess;
        bool improved = true;
        while ( improved ) {
            improved = false;
            for (int pi = 0; pi < nParams; pi++) {
                std::vector<TexelParam<ScoreType> > newParValues = bestParValues;
                ScoreType oldvalue = newParValues[pi]();
                ///@todo if not sup
                newParValues[pi].Set(oldvalue+1);
                double newE = E(data,data.size());
                LOG(logINFO) << "Best E " << bestE << "    newE " << newE;
                if (newE < bestE) {
                    bestE = newE;
                    bestParValues = newParValues;
                    improved = true;
                }
                else {
                    ///@todo if not inf
                    newParValues[pi].Set(oldvalue-1);
                    newE = E(data,data.size());
                    LOG(logINFO) << "Best E " << bestE << "    newE " << newE;
                    if (newE < bestE) {
                        bestE = newE;
                        bestParValues = newParValues;
                        improved = true;
                    }
                }
            }
            LOG(logINFO) << "Current values :";
            for (size_t k = 0; k < bestParValues.size(); ++k) {
                LOG(logINFO) << bestParValues[k].accessor << " " << bestParValues[k]();
            }

        }
        return bestParValues;
    }

    std::vector<TexelParam<ScoreType> > TexelOptimizeGD(const std::vector<TexelParam<ScoreType> >& initialGuess, std::vector<Texel::TexelInput> &data) {

        Definitions::ttConfig.do_transpositionTableEval = false;
        Definitions::ttConfig.do_transpositionTableEvalPawn = false;
        Definitions::ttConfig.do_transpositionTableQuiesce = false;

        Definitions::SetValue("pawnValue", std::to_string(100));
        Definitions::SetValue("knightValue", std::to_string(1000));
        Definitions::SetValue("bishopValue", std::to_string(1000));
        Definitions::SetValue("rookValue", std::to_string(1000));
        Definitions::SetValue("queenValue", std::to_string(1000));

        size_t errorBatchSize = 1024 * 32; // data.size();
        Randomize(data, errorBatchSize);

        //LOG(logINFO) << "Computing initial error";
        //double bestE = E(data, errorBatchSize);

        std::vector<TexelParam<ScoreType> > bestParam = initialGuess;
        bool improved = true;

        while (improved) {
            improved = false;

            std::vector<double> g = ComputeGradient(bestParam, data, errorBatchSize);

            double delta = 50;

            LOG(logINFO) << "Line search";

            double curE = E(data, errorBatchSize);
            double bestELS = curE;
            while (delta >= 2) {
                LOG(logINFO) << "Applying gradient, delta = " << delta;
                ///@todo check inf/sup
                for (size_t k = 0; k < bestParam.size(); ++k) {
                    ScoreType oldValue = bestParam[k]();
                    bestParam[k].Set(oldValue - ScoreType(delta * g[k]));
                }
                LOG(logINFO) << "Computing new error";
                curE = E(data, errorBatchSize);

                LOG(logINFO) << "LS : " << delta << " " << curE << " best was : " << bestELS;

                if (curE <= bestELS) {
                    improved = true;
                    bestELS = curE;
                    break;
                }
                else {
                    // reseting last iteration
                    for (size_t k = 0; k < bestParam.size(); ++k) {
                        ScoreType oldValue = bestParam[k]();
                        bestParam[k].Set(oldValue + ScoreType(delta * g[k]));
                    }
                    //break;
                }
                delta /= 2;
            }

            // randomize for next iteration
            Randomize(data, errorBatchSize);

            LOG(logINFO) << "Current values :";
            for (size_t k = 0; k < bestParam.size(); ++k) {
                LOG(logINFO) << bestParam[k].accessor << " " << bestParam[k]();
            }

        }
        return bestParam;
    }


}

void TexelTuning(const std::string & filename) {
    std::ifstream stream(filename);
    std::string line;

    std::vector<Texel::TexelInput> data;

    LOG(logINFO) << "Running texel tuning with file " << filename;

    int count = 0;
    while (std::getline(stream, line)){
        //std::cout << line << std::endl;
        nlohmann::json o;
        try {
            o = nlohmann::json::parse(line);
        }
        catch (...) {
            LOG(logFATAL) << "Cannot parse json " << line;
        }
        //std::cout << o << std::endl;
        std::string fen = o["fen"];
        Position * p = new Position(fen);
        if (std::abs(o["result"].get<int>()) < 800) {
            data.push_back(Texel::TexelInput(p, o["result"]));
        }
        ++count;
        if (count % 10000 == 0) {
            LOG(logINFO) << count << " position read";
        }
    }

    LOG(logINFO) << "Data size : " << data.size();

    std::vector<Texel::TexelParam<ScoreType> > guess;
    guess.push_back(Texel::TexelParam<ScoreType>("knightValue", 100, 2000));
    guess.push_back(Texel::TexelParam<ScoreType>("bishopValue", 100, 2000));
    guess.push_back(Texel::TexelParam<ScoreType>("rookValue", 100, 2000));
    guess.push_back(Texel::TexelParam<ScoreType>("queenValue", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("badBishopMalus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("bishopPairBonus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("knightPairMalus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("rookPairMalus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("isolatedPawnMalus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("doublePawnMalus", 100, 2000));
    //guess.push_back(Texel::TexelParam<ScoreType>("bonusCandidatePassedPawn", 100, 2000));

    LOG(logINFO) << "Initial values :";
    for (size_t k = 0; k < guess.size(); ++k) {
        LOG(logINFO) << guess[k].accessor << " " << guess[k]();
    }

    std::vector<Texel::TexelParam<ScoreType> > optim = Texel::TexelOptimizeGD(guess, data);

    LOG(logINFO) << "Optimized values :";
    for (size_t k = 0; k < optim.size(); ++k) {
        LOG(logINFO) << optim[k].accessor << " " << optim[k]();
    }

    for (size_t k = 0; k < data.size(); ++k) {
        delete data[k].p;
    }
}

void TestMoveOrdering(const std::string & fen){
   Position p(fen);

   p.Display();

   FastContainer<Move> moves;
   MoveGenerator().Generator(p,moves);
   UtilMove::DisplayList(moves,"Unsorted move list");
   Searcher s;
   Sort::SortMoves(moves,p,s,Sort::SP_all);
   UtilMove::DisplayList(moves,"Sorted move list");

}

void TestSEE(const std::string & fen, const std::string & square, ScoreType threshold){

    Position p(fen);

    p.Display();

    Square sq(square);
    LOG(logINFO) << "SSE at square " << sq.position();
    LOG(logINFO) << "Threshold value " << threshold;
    p.SwitchPlayer();
    FastContainer<Square::LightSquare> threats;
    UtilMove::GetThreads(p, sq.index(), false, &threats);
    p.SwitchPlayer();
    Move m(threats[0], sq.index());
    m.Validate(p, true, true);
    LOG(logINFO) << "SSE " << (Searcher::SEE(p,m,threshold)?"true":"false");
}

#define GETHASH \
h1 = p.GetZHash(); //\
//h1w = p.GetZHash(NSColor::c_white); \
//h1b = p.GetZHash(NSColor::c_black);

#define COMPAREHASH \
h2 = p.GetZHash(); \
if (h1  != h2 ) { LOG(logFAIL) << "bad hash  " << h1  << " " << h2  << " " << p.GetFEN(); }
//h2w = p.GetZHash(NSColor::c_white); \
//h2b = p.GetZHash(NSColor::c_black); \
//if (h1w != h2w) { LOG(logFAIL) << "bad whash " << h1w << " " << h2w << " " << p.GetFEN(); } \
//if (h1b != h2b) { LOG(logFAIL) << "bad bhash " << h1b << " " << h2b << " " << p.GetFEN(); }



void TestGameZHash(){

  Position p("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");

  /*
  LOG(logINFO) << Util::Count(p) ;

  LOG(logINFO) << (p.WhiteToPlay()?"White":"Black") << " to play" ;

  LOG(logINFO) << p.Info().white.CastlingRight() ;
  LOG(logINFO) << p.Info().black.CastlingRight() ;

  Square ep(p.Info().enPassant);
  if ( ep.IsValid()){
    LOG(logINFO) << ep.position();
    LOG(logINFO) << (int)ep.file();
    LOG(logINFO) << ep.fileStr();
    LOG(logINFO) << (int)ep.rank() ;
  }
  else{
    LOG(logINFO) << "No current en passant" ;
  }
  */

  p.Display();

  Util::Zobrist::HashType h1, h2;
  //Util::Zobrist::HashType h1w, h1b, h2w, h2b;

  // should fail "not your turn"
  Move("d5 d4", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // test capture with pawn
  Move("e4 e5", p.Turn()).ValidateAndApplyInPlace(p).Display();

  GETHASH;

  // bishop forward
  Move("c8 d7", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop forward
  Move("f1 e2", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop backward
  Move("d7 c8", p.Turn()).ValidateAndApplyInPlace(p).Display();

  // bishop backward
  Move("e2 f1", p.Turn()).ValidateAndApplyInPlace(p).Display();

  COMPAREHASH;

  p = Position("r3kbnr/pppqpppp/2npb3/8/8/2NBPN2/PPPP1PPP/R1BQK2R w KQkq - 6 5");

  Move("0-0"  , p.Turn()).ValidateAndApplyInPlace(p).Display();
  Move("0-0-0", p.Turn()).ValidateAndApplyInPlace(p).Display();

  GETHASH;

}

void TestGamePlay(){

  Position p("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d5 0 2");

  LOG(logINFO) << Util::Count(p,1) ;

  LOG(logINFO) << (p.WhiteToPlay()?"White":"Black") << " to play" ;

  LOG(logINFO) << p.WhitePlayer().CastlingRight() ;
  LOG(logINFO) << p.BlackPlayer().CastlingRight() ;

  Square ep(p.EnPassant());
  if ( ep.IsValid()){
    LOG(logINFO) << ep.position();
    LOG(logINFO) << (int)ep.file();
    LOG(logINFO) << ep.fileStr();
    LOG(logINFO) << (int)ep.rank() ;
  }
  else{
    LOG(logINFO) << "No current en passant" ;
  }

  p.Display();

  // should fail "not your turn"
  Move m0("d5 e4",p.Turn());
  p = m0.ValidateAndApply(p);
  p.Display();

  // test capture with pawn
  Move m1("e4 d5",p.Turn());
  p = m1.ValidateAndApply(p);
  p.Display();

  // generate possible en passant
  Move m2("e7 e5",p.Turn());
  p = m2.ValidateAndApply(p);
  p.Display();

  // capture en passant
  Move m3("d5 e6",p.Turn());
  p = m3.ValidateAndApply(p);
  p.Display();

  // should fail : invalid piece in the way
  Move m4("d8 a5",p.Turn());
  p = m4.ValidateAndApply(p);
  p.Display();

  // should fail : invalid wrong direction
  Move m5("d8 h6",p.Turn());
  p = m5.ValidateAndApply(p);
  p.Display();

  // capture with bishop
  Move m6("c8 e6",p.Turn());
  p = m6.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m7("g1 f3",p.Turn());
  p = m7.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m8("g8 f6",p.Turn());
  p = m8.ValidateAndApply(p);
  p.Display();

  // move bishop
  Move m9("f1 d3",p.Turn());
  p = m9.ValidateAndApply(p);
  p.Display();

  // should fail : piece in the way
  Move m10("f8 h6",p.Turn());
  p = m10.ValidateAndApply(p);
  p.Display();

  // move bishop
  Move m11("f8 d6",p.Turn());
  p = m11.ValidateAndApply(p);
  p.Display();

  // white castle king side
  Move m12("0-0",p.Turn());
  p = m12.ValidateAndApply(p);
  p.Display();

  // knight move
  Move m13("b8 d7",p.Turn());
  p = m13.ValidateAndApply(p);
  p.Display();

  // pawn
  Move m14("b2 b3",p.Turn());
  p = m14.ValidateAndApply(p);
  p.Display();

  // black queen
  Move m15("d8 e7",p.Turn());
  p = m15.ValidateAndApply(p);
  p.Display();

  // white fianquetto
  Move m16("c1 b2",p.Turn());
  p = m16.ValidateAndApply(p);
  p.Display();

  // black castle queen side
  Move m17("0-0-0",p.Turn());
  p = m17.ValidateAndApply(p);
  p.Display();

  // knight
  Move m18("b1 c3",p.Turn());
  p = m18.ValidateAndApply(p);
  p.Display();

  // attack with bishop : check
  Move m19("d6 h2",p.Turn());
  p = m19.ValidateAndApply(p);
  p.Display();

  // should fail, does not remove check
  Move m20("a2 a3",p.Turn());
  p = m20.ValidateAndApply(p);
  p.Display();

  // remove check, take bishop with king
  Move m21("g1 h2",p.Turn());
  p = m21.ValidateAndApply(p);
  p.Display();

  // check with queen now
  Move m22("e7 d6",p.Turn());
  p = m22.ValidateAndApply(p);
  p.Display();

  // remove check with pawn
  Move m23("g2 g3",p.Turn());
  p = m23.ValidateAndApply(p);
  p.Display();
}

std::deque<SearcherBase::SearchedMove> Play(const std::string & fen, DepthType depth, bool AIVsAI){
    Position p(fen);
    //p.Display();

    std::deque<SearcherBase::SearchedMove> gameMoves;
    std::string strMove;
    FastContainer<Move> moves;

    Game game;
    game.SetHash(p);
    Analyse::EvalCache c;

    while(true){

       // already mate or stalemate ?
       MoveGenerator(true,false,false).Generator(p,moves);
       if ( moves.empty() ){
           if ( p.IsInCheck() ){
              LOG(logINFO) << "Result :: Checkmate " << (p.WhiteToPlay()?"0-1":"1-0");
           }
           else{
              LOG(logINFO) << "Result :: Stalemate " << "1/2-1/2";
           }
           break;
       }
       // is it a draw ?
       if ( Searcher::IsForcedDraw(game,p,c,3,true) ){
           LOG(logINFO) << "Result :: Can be a draw because of " << (p.WhiteToPlay()?"white":"black") << " 1/2-1/2";
           return gameMoves;
       }
       /*
       if ( AIVsAI && fabs(Util::Count(p)) > 2000){
           LOG(logINFO) << "Play :: One player is lost";
           return gameMoves;
       }
       */
       p.Display();
       // first player
       if ( AIVsAI ){
           LOG(logINFO) << "Play :: AI1 turn";
           bool moveOK = false;
           SearcherBase::SearchedMove best = TestAnalysis(p,depth,Searcher::eVerb_standard);
           Move & m = SearcherBase::GetMove(best);
           LOG(logINFO) << "Play :: AI1 plays " << m.ShowAlgAbr(p);
           p = m.ValidateAndApply(p,&moveOK);
           game.SetHash(p,-1,m.IsCapture());
           if ( ! moveOK ){
               LOG(logERROR) << "Play :: Bad computer move ! " << m.Show() ;
               break;
           }
           gameMoves.push_back(best);
           Util::DisplayPGN(fen,gameMoves);
       }
       else{
           LOG(logINFO) << "Play :: Your move";
           bool moveOK = false;
           while ( ! moveOK ){
              std::getline(std::cin, strMove);
              Move m(strMove,p.Turn());
              p = m.ValidateAndApply(p,&moveOK);
              if ( moveOK){
                  game.SetHash(p,-1,m.IsCapture());
                  gameMoves.push_back(SearcherBase::SearchedMove(m,-9999,-2));
                  LOG(logINFO) << "Play :: you play " << m.Show();
              }
           }
       }

       // write receipt
       if ( false ){
           std::ofstream str;
           str.open("js/test.json");
           str << Util::JsonPGN(Position(Position::startPosition).GetFEN(),gameMoves);
           str.close();
       }

       // already mate or stalemate ?
       MoveGenerator(true,false,false).Generator(p,moves);
       if ( moves.empty() ){
           if ( p.IsInCheck() ){
              LOG(logINFO) << "Result :: Checkmate " << (p.WhiteToPlay()?"0-1":"1-0");;
           }
           else{
              LOG(logINFO) << "Result :: Stalemate " << "1/2-1/2";
           }
           break;
       }
       // is it a draw ?
       if ( Searcher::IsForcedDraw(game,p,c,3,true) ){
          LOG(logINFO) << "Result :: Can be a draw because of " << (p.WhiteToPlay()?"white":"black") << " 1/2-1/2";
          return gameMoves;
       }
       /*
       if ( AIVsAI && fabs(Util::Count(p)) > 2000){
           LOG(logINFO) << "One player is lost";
           return gameMoves;
       }
       */
       // second player
       LOG(logINFO) << "Play :: AI2 turn";
       bool moveOK = false;
       p.Display();
       SearcherBase::SearchedMove best = TestAnalysis(p,depth, Searcher::eVerb_standard);
       Move & m = SearcherBase::GetMove(best);
       LOG(logINFO) << "Play :: AI2 plays " << m.ShowAlgAbr(p);
       p = m.ValidateAndApply(p,&moveOK);
       game.SetHash(p,-1,m.IsCapture());
       if ( ! moveOK ){
           LOG(logERROR) << "Play :: Bad computer move ! " << m.Show() ;
           break;
       }
       gameMoves.push_back(best);
       Util::DisplayPGN(fen,gameMoves);

       // write receipt
       if ( false ){
           std::ofstream str;
           str.open("js/test.json");
           str << Util::JsonPGN(Position(Position::startPosition).GetFEN(),gameMoves);
           str.close();
       }

    }

    return gameMoves;
}
