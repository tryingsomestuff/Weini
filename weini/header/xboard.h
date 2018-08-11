#pragma once

#include <string>
#include "com.h"

class XBoard : public Com {

public:

   XBoard();

   virtual void Loop();

   static std::string DisplayInfo(DepthType depth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv);

private:
   virtual bool        MakeMove(Move &m, bool displayMove = true);
};
