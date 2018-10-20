#include "xboard.h"
#include "logger.hpp"
#include "CLI.h"
#include "timeman.h"
#include "thread.h"
#include "search.h"
#include "version.h"

#include <fstream>

XBoard::XBoard():Com(){
    Com::protocol = Com::e_XBoard;
}

std::string XBoard::DisplayInfo(DepthType depth, DepthType seldepth, ScoreType score, float sec, NodeStatType nodes, NodeStatType tbhits, const std::string & pv) {
    std::stringstream str;
    str << int(depth)
        << " " << int(score)
        << " " << int(sec * 100)
        << " " << nodes
        //<< " " << tbhits
        << "(" << int(depth) << "/" << int(seldepth) << "), "
        << "\t" << pv;
    return str.str();
}

bool XBoard::MakeMove(Move &m, bool displayMove){
    bool moveOK = true;
    LOG(logINFO) << (displayMove ? "Sending move":"Applying move") << " " << m.ShowAlg(position, false, true, false);
    Position position2 = m.ValidateAndApply(position,&moveOK);
    if ( ! moveOK ){
        LOG(logERROR) << "XBoard bad move ! " << m.Show();
        return false;
    }
    position = position2;
    if ( displayMove ) {
       LOG(logCOMGUI) << "move " << m.ShowAlg(position,false,true,false);
    }

    return true;
}

void XBoard::Loop(){

    LOG(logINFO) << "Starting XBoard main loop";

    ///@todo more feature disable !!
    LOG(logCOMGUI) << "feature ping=1 setboard=1 colors=0 usermove=1 memory=0 sigint=0 sigterm=0 otime=0 time=1 nps=0 myname=\"Weini " << Version::version << Definitions::debugConfig.nameSuffixe << "\"";
    LOG(logCOMGUI) << "feature done=1";

    while(true) {

        LOG(logINFO) << "XBoard: mode " << mode;
        LOG(logINFO) << "XBoard: stm  " << stm;

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
           if (Definitions::algo.forceNoPonder) {
               LOG(logWARNING) << "Pondering is disable in config file!";
           }
           else {
               PonderUntilInput();
           }
        }

        bool commandOK = true;
        int once = 0;

        while(once++ == 0 || !commandOK){ // loop until a good command is found

            commandOK = true;

            // read next command !
            ReadLine();

            if ( command == "force"){
                mode = m_force;
            }
            else if ( command == "xboard"){
                Version::Splash();
            }
            else if ( command == "post"){
                display = true;
            }
            else if ( command == "nopost"){
                display = false;
            }
            else if ( command == "computer"){
                // nothing !
            }
            else if ( command == "random"){
                Definitions::evalConfig.do_randomNoise = !Definitions::evalConfig.do_randomNoise;
            }
            else if ( strncmp(command.c_str(),"protover",8) == 0){
                // nothing !
            }
            else if ( strncmp(command.c_str(),"accepted",8) == 0){
                // nothing !
            }
            else if ( strncmp(command.c_str(),"rejected",8) == 0){
                // nothing !
            }
            else if ( strncmp(command.c_str(),"ping",4) == 0){
                std::string s(command);
                size_t p = command.find("ping");
                s = s.substr(p+4);
                s=Util::Trim(s);
                LOG(logCOMGUI) << "pong " << s;
            }
            else if ( command == "new"){
                StopPonder();
                Stop();
                ResetPrevious();

                mode = (Mode)((int)stm); ///@todo should not be here !!! I thought start mode shall be analysis ...
                position = Position(Position::startPosition);
                move = Move();
                if(mode != m_analyze){
                    mode = m_play_black;
                    stm = stm_white;
                }
                newGame = true;
            }
            else if ( command == "white"){ // deprecated
                StopPonder();
                Stop();

                mode = m_play_black;
                stm = stm_white;
            }
            else if ( command == "black"){ // deprecated
                StopPonder();
                Stop();

                mode = m_play_white;
                stm = stm_black;
            }
            else if ( command == "go"){
                StopPonder();
                Stop();

                mode = (Mode)((int)stm);
            }
            else if ( command == "playother"){
                StopPonder();
                Stop();

                mode = (Mode)((int)Opponent(stm));
            }
            else if ( strncmp(command.c_str(),"usermove",8) == 0){
                StopPonder();
                Stop();

                std::string mstr(command);
                size_t p = command.find("usermove");
                mstr = mstr.substr(p + 8);
                mstr = Util::Trim(mstr);
                Move m(mstr,position.Turn(),false);
                if(!MakeMove(m,false)){ // make move
                    commandOK = false;
                    LOG(logFATAL) << "Bad opponent move !" ;
                }
                else{
                    stm = Opponent(stm);
                }
            }
            else if (  strncmp(command.c_str(),"setboard",8) == 0){
                StopPonder();
                Stop();

                std::string fen(command);
                size_t p = command.find("setboard");
                fen = fen.substr(p+8);
                if (!SideToMoveFromFEN(fen)){
                    LOG(logERROR) << "Illegal FEN";
                    commandOK = false;
                }
            }
            else if ( strncmp(command.c_str(),"result",6) == 0){
                StopPonder();
                Stop();

                LOG(logINFO) << command;
                mode = m_force;
            }
            else if ( command == "analyze"){
                StopPonder();
                Stop();

                mode = m_analyze;
            }
            else if ( command == "exit"){
                StopPonder();
                Stop();

                mode = m_force;
            }
            else if ( command == "easy"){
                StopPonder();

                ponder = p_off;
            }
            else if ( command == "hard"){
                ponder = p_on;
            }
            else if ( command == "quit"){
                _exit(0);
            }
            else if ( command == "pause"){
                StopPonder();
                ReadLine();
                while( command != "resume"){
                    LOG(logERROR) << "Error (paused): " << command;
                    ReadLine();
                }
            }
            else if( strncmp(command.c_str(), "time",4) == 0) {
                int centisec = 0;
                sscanf(command.c_str(), "time %d", &centisec);
                centisec *= 10;
                TimeMan::Instance().SwithTCTypeDynamic(true);
                TimeMan::Instance().SetTCParamDynamic(centisec, 0); // reset remainingMoveUntilNextTC to zero so it is updated automatically
            }
            else if ( strncmp(command.c_str(), "st", 2) == 0) {
                int msecPerMove = 0;
                sscanf(command.c_str(), "st %d", &msecPerMove);
                msecPerMove *= 1000;
                TimeMan::Instance().SetTCType(TimeMan::TCT_forced);
                TimeMan::Instance().SetMsecPerMove(msecPerMove);
            }
            else if ( strncmp(command.c_str(), "sd", 2) == 0) {
                sscanf(command.c_str(), "sd %d", &depth);
                TimeMan::Instance().SetTCType(TimeMan::TCT_notc);
                if(depth<0) depth = (int)Definitions::debugConfig.defaultDepth;
                TimeMan::Instance().ResetParam();
            }
            else if(strncmp(command.c_str(), "level",5) == 0) {
                int timeLeft = 0;
                int sec = 0;
                int inc = 0;
                int mps = 0;
                if( sscanf(command.c_str(), "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
                  sscanf(command.c_str(), "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                }
                timeLeft *= 60000;
                timeLeft += sec * 1000;
                int msecinc = inc * 1000;
                TimeMan::Instance().SetTCParam(mps, timeLeft, msecinc);
                if ( mps != 0 ){ // classic TC
                    TimeMan::Instance().SetTCType(TimeMan::TCT_classic);
                }
                else{ // incremental TC
                    TimeMan::Instance().SetTCType(TimeMan::TCT_suddenDeath);
                }
            }
            else if ( command == "edit"){
                ///@todo xboard edit
                ;
            }
            else if ( command == "?"){
                Stop();
            }
            else if ( command == "draw"){
                ///@todo xboard draw
                ;
            }
            else if ( command == "undo"){
                ///@todo xboard undo
                ;
            }
            else if ( command == "remove"){
                ///@todo xboard remove
                ;
            }
            else if (strncmp(command.c_str(), "setoption", 9) == 0) {
                std::stringstream str(command);
                std::string dummy,key,value;
                str >> dummy;
                str >> key;
                str >> value;
                if ( !Definitions::SetValue(key,value)){
                    LOG(logERROR) << "Unable to set value " << key << " = " << value;
                }
            }
            //************ end of Xboard command ********//
            else if ( command == "show"){
                position.Display();
                commandOK = false; // read another command
            }
            else{
                // old move syntax
                LOG(logWARNING) << "Xboard does not know this command " << command;
            }

        } // readline

    } // while true

}
