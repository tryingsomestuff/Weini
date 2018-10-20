#pragma once

#include <chrono>

class Position;

// singleton
class TimeMan {
public:

    enum eTC {
        eTC_off = 0,
        eTC_on  = 1
    };

    static inline bool IsTCOn(eTC tc) {
        return tc == eTC_on;
    }

	static TimeMan & Instance();
	~TimeMan();

	static const int invalid = -1;

	void  Init();
	void  ResetTC();
	void  ResetGame();

	// will update Definition::tc::* and return msecPerMove for next move
	int   GetNextTimePerMove(const Position & position);
	// will update Definition::tc::* and return msecPerMove for next move
	int   GetNextMaximalTimePerMove(const Position & position);

    typedef std::chrono::milliseconds::rep TimeMSec;

    static inline TimeMSec Now() {
      return std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();
    }

	float Elapsed();
	int   ElapsedMS();
	float ElapsedTC();
	int   ElapsedTCMS();
	float ElapsedTic();
	int   ElapsedTicMS();

	enum TCType : unsigned char {
		TCT_forced      = 0, // fixed "timetomove"
		TCT_classic     = 1, // used when mps is given
		TCT_suddenDeath = 2, // used when mps is not given but total time and incr are given
		TCT_notc        = 3, // used for infinite seach, used for fixed depth, fixed nodes
		TCT_classicWithDynamicInfo     = 4, // used when mps is given and time/otime are given
		TCT_suddenDeathWithDynamicInfo = 5  // used when mps is not given and time/otime are given
	};

	void SetTCType(TCType t);

	inline TCType GetTCType() { return typeOfTC; }

	void SwithTCTypeDynamic(bool isDyn);

	inline bool IsForced() { return typeOfTC == TCT_forced; }

	void SetTCParam(int mps = invalid, int msecPerTC = invalid, int msecIncr = invalid);
	void SetTCParamDynamic(int msecRemainingTimeInTC = invalid, int remainingMoveUntilNextTC = invalid);

	void ResetParam();

	void SetMsecPerMove(int ms); // for foced mode

	void Tic();
	void Toc();

private:

	TimeMan();

	void Start();
	void Stop();

    TimeMSec start;
    TimeMSec startTC;
    TimeMSec tic;
    TimeMSec toc;

	// base informations
	int msecPerMoveMax; // max msec for next move
	int msecPerMove;    // optimal msec for next move
	int msecPerTC;      // msec per TC
	int msecinc;        // increment
	int mps;            // move per TC

	// Dynamic informations
	int msecRemainingTimeInTC; // remaining time in current TC
	int moveNumberForNextTC;   // move number at which next TC will start
	int remainingMoveUntilNextTC;   // move to go until next TC
	int msecPreviousElapsed;   // last think time duration

	TCType typeOfTC;

};
