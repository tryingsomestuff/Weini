#include <iostream>

#include "CLI.h"
#include "cliparser.h"
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

  //**********************************************************************
  // say hello
  //**********************************************************************
  Version::Splash();

  //**********************************************************************
  // setup
  //**********************************************************************
  Setup(argc,argv);

  //**********************************************************************
  // redirect log output if needed
  //**********************************************************************
  std::ofstream * xof = 0;
  if ( CLIParser::Opt().logInFile.getValue() ){
     xof = new std::ofstream(/*Util::GetExePath()+"/"+*/std::string("log.")+Util::GetDate());
     LogIt::SetOStream(*xof);
  }

  //**********************************************************************
  // Tests
  //**********************************************************************
  if ( !CLIParser::Opt().test.getValue().empty() ){
     return Test(CLIParser::Opt().test.getValue());
  }

  //**********************************************************************
  // XBOARD protocol
  //**********************************************************************
  if ( CLIParser::Opt().xboard.getValue()){
      XBoard().Loop();
  }
  //**********************************************************************
  // UCI protocol
  //**********************************************************************
  else if (CLIParser::Opt().uci.getValue()) {
      UCI().Loop();
  }

  //**********************************************************************
  // Close log, reset output stream
  //**********************************************************************
  if ( CLIParser::Opt().logInFile.getValue() ){
      LogIt::SetOStream(std::cout);
      xof->close();
      delete xof;
  }

  return 0;
}
