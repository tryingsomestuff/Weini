#include "uci.h"
#include "logger.hpp"
#include "CLI.h"
#include "timeman.h"
#include "thread.h"
#include "search.h"
#include "version.h"

#include <fstream>
#include <stdio.h>
#include <string>

UCI::UCI():Com(){
    Com::protocol = Com::e_UCI;
}

std::string UCI::DisplayInfo(DepthType depth, DepthType seldepth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv) {
    std::stringstream str;
    str << " depth " << int(depth) << "/" << int(seldepth)
        << " score cp " << int(score)
        << " nps " << nodes / float(sec)
        << " nodes " << nodes
        << " time " << int(sec*1000.f)
        << " pv " << pv;
        ///@todo hashfull
    return str.str();
}

bool UCI::MakeMove(Move &m, bool displayMove){
    bool moveOK = true;
    Position position2 = m.ValidateAndApply(position,&moveOK);
    if ( ! moveOK ){
        LOG(logERROR) << "UCI bad move ! " << m.Show();
        return false;
    }
    position = position2;
    if ( displayMove ) {
       LOG(logCOMGUI) << "bestmove " << m.ShowAlg(position,false,true,false);
    }
    ///@todo

    return true;
}

void UCI::Loop(){

    LOG(logINFO) << "Starting UCI main loop";

    while(true) {

        LOG(logINFO) << "UCI: mode " << mode;
        LOG(logINFO) << "UCI: stm  " << stm;

        // first do what's need to be done !

        // launch an analysis
        if(mode == m_analyze){
            //AnalyzeUntilInput(); ///@todo
        }

        // move as computer if mode is equal to stm
        if((int)mode == (int)stm) { // mouarfff
            move = ThinkUntilTimeUp();
            if(!move.IsValid()){ // game ends
                mode = m_force;
            }
            else{
                if ( ! MakeMove(move,true) ){
                    LOG(logFATAL) << "Bad computer move !" ;
                }
                stm = Opponent(stm);
            }
        }

        // if not our turn, and ponder on, let's think ...
        if(move.IsValid() && (int)mode == (int)Opponent(stm) && ponder == p_on && !pondering){
            PonderUntilInput();
        }

        bool commandOK = true;
        int once = 0;

        while(once++ == 0 || !commandOK){ // loop until a good command is found

            commandOK = true;

            // read next command !
            ReadLine();

            if ( command == "debug"){
                ///@todo debug mode
            }
            else if ( command == "uci"){
                Version::Splash();
                LOG(logCOMGUI) << "id name Weini " << Version::version;
                LOG(logCOMGUI) << "id author Vivien Clauzon";
                LOG(logCOMGUI) << "option name Ponder type check default true";
                ///@todo LOG(logCOMGUI) << "option name Ponder type check default true";
                LOG(logCOMGUI) << "uciok";
            }
            else if ( command == "isready"){
                // nothing ! I am ...
                LOG(logCOMGUI) << "readyok";
            }
            else if (strncmp(command.c_str(), "setoption", 9) == 0) {
                ///@todo responde to all allowed options !
            }
            else if ( command == "ucinewgame"){
                StopPonder();
                Stop();
                ResetPrevious();

                mode = (Mode)((int)stm); ///@todo should not be here ???
                position = Position(Position::startPosition);
                move = Move();
                if(mode != m_analyze){
                    mode = m_play_black;
                    stm = stm_white;
                }
                newGame = true;
            }
            else if (strncmp(command.c_str(), "position", 8) == 0) {
                StopPonder();
                Stop();
                std::string fen(command);
                size_t p = command.find("position");
                fen = fen.substr(p + 9);
                if (fen == "startpos") {
                    fen = "start";
                }
                else {
                    fen = fen.substr(p + 13);
                }

                if (!SideToMoveFromFEN(fen)) {
                    LOG(logERROR) << "Illegal FEN";
                    commandOK = false;
                }

                ///@todo parse moves ...
            }
            else if ( command == "register"){
                ///@todo
            }
            else if ( strncmp(command.c_str(),"go",2) == 0){
                StopPonder();
                Stop();
                mode = (Mode)((int)stm); /// mouarfff

                TimeMan::Instance().ResetParam();

                if (std::strstr(command.c_str(), "infinite")) {
                    mode = m_analyze;
                    TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                }

                if (std::strstr(command.c_str(), "ponder")) {
                    pondering = true;
                }

                if (std::strstr(command.c_str(), "searchmoves")) {
                    ///@todo implemente this ...
                    mode = m_analyze;
                    TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                }

                int converted; ///@todo use to catch errors
                if (std::strstr(command.c_str(), "wtime")) {
                    int wtime = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "wtime"), "%*s %d", &wtime);
                    if (mode == m_play_white && stm == stm_white) {
                        TimeMan::Instance().SetTCType(TimeMan::TCT_suddenDeathWithDynamicInfo);
                        TimeMan::Instance().SetTCParamDynamic(wtime,TimeMan::invalid);
                    }
                }
                if (std::strstr(command.c_str(), "btime")) {
                    int btime = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "btime"), "%*s %d", &btime);
                    if (mode == m_play_black && stm == stm_black) {
                        TimeMan::Instance().SetTCType(TimeMan::TCT_suddenDeathWithDynamicInfo);
                        TimeMan::Instance().SetTCParamDynamic(btime, TimeMan::invalid);
                    }
                }
                if (std::strstr(command.c_str(), "winc")) {
                    int winc = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "winc"), "%*s %d", &winc);
                    if (mode == m_play_white && stm == stm_white && winc > 0) {
                        TimeMan::Instance().SetTCType(TimeMan::TCT_suddenDeathWithDynamicInfo);
                        TimeMan::Instance().SetTCParam(TimeMan::invalid, TimeMan::invalid, winc);
                    }
                }
                if (std::strstr(command.c_str(), "binc")) {
                    int binc = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "binc"), "%*s %d", &binc);
                    if (mode == m_play_black && stm == stm_black && binc > 0) {
                        TimeMan::Instance().SetTCType(TimeMan::TCT_suddenDeathWithDynamicInfo);
                        TimeMan::Instance().SetTCParam(TimeMan::invalid, TimeMan::invalid, binc);
                    }
                }
                if (std::strstr(command.c_str(), "movestogo")) {
                    int movestogo = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "movestogo"), "%*s %d", &movestogo);
                    TimeMan::Instance().SetTCParamDynamic(TimeMan::invalid, movestogo);
                    TimeMan::Instance().SetTCType(TimeMan::TCT_classicWithDynamicInfo);
                }
                if (std::strstr(command.c_str(), "depth")) {
                    converted = sscanf(std::strstr(command.c_str(), "depth"), "%*s %d", &depth);
                    TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                }
                if (std::strstr(command.c_str(), "nodes")) {
                    int nodes = -1;
                    converted = sscanf(std::strstr(command.c_str(), "nodes"), "%*s %d", &nodes);
                    TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                    ///@todo implement this
                }
                if (std::strstr(command.c_str(), "mate")) {
                    ///@todo implement this
                    int n = -1;
                    converted = sscanf(std::strstr(command.c_str(), "mate"), "%*s %d", &n);
                    mode = m_analyze;
                    TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                }
                if (std::strstr(command.c_str(), "movetime")) {
                    int movetime = TimeMan::invalid;
                    converted = sscanf(std::strstr(command.c_str(), "movetime"), "%*s %d", &movetime);
                    TimeMan::Instance().SetMsecPerMove(movetime);
                    TimeMan::Instance().SetTCType(TimeMan::TCT_forced);
                }
            }
            else if ( command == "stop"){
                StopPonder();
                Stop();
                mode = m_force;
            }
            else if ( command == "ponderhit"){
                StopPonder();
                Stop();
                ponder = p_off;
                mode = m_force;
                ///@todo BETTER, go on searching !
            }
            else if ( command == "quit"){
                _exit(0);
            }
            //************ end of UCI command ********//
            else if ( command == "show"){
                position.Display();
                commandOK = false; // read another command
            }
            else{
                // old move syntax
                LOG(logWARNING) << "Uci does not know this command " << command;
            }

        } // readline

    } // while true

}
