#pragma once

#include <string>

#include "move.h"
#include "position.h"

class Com {
public:

    enum Protocol {
        e_XBoard = 0,
        e_UCI = 1
    };

    static Protocol protocol;

    static std::string DisplayInfo(DepthType depth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv);

    Com();
    ~Com(){}

    enum Mode : unsigned char {
        m_play_white = 0,
        m_play_black = 1,
        m_force = 2,
        m_analyze = 3
    };

    enum SideToMove : unsigned char {
        stm_white = 0,
        stm_black = 1
    };

    enum Ponder : unsigned char {
        p_off = 0,
        p_on = 1
    };

    virtual void Loop() = 0;

    SideToMove Opponent(SideToMove & s) {
        return s == stm_white ? stm_black : stm_white;
    }

protected:

    Ponder      ponder;
    Mode        mode;
    SideToMove  stm;
    std::string command;
    Move        move;

    bool        newGame;

    Position    position;

    bool        display;

    bool        pondering;

    int         depth; // max depth

    void         AnalyzeUntilInput();
    Move         ThinkUntilTimeUp();
    void         PonderUntilInput();
    void         StopPonder();
    void         Stop();
    void         ResetPrevious();
    bool         SideToMoveFromFEN(const std::string &fen);

    virtual bool MakeMove(Move &m, bool displayMove = true) = 0;
    virtual void ReadLine();
};
