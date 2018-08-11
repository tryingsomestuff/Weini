#pragma once

#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <mutex>
#include <iomanip>

inline
std::string showDate() {
    using Clock = std::chrono::high_resolution_clock;
    std::stringstream str;
    time_t rawtime;
    time(&rawtime);
    auto msecEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch());
    struct tm * timeInfo;
    timeInfo = localtime(&rawtime);
    char buffer[64];
    std::strftime(buffer,63,"%Y-%m-%d %H:%M:%S",timeInfo);
    str << buffer << "-";

    str << std::setw(3) << std::setfill('0') << msecEpoch.count()%1000;
    return str.str();
}

#ifdef __linux__

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

inline
std::string backtrace() {
  void *array[64];
  size_t size = backtrace(array, 64);
  char ** bk = backtrace_symbols(array, size);
  std::string ret;
  for(size_t k = 0 ; k < size ; ++k){
      ret += std::string(bk[k]) + "\n";
  }
  return ret;
}

#elif defined __MINGW32__
inline
std::string backtrace(){
    ////@todo
    return "";
}

#elif defined _WIN32

#include "dbg_win.h"

inline
std::string backtrace() {
    std::stringstream buff;
    std::vector<windbg::StackFrame> stack = windbg::stack_trace();
    buff << "Callstack: \n";
    for (unsigned int i = 0; i < stack.size(); ++i)	{
        buff << "0x" << std::hex << stack[i].address << ": " << stack[i].name << "(" << std::dec << stack[i].line << ") in " << stack[i].module << "\n";
    }
    return buff.str();
}

#elif defined __CYGWIN__
inline
std::string backtrace() {
    ///@todo
    return "";
}

#endif

#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>

template <typename T>
std::ostream& Print(std::ostream& os, const T& container){
    for(auto ii = container.cbegin(); ii != container.cend(); ++ii){
      os << (*ii) << " ";
    }
    return os;
}

template< class X>
std::ostream& operator<<(std::ostream& os, const std::vector<X>& v) { return Print(os, v); }
template< class X>
std::ostream& operator<<(std::ostream& os, const std::deque<X>& v) { return Print(os, v); }
template< class X>
std::ostream& operator<<(std::ostream& os, const std::list<X>& v) { return Print(os, v); }
template< class X>
std::ostream& operator<<(std::ostream& os, const std::set<X>& v) { return Print(os, v); }
template< class X>
std::ostream& operator<<(std::ostream& os, const std::multiset<X>& v) { return Print(os, v); }

///@todo logTITLE
enum loglevel_e : unsigned char {
    logCOMGUIERR = 0,
    logCOMGUI    = 1,
    logFATAL     = 2,
    logERROR     = 3,
    logWARNING   = 4,
    logFAIL      = 5,
    logINFO      = 6,
    logDEBUG     = 7,
    logMAX       = 8
};

// leading # is considered as a comment in xboard/uci communication protocol...
static const char * _levelNames[logMAX] = {"COM_ERR ",
                                           "",
                                           "#FATAL   - ",
                                           "#ERROR   - ",
                                           "#WARNING - ",
                                           "#FAIL    - ",
                                           "#INFO    - ",
                                           "#DEBUG   - "
                                           };

class LogIt{
public:
    LogIt(loglevel_e loglevel):_level(loglevel){}

    inline
    static void SetOStream(std::ostream & os){ _os = &os;}

    template <typename T>
    LogIt & operator<<(T const & value)    {
        _buffer << value;
        return *this;
    }

    ~LogIt(){
        std::lock_guard<std::mutex> lock(_mutex);

        if ( _level != logCOMGUI){
           // log
           (*_os) << _levelNames[_level] << showDate() << ": " << _buffer.str() << std::endl;
        }
        else{
           // log (debug only ...)
           //(*_os) << "#COMGUI  - "       << showDate() << ": " << _buffer.str() << std::endl;
           // gui communication
           std::cout << _buffer.str() << std::endl << std::flush;
        }

        if (_level == logFATAL) {
            (*_os) << backtrace() << std::endl;
        }
        else if (_level == logERROR) {
            (*_os) << backtrace() << std::endl;
        }

        if ( _level == logFATAL){
#ifdef _WIN32
            //DebugBreak();
            _exit(1);
#else
            exit(1);
#endif
        }
    }

private:

    static std::mutex     _mutex;
    static std::ostream * _os;
    std::ostringstream    _buffer;
    loglevel_e            _level;
};

extern loglevel_e loglevel;

#define LOG(level)      \
if (level > loglevel) ; \
else LogIt(level)

#ifdef __linux__

inline
void handler(int sig = -1) {
  if ( sig != -1){
      LOG(logWARNING) << "Signal " << sig;
  }
  LOG(logWARNING) << backtrace();
  exit(-sig);
}

#endif
