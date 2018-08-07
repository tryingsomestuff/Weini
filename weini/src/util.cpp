#include "util.h"
#include "UtilPosition.h"
#include "Piece.h"
#include "position.h"
#include "thread.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <stdlib.h>

#include "move.h"

void Util::DisplayPGN(const std::string & fen,const std::deque<SearcherBase::SearchedMove> & game){
    // write pgn
    Position p2(fen);
    int k = p2.Moves();
    std::stringstream ss;
    ss << "PGN :: " ;
    for(auto it = game.begin() ; it != game.end() ; ++it,++k){
       if(k%2){
           ss << k/2+1 << ". ";
       }
       ss << SearcherBase::GetMove(*it).ShowAlgAbr(p2) << " {" << SearcherBase::GetScore(*it) << ", (" << (int)SearcherBase::GetDepth(*it) << ")} ";
       p2.ApplyMove(SearcherBase::GetMove(*it));
    }
    ss << std::endl;
    LOG(logINFO) << ss.str();
}

std::string Util::JsonPGN(const std::string & fen, const std::deque<SearcherBase::SearchedMove> &game){
    // write pgn to json format
    Position p2(fen);
    int k = p2.Moves();
    std::stringstream ss;
    ss << "{ \n" ;
    ss << "   \"PGN\": [ \n" ;
    for(auto it = game.begin() ; it != game.end() ; ++it,++k){
       ss << "      {\n";
       ss << "           \"ply\": " << k << ",\n";
       ss << "           \"move\": \"" << SearcherBase::GetMove(*it).ShowAlgAbr(p2) << "\",\n";
	   ss << "           \"depth\": \"" << SearcherBase::GetDepth(*it) << "\",\n";
       ss << "           \"score\": " << SearcherBase::GetScore(*it) << "\n";
       ss << "      },\n";
       p2.ApplyMove(SearcherBase::GetMove(*it));
    }
    ss << "      {\n";
    ss << "           \"ply\": -1,\n";
    ss << "           \"move\": \"none\",\n";
	ss << "           \"depth\": -1,\n";
    ss << "           \"score\": -9999\n";
    ss << "      }\n";
    ss << "   ]\n" ;
    ss << "}\n";
    return ss.str();
}

std::deque<Square::LightSquare> Util::FindPiece(const Position & p, NSColor::eColor c, Piece::eType t){
    std::deque<Square::LightSquare> v;
	std::bitset<64> bs(p.BitBoard().GetBitBoard(t));
    for ( Square::LightSquare k = 0 ; k < 64 ; ++k){
        if (bs[k]){
            v.push_back(k);
        }
    }
    return v;
}

// always only one king ... so here's an optimized version (no vector and early _exit)
Square::LightSquare Util::FindKing(const Position & p, NSColor::eColor c){
    Piece::eType kingType = (c == NSColor::c_white ? Piece::t_Wking : Piece::t_Bking);
	std::bitset<64> bs(p.BitBoard().GetBitBoard(kingType));
    for ( Square::LightSquare k = 0 ; k < 64 ; ++k){
		if (bs[k]) {
            return k;
        }
    }
    LOG(logFATAL) << "King not found";
    return Square::Invalide;
}

Util::Zobrist::HashType Util::Zobrist::zTable[64][15];
Util::Zobrist::HashType Util::Zobrist::enPassant[8][2];
Util::Zobrist::HashType Util::Zobrist::castlingRights[4][2];
Util::Zobrist::HashType Util::Zobrist::turn[2];
Util::Zobrist::HashType Util::Zobrist::trusted[2];
Util::Zobrist::HashType Util::Zobrist::board[64];
Util::Zobrist::HashType Util::Zobrist::disablePromotion[2];
Util::Zobrist::HashType Util::Zobrist::movetype[10];

#if __cplusplus <= 199711L
Util::Zobrist::HashType rand64(){
   Util::Zobrist::HashType r30 = RAND_MAX*rand()+rand();
   Util::Zobrist::HashType s30 = RAND_MAX*rand()+rand();
   Util::Zobrist::HashType t4  = rand() & 0xf;
   return (r30 << 34) + (s30 << 4) + t4;
}
#endif

void Util::Zobrist::Init(){
	LOG(logINFO) << "Init Zobrist hash";
#if __cplusplus <= 199711L
  #define MY_RAND() rand64();
#else
    std::random_device rd;
    std::mt19937 mt(rd());
    auto randInt = std::uniform_int_distribution<Util::Zobrist::HashType>(1,UINT64_MAX);
#define MY_RAND() randInt(mt);
#endif
    for(int i = 0 ; i < 64 ; ++i){
            for (int k = 0 ; k < 15 ; ++k){
                zTable[i][k] = MY_RAND();
            }
    }
    for(int i = 0 ; i < 8 ; ++i){
        for(int k = 0 ; k < 2 ; ++k){
            enPassant[i][k] = MY_RAND();
        }
    }
    for(int i = 0 ; i < 4 ; ++i){
        for(int k = 0 ; k < 2 ; ++k){
            castlingRights[i][k] = MY_RAND();
        }
    }
    for(int i = 0 ; i < 2 ; ++i){
        turn[i] = MY_RAND();
        trusted[i] = MY_RAND();
    }
    for(int i = 0 ; i < 64 ; ++i){
        board[i]    = MY_RAND();
    }
    for(int i = 0 ; i < 2 ; ++i){
        disablePromotion[i] = MY_RAND();
    }
    for(int i = 0 ; i < 10 ; ++i){
        movetype[i] = MY_RAND();
    }
}

void Util::ProcessMemUsage(double& vm_usage, double& resident_set){

    vm_usage     = 0.0;
    resident_set = 0.0;

#ifdef __linux__
#ifndef __CYGWIN__
    // 'file' stat seems to give the most reliable results
    std::ifstream stat_stream("/proc/self/stat",std::ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    std::string pid, comm, state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    std::string utime, stime, cutime, cstime, priority, nice;
    std::string O, itrealvalue, starttime;

    // the two fields we want
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage     = vsize / 1024.0;
    resident_set = rss * page_size_kb;
#endif
#endif
}

void Util::PrintStacktrace(FILE *out, unsigned int max_frames){
    fprintf(out, "stack trace:\n");

#ifdef __linux__

    // storage array for stack trace address _data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0) {
        fprintf(out, "  <empty, possibly corrupt>\n");
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++){
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p){
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
                && begin_name < begin_offset){
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                fprintf(out, "  %s : %s+%s\n",
                        symbollist[i], funcname, begin_offset);
            }
            else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf(out, "  %s : %s()+%s\n",
                        symbollist[i], begin_name, begin_offset);
            }
        }
        else{
            // couldn't parse the line? print the whole line.
            fprintf(out, "  %s\n", symbollist[i]);
        }
    }

    free(funcname);
    free(symbollist);

#endif
}

size_t Util::CountLine(std::istream &is){
    // skip when bad
    if( is.bad() ) return 0;
    // save state
    std::istream::iostate state_backup = is.rdstate();
    // clear state
    is.clear();
    std::istream::streampos pos_backup = is.tellg();

    is.seekg(0);
    size_t line_cnt;
    size_t lf_cnt = (size_t)std::count(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), '\n');
    line_cnt = lf_cnt;
    // if the file is not end with '\n' , then line_cnt should plus 1
    is.unget();
    if( is.get() != '\n' ) { ++line_cnt ; }

    // recover state
    is.clear() ; // previous reading may set eofbit
    is.seekg(pos_backup);
    is.setstate(state_backup);

    return line_cnt;
}

std::string Util::Trim(const std::string& str, const std::string& whitespace){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string Util::Reduce(const std::string& str,
                   const std::string& fill,
                   const std::string& whitespace){
    // trim first
    auto result = Trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

#define MAX_DATE 24

std::string Util::GetDate(void){
   time_t now;
   char the_date[MAX_DATE];
   the_date[0] = '\0';
   now = time(NULL);
   if (now != -1){
      strftime(the_date, MAX_DATE, "%Y%m%d_%H%M%S", gmtime(&now));
   }
   return std::string(the_date);
}

#ifdef _WIN32
std::string Util::GetExePath(){
  char result[ MAX_PATH ];
  return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
}
#elif defined __CYGWIN__
std::string Util::GetExePath(){
  return "";
}
#elif defined __linux__
std::string Util::GetExePath(){
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}
#endif

