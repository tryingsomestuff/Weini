#pragma once

#include "move.h"
#include "logger.hpp"

class Position;

struct Line {
    Line():n(0){}

    DepthType n;
	MiniMove moves[MAX_SEARCH_DEPTH];

    Line & operator=(const Line & l);

    static void ClearPV(Line * pv);
    static void ClearPV(Line & pv);
    static void SetPV(Line * pv,const MiniMove & m);
    static void UpdatePV(Line * pv, const Line & pv_loc, const MiniMove & m, const Position & p);
    std::string ShowPV();
    std::string GetPV(const Position & pp, bool abr = false);

};
