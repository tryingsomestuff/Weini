#include "com.h"
#include "logger.hpp"
#include "CLI.h"
#include "search.h"
#include "timeman.h"
#include "thread.h"

#include "xboard.h"
#include "uci.h"

Com::Protocol Com::protocol = Com::e_XBoard;

std::string Com::DisplayInfo(DepthType depth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv) {
    return protocol == e_XBoard ? XBoard::DisplayInfo(depth, score, sec, nodes, tbhits, pv)
                                : UCI   ::DisplayInfo(depth, score, sec, nodes, tbhits, pv);
}

void Com::ReadLine() {
#define CPP_STYLE_READ_LINE
#ifdef CPP_STYLE_READ_LINE
    LOG(logINFO) << "Waiting for input";
    command.clear();
    std::getline(std::cin, command);
#else
    static char com[4096];
    char *np;
    if ((fgets(com, sizeof com, stdin)) != NULL) {
        if (np = strchr(com, '\n')) { // replace \n with \0 ...
            *np = '\0';
        }
    }
    command = com;
#endif
    LOG(logINFO) << "Receive command : " << command;
}

Com::Com() : position(Position::startPosition) {
    ponder = Com::p_off;
    mode   = Com::m_analyze;
    stm    = Com::stm_white;

    depth     = -1; // max depth
    display   = false;
    newGame   = true;
    pondering = false;
}

void Com::AnalyzeUntilInput() {
    LOG(logINFO) << "Analysis requested";
    TestAnalysis(position, depth != -1 ? depth : (int)Definitions::debugConfig.defaultDepth, true/*display*/);
}

Move Com::ThinkUntilTimeUp() {
    LOG(logINFO) << "Think requested";

    if (newGame) {
        TimeMan::Instance().ResetGame();
        newGame = false;
    }

    TimeMan::Instance().Tic();

    TimeMan::Instance().GetNextTimePerMove(position);

    LOG(logINFO) << "Next search time " << Definitions::timeControl.currentMoveMs;

    SearcherBase::Data _data(position, depth > 0 ? depth : (int)Definitions::debugConfig.defaultDepth, true, true, Definitions::debugConfig.modedebug);
    SearcherBase::SearchedMove result = ThreadPool<Searcher>::Instance().SearchSync(_data);

    TimeMan::Instance().Toc();

    return SearcherBase::GetMove(result);
}

void Com::PonderUntilInput() {
    pondering = true;
    LOG(logINFO) << "Pondering request";
    TimeMan::Instance().Tic();
    Definitions::timeControl.currentMoveMs = ULLONG_MAX; // infinity
    SearcherBase::Data _data(position, depth > 0 ? depth : (int)Definitions::debugConfig.defaultDepth, true, true, Definitions::debugConfig.modedebug);
    ThreadPool<Searcher>::Instance().SearchASync(_data);
    LOG(logINFO) << "Pondering started async";
}

void Com::StopPonder() {
    if ((int)mode == (int)Opponent(stm) && ponder == p_on && pondering) {
        pondering = false;
        LOG(logINFO) << "Pondering stopped";
        Stop();
    }
}

void Com::Stop() {
    Searcher::stopFlag = true;
}

void Com::ResetPrevious(){
    Stats::previousScore[NSColor::c_white] = 0;
    Stats::previousScore[NSColor::c_black] = 0;
}

bool Com::SideToMoveFromFEN(const std::string & fen) {
    LOG(logINFO) << "Loading from FEN";
    position = Position(fen);
    stm = position.Turn() == NSColor::c_white ? stm_white : stm_black;
    return true; ///@todo false when position is invalid ??
}

