
template < class T>
ThreadPool<T> & ThreadPool<T>::Instance() {
    static ThreadPool<T> pool;
    return pool;
}

template < class T>
ThreadPool<T>::ThreadPool() : stop(false) {
    LOG(logINFO) << "Init thread pool";
    push_back(new T(size())); // this one will be called "Main" thread
}

template < class T>
void ThreadPool<T>::Setup(unsigned int n) {

    LOG(logINFO) << "Populating thread pool with " << n << " threads";

    assert(n > 0);
    while (size() < (unsigned int)n) {
        push_back(new T(size()));
    }

}

template < class T>
ThreadPool<T>::~ThreadPool() {
    while (size()) {
        delete back();
        pop_back();
    }
}

template < class T>
void ThreadPool<T>::StartOthers() {
    //LOG(logINFO) << "Call to ThreadPool StartOthers";

    // launch everybody else
    for (auto s : *this) {
        if (!(*s).IsMainThread()) {
            (*s).Start();
        }
    }
}

template < class T>
SearcherBase::SearchedMove ThreadPool<T>::SearchSync(const SearcherBase::Data & d) {

    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchSync begin";
    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchSync waiting for threads";

    // wait for everyone
    for (auto s : *this) {
       (*s).Wait();
    }

    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchSync all threads ok";

    SearcherBase::ResetDepthDisplayed();

    // gives new data to everybody
    for (auto s : *this) {
        (*s).SetData(d);
        (*s).SetIsASync(false);
    }

    // Launch main thread
    Main().Search();

    // tells everybody to stop
    T::stopFlag = true;

    // wait for other threads to return
    for(auto s : *this){
        if (!(*s).IsMainThread()) {
            (*s).Wait();
        }
    }

    // return main thread result
    return ThreadPool<T>::Instance().Main().GetData().best;
}

template < class T>
void ThreadPool<T>::SearchASync(const SearcherBase::Data & d) {

    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchASync begin";
    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchASync waiting for threads";

    // wait for everyone
    for (auto s : *this) {
       (*s).Wait();
    }

    if (d.verbosity >= SearcherBase::eVerb_verbose) LOG(logINFO) << "SearchASync all threads ok";

    SearcherBase::ResetDepthDisplayed();

    // gives new data to everybody
    for (auto s : *this) {
        (*s).SetData(d);
        (*s).SetIsASync(true);
    }

    // Launch main thread
    Main().Start();

    // stopFlag shall be set somewhere else !!!

}
