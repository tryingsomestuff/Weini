#pragma once

#include "position.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>
#include <locale>
#include <iostream>
#include <iomanip>

class ExtendedPosition : public Position{
public:

    ExtendedPosition(const std::string & s, bool withMoveCount = true);

    bool ShallFindBest();

    std::vector<std::string> BestMoves();

    std::vector<std::string> BadMoves();

    std::string Id();

    static bool ReadEPDFile(const std::string & fileName, std::vector<std::string> & positions);
    static void Test(const std::vector<std::string> & positions,
                     const std::vector<int> &         timeControls,
                     bool                             breakAtFirstSuccess,
                     const std::vector<int> &         scores,
                     std::function< int(int) >        eloF,
                     bool                             withMoveCount = true,
                     bool                             display = false);

	static void TestStatic(const std::vector<std::string> & positions,
		                   int chunck = 4,
		                   bool withMoveCount = false,
		                   bool display = false);

private:

    std::map<std::string,std::vector<std::string> > _extendedParams;

};
