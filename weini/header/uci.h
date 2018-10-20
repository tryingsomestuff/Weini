#pragma once

#include <string>
#include "com.h"

class UCI : public Com{

public:

   UCI();

   virtual void Loop();

   static std::string DisplayInfo(DepthType depth, DepthType seldepth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv);

private:
   virtual bool        MakeMove(Move &m, bool displayMove = true);
};
