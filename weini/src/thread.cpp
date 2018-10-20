#include "thread.h"

#include <assert.h>

std::atomic_bool SearcherBase::_depthDisplayed[MAX_SEARCH_DEPTH];

std::mutex SearcherBase::_mutexDisplay;

void SearcherBase::SetData(Position & p, DepthType depth, Line pv, SearcherBase::eVerbosity verbosity) {
    _data.p             = p;
    _data.depth         = depth;
    _data.pv            = pv;
    _data.verbosity     = verbosity;
}

void SearcherBase::SetData(const Data & d) {
    _data = d;
}

SearcherBase::Data & SearcherBase::GetData() {
    return _data;
}

SearcherBase::SearcherBase(size_t n)
    : _index(n),
      _exit(false),
      _searching(true),
      _stdThread(&SearcherBase::IdleLoop, this),
      _data(Position("start"),0, eVerb_standard),
      _isASync(false){

  static bool onlyOnce = false;
  if ( ! onlyOnce ){
      std::lock_guard<std::mutex> lock(_mutex);
      ResetDepthDisplayed();
      onlyOnce = true;
  }

  TheadingTools::Register(Id());
  Wait();
}

SearcherBase::~SearcherBase(){
    assert(!_searching);
    _exit = true;
    Start();
    _stdThread.join();
}

void SearcherBase::ResetDepthDisplayed(){
    std::lock_guard<std::mutex> lock(_mutexDisplay);
    for (DepthType k = 0 ; k < MAX_SEARCH_DEPTH ; ++k ){
        _depthDisplayed[k] = false;
    }
}

bool SearcherBase::TryDisplay(int d, const std::string & txt, bool rewriteLast){
    std::lock_guard<std::mutex> lock(_mutexDisplay);

    for(int k = 0 ; k < d ; ++k){
        if ( ! _depthDisplayed[k]){
            return false;
        }
    }
    if ( _depthDisplayed[d]){
        if ( !rewriteLast){
            return false;
        }
        else{
            for(DepthType k = d+1 ; k < MAX_SEARCH_DEPTH ; ++k){
                if ( _depthDisplayed[k]){
                    return false;
                }
            }
        }
    }
    _depthDisplayed[d] = true;
    LOG(logCOMGUI) << txt;
    return true;
}

void SearcherBase::Start() {
  std::lock_guard<std::mutex> lock(_mutex);
  _searching = true;
  _cv.notify_one(); // Wake up the thread in IdleLoop()
}

void SearcherBase::Wait() {
  std::unique_lock<std::mutex> lock(_mutex);
  _cv.wait(lock, [&]{ return !_searching; });
}

void SearcherBase::IdleLoop() {
  while (true){
      std::unique_lock<std::mutex> lock(_mutex);
      _searching = false;
      _cv.notify_one(); // Wake up anyone waiting for search finished
      _cv.wait(lock, [&]{ return _searching; });
      if (_exit){
         return;
      }
      lock.unlock();
      Search();
      //LOG(logINFO) << "Thread " << Id() << " search done";
  }
}



