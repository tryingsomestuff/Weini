#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "line.h"
#include "move.h"
#include "position.h"

// some ideas taken from stockfish implementation in here

class SearcherBase{
public:

    typedef std::tuple<Move, ScoreType, DepthType> SearchedMove; // move, score, depth

    static inline Move &          GetMove (      SearchedMove & m) { return std::get<0>(m); }
    static inline ScoreType       GetScore(      SearchedMove & m) { return std::get<1>(m); }
    static inline DepthType       GetDepth(      SearchedMove & m) { return std::get<2>(m); }
    static inline const Move &    GetMove (const SearchedMove & m) { return std::get<0>(m); }
    static inline ScoreType       GetScore(const SearchedMove & m) { return std::get<1>(m); }
    static inline DepthType       GetDepth(const SearchedMove & m) { return std::get<2>(m); }

	struct Data {
        inline
        Data(const Position & pp, DepthType d, bool allowNM, bool disp, bool dispDebug)
			: p(pp), depth(d), allowNullMove(allowNM), display(disp), displayDebug(dispDebug),searchDone(false) {}
		Position     p;
        DepthType    depth;
		Line         pv;
		bool         allowNullMove;
		bool         display;
		bool         displayDebug;
		SearchedMove best;
		bool         searchDone;
	};

    void SetData(Position & p, DepthType depth, Line pv, bool  allowNullMove, bool display, bool displayDebug);
	void SetData(const Data & _data);
	Data & GetData();


    SearcherBase(size_t);
    virtual ~SearcherBase();

	virtual void Search() = 0;

    void         IdleLoop();
    void         Start();
    void         Wait();

    size_t       Id()const { return _index;}

    bool         IsMainThread()const { return Id() == 0 ; }

    bool         IsASync()const { return _isASync; }
    void         SetIsASync(bool b) { _isASync = b;}

    // thread-safe
    static void ResetDepthDisplayed();
    // thread-safe
    static bool TryDisplay(int d, const std::string & txt, bool rewriteLast);

private:
    static std::atomic_bool _depthDisplayed[MAX_SEARCH_DEPTH];
	
    std::mutex              _mutex;
    static std::mutex       _mutexDisplay;
    std::condition_variable _cv;
    size_t                  _index;

	// MUST be initialized BEFORE _stdThread (can be here to be sure ...)
	bool                    _exit;// = false;
	bool                    _searching;// = true;

    std::thread             _stdThread;

	Data                    _data;

    bool                    _isASync;
};

// singleton pool of threads
template < class T>
class ThreadPool : public std::vector<SearcherBase*> {
public:
	static ThreadPool<T> & Instance();
	~ThreadPool();

    void Setup(unsigned int n); // populate with n threads

	T & Main() { return *static_cast<T*>(front()); }

	SearcherBase::SearchedMove SearchSync(const SearcherBase::Data & d);
    void                       SearchASync(const SearcherBase::Data & d);

    bool stop;

    void StartOthers();
private:
	ThreadPool();
};

#include "thread.hpp"
