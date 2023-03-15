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
    new (p) T(value);
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
    ForwardIterator cur = first;
    while(cur < last)
    {
        destory(cur);
        ++cur;
    }
}

template <typename ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
    __destory(first, last, typename mystl::__type_traits<
             typename iterator_traits<ForwardIterator>::iterator_category>::has_trivial_destructor());
}













}


#endif