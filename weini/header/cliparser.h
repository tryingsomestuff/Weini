#pragma once

#include <tclap/CmdLine.h>
#include "version.h"

class OptionCallBack : public TCLAP::Visitor{
public:
   OptionCallBack(TCLAP::MultiArg<std::string> * args);
   void visit();
private:
   TCLAP::MultiArg<std::string> * _args;
};

// Singleton
class CLIParser{
public:

    class Options{
        friend class CLIParser;
    private:
        Options();
    public:
        TCLAP::SwitchArg logInFile;
        TCLAP::SwitchArg xboard;
        TCLAP::SwitchArg uci;
        TCLAP::MultiArg<std::string> command;
        TCLAP::ValueArg<std::string> test;
    };

    static CLIParser & Instance();
    static const Options & Opt();
    bool Parse(int argc, char ** argv);

private:
    TCLAP::CmdLine _cmd;
    CLIParser();
    Options _opt;
};
