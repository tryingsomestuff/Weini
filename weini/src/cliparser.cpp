#include "cliparser.h"
#include "logger.hpp"
#include "definitions.h"

OptionCallBack::OptionCallBack(TCLAP::MultiArg<std::string>  * args):
    TCLAP::Visitor(),
   _args(args)
{

}

void OptionCallBack::visit(){
   std::string option = _args->getValue().back();
   std::stringstream str(option);
   std::string key,value;
   LOG(logINFO) << "Command line override " << option;
   str >> key;
   str >> value;
   //std::cout << key << std::endl;
   //std::cout << value << std::endl;
   Definitions::SetValue(key,value);
}

CLIParser::Options::Options():
    logInFile("l","logInFile","Output log to file instead of stdout (default is false)",false),
    xboard   ("x","xboard"   ,"Switch to xboard mode directly (default is false)"      ,false),
    uci      ("u","uci"      ,"Switch to uci mode directly (default is false)"         ,false),
    command  ("o","option"   ,"Override an option given in the config.json file"       ,false,"string",new OptionCallBack(&command)),
    test     ("t","test"     ,"Run a specific test (help for getting some hints)"      ,false,"","string")
{

}

CLIParser::CLIParser():
    _cmd("Weini from command line", ' ', Version::version)
{

}

bool CLIParser::Parse(int argc, char ** argv){
    static bool onlyOnce = true;
    if ( !onlyOnce){
       LOG(logERROR) << "CLI parser cannot be called twice";
       return false;
    }
    onlyOnce = false;
    bool success = true;
    try{
      // add my own additional args
      _cmd.add( _opt.logInFile );
      _cmd.add( _opt.xboard );
      _cmd.add( _opt.uci );
      _cmd.add( _opt.command );
      _cmd.add( _opt.test );
      // and parse the command line
      _cmd.parse(argc,argv);
    }
    catch(...){
        success = false;
    }
    return success;
}

CLIParser & CLIParser::Instance(){
    static CLIParser _parser;
    return _parser;
}

const CLIParser::Options & CLIParser::Opt(){
    return Instance()._opt;
}
