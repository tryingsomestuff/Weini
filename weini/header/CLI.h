#pragma once

#include "square.h"
#include "fastContainer.h"
#include "moveGenerator.h"
#include "stats.h"
#include "thread.h"
#include "search.h"

#include <vector>

class Position;

void GeneratorWrapperLoop(std::vector<Stats::NodeCount> & nodesV,
                          std::vector<NodeStatType> & perft,
                          Position & p,
                          const FastContainer<Move> & moves,
                          DepthType currentDepth,
                          bool display = false,
                          bool pseudolegal = false);

std::vector<NodeStatType> GeneratorWrapper(const std::string & fen,
                                            DepthType depth = 1,
                                            bool display = false,
                                            bool pseudolegal = false);

bool TestGenerator(const std::string & fen,
                   std::vector<NodeStatType> expected,
                   DepthType depth = 1,
                   bool display = false,
                   bool pseudolegal = false);


bool TestThreats(const std::string & fen,
                 const std::string & square,
                 std::vector<Square> expected,
                 bool display = false);

bool TestMoveHash();

SearcherBase::SearchedMove TestAnalysis(Position & pAnal, DepthType depth, Searcher::eVerbosity verbosity);

SearcherBase::SearchedMove TestAnalysis(const std::string & fen, DepthType depth, Searcher::eVerbosity verbosity);

ScoreType TestStaticAnalysis(const std::string & fen);

void TestMoveOrdering(const std::string & fen);

void TestSEE(const std::string & fen, const std::string & square, ScoreType initalCaptureValue);

void TestGameZHash();

void TestGamePlay();

void TexelTuning(const std::string & filename);

std::deque<SearcherBase::SearchedMove> Play(const std::string & fen,DepthType depth, bool AIVsAI = false);


