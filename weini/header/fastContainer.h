#pragma once

#include <deque>
#include <vector>
#include <array>
#include "definitions.h"
#include <algorithm>
#include <type_traits>

#define SORT_FIRST_TIME_LIMIT 8

//#ifdef WITH_MEMORY_POOL
#include "memoryPool.h"
static const unsigned int _default_block_size = 4096u * 4096u;
//#endif

//#ifdef WITH_MEMORY_POOL_BUILDER
template<class T, int blockSize>
class PoolObjectBuilder {
public:
    template <class... Args>
    T * New(Args&&... args) {
        T * n = pool.newElement(args...);
        assert(n);
        return n;
    }

    static
    T * Clone(const T & p) {
        T * n = pool.newElement(p);
        assert(n);
        return n;
    }

    static
    void Free(T * p) {
        pool.deleteElement(p);
    }

private:
    static MemoryPool<T, blockSize> pool;
};

template<class T, int blockSize>
MemoryPool<T,blockSize> PoolObjectBuilder<T,blockSize>::pool;

template<class T, int blockSize>
class ScopePoolObject {
public:
    template <class... Args>
    ScopePoolObject(Args&&... args) {
        _p = PoolObjectBuilder<T,blockSize>().New(args...);
    }

    ~ScopePoolObject() {
        PoolObjectBuilder<T, blockSize>::Free(_p);
    }

    T & Get() {
        return *_p;
    }

private:
    T * _p;
};


template<class T, int blockSize>
class ScopeClone {
public:
    ScopeClone(const T & p) {
        _p = PoolObjectBuilder<T,blockSize>::Clone(p);
    }

    ~ScopeClone() {
        PoolObjectBuilder<T, blockSize>::Free(_p);
    }

    T & Get() {
        return *_p;
    }

private:
    T * _p;
};

//#endif

//#define FCBaseClass std::array<T,128> // in fact slower than vector ...!
#define FCBaseClass std::vector<T>

template < class T >
class FastContainer : public FCBaseClass {
public:

    //FastContainer():FCBaseClass(){ FCBaseClass::reserve(32);}

    template<class A>
    inline
    void partial_sort(const A & a) {
        std::partial_sort(FCBaseClass::begin(), FCBaseClass::begin()+SORT_FIRST_TIME_LIMIT, FCBaseClass::end(), a);
    }

    template<class A>
    inline
    void partial_sort_end(const A & a) {
        std::sort(FCBaseClass::begin()+SORT_FIRST_TIME_LIMIT, FCBaseClass::end(), a);
    }

    template<class A>
    inline
    void sort(const A & a) {
        std::sort(FCBaseClass::begin(), FCBaseClass::end(), a);
    }

    inline
    void moveToFront(int64_t i){
        if (i == 0) return;
        T t = FCBaseClass::operator[](i);
        for (int64_t z = i - 1; z >= 0; --z) {
            FCBaseClass::operator[](z + 1) = FCBaseClass::operator[](z);
        }
        FCBaseClass::operator[](0) = t;
    }

};
