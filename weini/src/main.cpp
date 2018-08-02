#include <iostream>

#include "CLI.h"

#include "logger.hpp"
#include "setup.h"
#include "test.h"
#include "uci.h"
#include "version.h"
#include "xboard.h"

loglevel_e loglevel = logINFO;

int main(int argc, char ** argv) {

#ifdef __linux__
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);
#endif

  ///@todo use TCLAP instead of this stupid manual parsing of command line
  std::string option;
  if ( argc > 1 ){
      option = argv[1];
  }

  //**********************************************************************
  // say hello
  //**********************************************************************
  Version::Splash();

  //**********************************************************************
  // redirect log output if communication protocol is used
  //**********************************************************************
  std::ofstream * xof = 0;
  if ( option == "-xboard" || option == "-uci" ){ ///@todo json on/off
     xof = new std::ofstream(/*Util::GetExePath()+"/"+*/std::string("log.")+Util::GetDate());
     LogIt::SetOStream(*xof);
  }

  //**********************************************************************
  // setup
  //**********************************************************************
  Setup();

  //**********************************************************************
  // Tests
  //**********************************************************************
  if ( Test(argc,argv) ) return 0;

  //**********************************************************************
  // XBOARD protocol
  //**********************************************************************
  if ( option == "-xboard"){ // xboard is default
      XBoard().Loop();
  }

  //**********************************************************************
  // UCI protocol
  //**********************************************************************
  if (option == "-uci") {
      UCI().Loop();
  }

  //**********************************************************************
  // Close log, reset output stream
  //**********************************************************************
  if ( option == "-xboard" || option == "-uci" ){
      LogIt::SetOStream(std::cout);
      xof->close();
      delete xof;
  }

  return 0;
}
