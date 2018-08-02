#pragma once

#include "logger.hpp"

#include <fstream>

#ifndef GIT_SHA
#define GIT_SHA "coffeedeadbeef0000"
#endif

#ifndef GIT_VERSION
#define GIT_VERSION "v0.0.20"
#endif

class Version{
public:
  static const std::string sha;
  static const std::string version;

  static std::string GetProc(){
#ifndef WIN32
      std::string line;
      std::ifstream finfo("/proc/cpuinfo");
      while(getline(finfo,line)) {
          std::stringstream str(line);
          std::string itype;
          std::string info;
          if ( getline( str, itype, ':' ) && getline(str,info) && itype.substr(0,10) == "model name" ) {
              return info;
              break;
          }
      }
#else
	  return "unknown";
#endif
      return "aieaie";
  }

  static void Splash(){
      LOG(logINFO) << "This is Weini ";
      LOG(logINFO) << "Current version is " << version;
      LOG(logINFO) << "SHA : "              << sha;
      LOG(logINFO) << "Running on : "       << GetProc();
  }

};
