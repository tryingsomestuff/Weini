#include "timeman.h"
#include "definitions.h"
#include "logger.hpp"
#include "position.h"

#include <math.h>

TimeMan::TimeMan() {

    // static info
    msecPerMove = 666;
    msecPerTC   = 60000;
    msecinc     = 0;
    mps         = 40;
    typeOfTC    = TCT_forced; // forced mode by default

    // dynamic info
    msecPreviousElapsed      = invalid;
    msecRemainingTimeInTC    = invalid;
    moveNumberForNextTC      = invalid;
    remainingMoveUntilNextTC = invalid;
}

TimeMan::~TimeMan() {
    Stop();
}

TimeMan & TimeMan::Instance() {
    static TimeMan tm;
    return tm;
}

void TimeMan::Init() {
    LOG(logINFO) << "Init TimeMan";
    Start();
}

void  TimeMan::ResetTC() {
    startTC               = Now(); // start time of the current TC
    msecRemainingTimeInTC = msecPerTC;
    moveNumberForNextTC   += mps;
}

void  TimeMan::ResetGame() {
    start   = Now(); // start time of the current game/test/...
    startTC = Now(); // start time of the current TC

    msecRemainingTimeInTC = msecPerTC;
    moveNumberForNextTC   = mps;
}

void TimeMan::SetTCType(TCType t) {
    typeOfTC = t;
}

void TimeMan::SwithTCTypeDynamic(bool isDyn) {
    switch (typeOfTC) {
    case TCT_classic:
        if ( isDyn) {
            typeOfTC = TCT_classicWithDynamicInfo;
        }
        break;
    case TCT_suddenDeath:
        if ( isDyn) {
            typeOfTC = TCT_suddenDeathWithDynamicInfo;
        }
        break;
    case TCT_notc:
        break;
    case TCT_forced:
        break;
    case TCT_classicWithDynamicInfo:
        if ( ! isDyn) {
            typeOfTC = TCT_classic;
        }
        break;
    case TCT_suddenDeathWithDynamicInfo:
        if ( ! isDyn) {
            typeOfTC = TCT_suddenDeath;
        }
        break;
    }
}

void TimeMan::SetMsecPerMove(int ms) { // for forced mode
    if ( ms != invalid){
        msecPerMove = ms;
    }
}

void TimeMan::SetTCParam(int _mps, int _msecPerTC, int _msecincr) {
    if (msecPerTC != invalid) { msecPerTC = _msecPerTC; }
    if (msecinc != invalid)   { msecinc   = _msecincr;  }
    if (mps != invalid)       { mps       = _mps;       }
}

void TimeMan::SetTCParamDynamic(int _msecRemainingTimeInTC, int _remainingMoveUntilNextTC) {
    if (_msecRemainingTimeInTC != invalid)    {  msecRemainingTimeInTC     = _msecRemainingTimeInTC;     }
    if (_remainingMoveUntilNextTC != invalid) {  remainingMoveUntilNextTC  = _remainingMoveUntilNextTC ; }
}

void TimeMan::ResetParam() {
    msecPerTC                = invalid;
    msecinc                  = invalid;
    mps                      = invalid;
    msecRemainingTimeInTC    = invalid;
    moveNumberForNextTC      = invalid;
    remainingMoveUntilNextTC = invalid;
}

void TimeMan::Tic() {
    tic = Now();
}

void TimeMan::Toc() {
    toc                 = Now();
    msecPreviousElapsed = int(toc - tic);
}

int TimeMan::GetNextTimePerMove(const Position & position) {

    // we use Definition::tc as a bridge to set up time control
    // this allow search to work without TimeMan if needed (test / self play / ...)

    switch (typeOfTC) {
    case TCT_notc:
    {
        LOG(logINFO) << "TC type : notc";
        Definitions::timeControl.timeControl = invalid;
        Definitions::timeControl.increment = invalid;
        Definitions::timeControl.movePerSession = invalid;
        Definitions::timeControl.currentMoveMs = ULLONG_MAX; // infinity
        break;
    }
    case TCT_forced:
    {
        LOG(logINFO) << "TC type : forced";
        Definitions::timeControl.timeControl = invalid;
        Definitions::timeControl.increment = invalid;
        Definitions::timeControl.movePerSession = invalid;
        Definitions::timeControl.currentMoveMs = msecPerMove; // forced
        break;
    }
    case TCT_classic:
    {
        LOG(logINFO) << "TC type : classic";
        Definitions::timeControl.timeControl    = msecPerTC;
        Definitions::timeControl.increment      = msecinc;
        Definitions::timeControl.movePerSession = mps;
        // computing Definitions::timeControl.currentMoveMs

        msecRemainingTimeInTC += msecinc;                      // adds last increment
        msecRemainingTimeInTC -= msecPreviousElapsed;          // remove last think time (from tic/toc)
        remainingMoveUntilNextTC = moveNumberForNextTC - position.CurrentMoveCount() + 1;

        if (remainingMoveUntilNextTC <= 0) { // end of current TC
            ResetTC();
            // and update remaining move count (which was 0) ...
            remainingMoveUntilNextTC = moveNumberForNextTC - position.CurrentMoveCount() + 1;
        }

        msecRemainingTimeInTC -= 50; // margin

        if (msecRemainingTimeInTC < 0) {
            // ouch
            LOG(logWARNING) << "Negative remaining time in TC... " << msecRemainingTimeInTC;
            msecRemainingTimeInTC = 50; // let's try that ...
        }

        const float x = 1.f - position.GamePhasePercent() / 100.f;
        msecPerMove = (int)(msecRemainingTimeInTC / remainingMoveUntilNextTC);
        msecPerMove += msecinc;                                 // add current increment
        LOG(logINFO) << "msecPerMove " << msecPerMove;
        //msecPerMove = int(msecPerMove * 1.15 * (tanh(-(2 * x - 2)) + 1) / 2); // game phase correction
        if (msecPerMove >= 0.9f*msecRemainingTimeInTC) {
            msecPerMove = int(0.85f*msecRemainingTimeInTC);              // security margin again
        }
        LOG(logINFO) << "msecPerMove corrected " << msecPerMove;

        LOG(logINFO) << "TC computation (classic) :";
        LOG(logINFO) << "                 msecPreviousElapsed        " << msecPreviousElapsed;
        LOG(logINFO) << "                 startTC                    " << startTC;
        LOG(logINFO) << "                 msecRemainingTimeInTC      " << msecRemainingTimeInTC;
        LOG(logINFO) << "                 mps                        " << mps;
        LOG(logINFO) << "                 current moves              " << position.CurrentMoveCount();
        LOG(logINFO) << "                 remainingMoveUntilNextTC   " << remainingMoveUntilNextTC;
        LOG(logINFO) << "                 msecinc                    " << msecinc;
        LOG(logINFO) << "                 msecPerMove                " << msecPerMove;

        Definitions::timeControl.currentMoveMs = msecPerMove;
        break;
    }
    case TCT_suddenDeath:
    {
        LOG(logINFO) << "TC type : sudden death";
        Definitions::timeControl.timeControl    = msecPerTC; // which is in fact for the full game
        Definitions::timeControl.increment      = msecinc;
        Definitions::timeControl.movePerSession = invalid;   // we don't know !...
        // computing Definitions::timeControl.currentMoveMs

        msecRemainingTimeInTC += msecinc;                      // adds last increment
        msecRemainingTimeInTC -= msecPreviousElapsed;          // remove last think time (fomr tic/toc)
        // always be able to play some more moves ...
        remainingMoveUntilNextTC = std::max((int)Definitions::timeControl.minMoveToGo,
                                            (int)Definitions::timeControl.maxMoveToGo - position.CurrentMoveCount());

        if (msecRemainingTimeInTC < 0) {
            // ouch
            LOG(logWARNING) << "Negative remaining time in TC... " << msecRemainingTimeInTC;
            msecRemainingTimeInTC = 50; // let's try that ...
        }

        const float x = 1.f - position.GamePhasePercent() / 100.f;
        msecPerMove = (int)(msecRemainingTimeInTC / remainingMoveUntilNextTC);
        msecPerMove += msecinc;                                // add current increment
        LOG(logINFO) << "msecPerMove " << msecPerMove;
        //msecPerMove = int(msecPerMove * 1.15 * (tanh(-(2 * x - 2)) + 1) / 2); // game phase correction
        if (msecPerMove >= 0.9f*msecRemainingTimeInTC) {
            msecPerMove = int(0.85f*msecRemainingTimeInTC);              // security margin again
        }
        LOG(logINFO) << "msecPerMove corrected " << msecPerMove;

        LOG(logINFO) << "TC computation (incremental):";
        LOG(logINFO) << "                 msecPreviousElapsed        " << msecPreviousElapsed;
        LOG(logINFO) << "                 startTC                    " << startTC;
        LOG(logINFO) << "                 msecRemainingTimeInTC      " << msecRemainingTimeInTC;
        LOG(logINFO) << "                 mps                        " << mps;
        LOG(logINFO) << "                 current moves              " << position.CurrentMoveCount();
        LOG(logINFO) << "                 remainingMoveUntilNextTC   " << remainingMoveUntilNextTC;
        LOG(logINFO) << "                 msecinc                    " << msecinc;
        LOG(logINFO) << "                 msecPerMove                " << msecPerMove;

        Definitions::timeControl.currentMoveMs = msecPerMove;
        break;
    }
    case TCT_classicWithDynamicInfo:
    {
        LOG(logINFO) << "TC type : classic with dynamic info";
        Definitions::timeControl.timeControl    = msecPerTC;
        Definitions::timeControl.increment      = msecinc;
        Definitions::timeControl.movePerSession = mps;
        // computing Definitions::timeControl.currentMoveMs

        // msecRemainingTimeInTC should have been updated !
        if (remainingMoveUntilNextTC <= 0) { // if remainingMoveUntilNextTC has been reset
            remainingMoveUntilNextTC = moveNumberForNextTC - position.CurrentMoveCount() + 1;
        }

        if (remainingMoveUntilNextTC <= 0) { // end of current TC
            ResetTC();
            // and update remaining move count (which was 0) ...
            remainingMoveUntilNextTC = moveNumberForNextTC - position.CurrentMoveCount() + 1;
        }

        msecRemainingTimeInTC -= 50; // margin

        if (msecRemainingTimeInTC < 0) {
            // ouch
            LOG(logWARNING) << "Negative remaining time in TC... " << msecRemainingTimeInTC;
            msecRemainingTimeInTC = 50; // let's try that ...
        }

        const float x = 1.f - position.GamePhasePercent() / 100.f;
        msecPerMove = (int)(msecRemainingTimeInTC / remainingMoveUntilNextTC);
        msecPerMove += msecinc;                                 // add current increment
        LOG(logINFO) << "msecPerMove " << msecPerMove;
        //msecPerMove = int(msecPerMove * 1.15 * (tanh(-(2 * x - 2)) + 1) / 2); // game phase correction
        if (msecPerMove >= 0.9f*msecRemainingTimeInTC) {
            msecPerMove = int(0.85f*msecRemainingTimeInTC);              // security margin again
        }
        LOG(logINFO) << "msecPerMove corrected " << msecPerMove;

        LOG(logINFO) << "TC computation (classic dyn) :";
        LOG(logINFO) << "                 msecPreviousElapsed        " << msecPreviousElapsed;
        LOG(logINFO) << "                 startTC                    " << startTC;
        LOG(logINFO) << "                 msecRemainingTimeInTC      " << msecRemainingTimeInTC;
        LOG(logINFO) << "                 mps                        " << mps;
        LOG(logINFO) << "                 current moves              " << position.CurrentMoveCount();
        LOG(logINFO) << "                 remainingMoveUntilNextTC   " << remainingMoveUntilNextTC;
        LOG(logINFO) << "                 msecinc                    " << msecinc;
        LOG(logINFO) << "                 msecPerMove                " << msecPerMove;

        Definitions::timeControl.currentMoveMs = msecPerMove;
        break;
    }
    case TCT_suddenDeathWithDynamicInfo:
    {
        LOG(logINFO) << "TC type : sudden death with dynamic info";
        Definitions::timeControl.timeControl    = invalid;
        Definitions::timeControl.increment      = invalid;
        Definitions::timeControl.movePerSession = invalid;
        // computing Definitions::timeControl.currentMoveMs

        // msecRemainingTimeInTC should have been set (if not, maybe depth or nodes mode activated ///@todo)
        // remainingMoveUntilNextTC has been set in "classic mode" and not set (and even unset !!!) in "sudden death" mode
        if (remainingMoveUntilNextTC <= 0) {
            // always be able to play some more moves ...
            remainingMoveUntilNextTC = std::max((int)Definitions::timeControl.minMoveToGo,
                                                (int)Definitions::timeControl.maxMoveToGo - position.CurrentMoveCount());
        }

        if (msecRemainingTimeInTC < 0) { // flag !
            // ouch
            LOG(logWARNING) << "Negative remaining time in TC... " << msecRemainingTimeInTC;
            msecRemainingTimeInTC = 50; // let's try that ...
        }

        const float x = 1.f - position.GamePhasePercent() / 100.f;
        msecPerMove = (int)(msecRemainingTimeInTC / remainingMoveUntilNextTC);
        msecPerMove += msecinc;                                 // add current increment
        LOG(logINFO) << "msecPerMove " << msecPerMove;
        //msecPerMove = int(msecPerMove * 1.15 * (tanh(-(2 * x - 2)) + 1) / 2); // game phase correction
        if (msecPerMove >= 0.9f*msecRemainingTimeInTC) {
            msecPerMove = int(0.85f*msecRemainingTimeInTC);              // security margin again
        }
        LOG(logINFO) << "msecPerMove corrected " << msecPerMove;

        LOG(logINFO) << "TC computation (incremental dyn) :";
        LOG(logINFO) << "                 msecPreviousElapsed        " << msecPreviousElapsed;
        LOG(logINFO) << "                 startTC                    " << startTC;
        LOG(logINFO) << "                 msecRemainingTimeInTC      " << msecRemainingTimeInTC;
        LOG(logINFO) << "                 mps                        " << mps;
        LOG(logINFO) << "                 current moves              " << position.CurrentMoveCount();
        LOG(logINFO) << "                 remainingMoveUntilNextTC   " << remainingMoveUntilNextTC;
        LOG(logINFO) << "                 msecinc                    " << msecinc;
        LOG(logINFO) << "                 msecPerMove                " << msecPerMove;

        Definitions::timeControl.currentMoveMs = msecPerMove;
        break;
    }
    }

    return msecPerMove;
}

int TimeMan::GetNextMaximalTimePerMove(const Position & position) {
    return std::min(10*msecPerMove, int(0.6*msecRemainingTimeInTC)); // this is a security margin
}

float TimeMan::Elapsed() {
    return (Now() - start) / 1000.f;
}

int TimeMan::ElapsedMS() {
    return int(Elapsed()*1000);
}

float TimeMan::ElapsedTC() {
    return (Now() - startTC) / 1000.f;
}

int TimeMan::ElapsedTCMS() {
    return int(ElapsedTC()*1000);
}

float TimeMan::ElapsedTic() {
    return (Now() - tic) / 1000.f;
}

int TimeMan::ElapsedTicMS() {
    return int(ElapsedTic()*1000);
}

void TimeMan::Start() {
    ResetGame();
}

void TimeMan::Stop() {

}
