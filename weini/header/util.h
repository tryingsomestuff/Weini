#pragma once

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <iostream>
#include <utility>
#include <time.h>

#ifdef _WIN32
#include <string>
#include <windows.h>
#else
#include <string>
#include <limits.h>
#include <unistd.h>
#endif

namespace Util{

template <typename T>
inline
void NextPowerOf2(T & x){
   T power = 1;
   while(power < x) power*=2;
   x = power;
}

struct Zobrist{
   typedef unsigned long long int HashType;

   static HashType zTable[64][15];
   static HashType enPassant[8][2];
   static HashType castlingRights[4][2];
   static HashType turn[2];
   static HashType trusted[2];
   static HashType board[64];
   static HashType disablePromotion[2];
   static HashType movetype[10];

   static void Init();
};

void ProcessMemUsage(double& vm_usage, double& resident_set);

void PrintStacktrace(FILE *out = stderr, unsigned int max_frames = 63);

size_t CountLine(std::istream &is);

std::string Trim(const std::string& str,
                 const std::string& whitespace = " \t");

std::string Reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t");

std::string GetDate(void);

std::string GetExePath();

}
