#ifndef __MYSTL_CONSTRUCT_H__
#define __MYSTL_CONSTRUCT_H__

/**
 * @file construct.h
 * @author NishikiChisato
 * @brief 负责对象的构造与析构
 * @version 0.1
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <new>
#include "type_traits.h"
#include "iterator.h"

namespace mystl
{

template <typename _Tp, typename T>
inline void construct(_Tp* p, T value)
{
    //调用placement new
    new ((void*)p) T(value);
}

template <typename _Tp>
inline void construct(_Tp* p)
{
    new ((void*)p) _Tp();
}

template <typename T>
inline void destory(T* p)
{
    if(nullptr != p) p->~T();
}

//含有trivial的，直接空实现
template <typename ForwardIterator>
inline void __destory(ForwardIterator first, ForwardIterator last, __true_type) {  }

template <typename ForwardIterator>
inline void __destory(ForwardIterator first, ForwardIterator last, __false_type) 
{
    for( ; first < last; ++first)
        destory(&*first);
}

template <typename ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
    __destory(first, last, typename mystl::__type_traits<
             typename iterator_traits<ForwardIterator>::value_type>::has_trivial_destructor());
}


//对于内置数据类型不需要析构

inline void destory(char*, char*) {  };
inline void destory(int*, int*) {  };
inline void destory(short*, short*) {  };
inline void destory(long*, long*) {  };
inline void destory(long long*, long long*) {  };
inline void destory(float*, float*) {  };
inline void destory(double*, double*) {  };






}


#endif