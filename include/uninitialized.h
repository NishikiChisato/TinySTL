#ifndef __MYSTL_UNINITIALIZED_H__
#define __MYSTL_UNINITIALIZED_H__

/**
 * @file uninitialized.h
 * @author NishikiChisato (NishikiChisato@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "type_traits.h"
#include "algobase.h"
#include <cstring>

namespace mystl
{

//下列为整组__uninitialized_fill_n的函数实现

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator 
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
    ForwardIterator cur = first;
    try
    {
        while(n--)
        {
            construct(cur, x);
            ++cur;
        }
    }
    catch(...)
    {
        for(; first != cur; ++first)
            destory(first);
    }
    return cur;//返回最后一个元素的下一个位置的迭代器
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator 
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
    //存在POD，交由更高阶函数执行
    return mystl::fill_n(first, n, x);
}

template <typename ForwardIterator, typename Size, typename T, typename _Tp>
inline ForwardIterator 
__uninitialized_fill_n(ForwardIterator first, Size n, const T& x, _Tp*)
{
    //用__type_traits来判断_Tp型别是否含有POD
    typedef typename mystl::__type_traits<_Tp>::is_POD_type     is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator 
uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

//以下为整组uninitialized_copy的实现

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator dest, mystl::__false_type)
{
    InputIterator p1 = first;
    ForwardIterator p2 = dest;
    try
    {
        while(p1 < last)
        {
            mystl::construct(p2, *p1);
            ++p2, ++p1;
        }
    }
    catch(...)
    {
        while(dest < p2)
        {
            mystl::destory(dest);
            ++dest;
        }
    }
    return p2;
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator dest, mystl::__true_type)
{
    //交由高阶算法批量执行
    //return mystl::copy(first, last, dest);
    ;
}

template <typename InputIterator, typename ForwardIterator, typename _Tp>
inline ForwardIterator 
__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator dest, _Tp*)
{
    typedef typename mystl::__type_traits<_Tp>::is_POD_type   is_POD;
    return __uninitialized_copy_aux(first, last, dest, is_POD());
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator 
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator dest)
{
    return __uninitialized_copy(first, last, dest, value_type(dest));
}

//针对const char*与const wchar_t*的特化版本（本质上是移动字符串，直接调用memmove最快）

inline const char* uninitialized_copy(const char* first, const char* last, char* dest)
{
    std::memmove(dest, first, last - first);
    return dest + (last - first);
}

inline const wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* dest)
{
    std::memmove(dest, first, sizeof(wchar_t) * (last - first));
    return dest + (last - first);
}


//以下为整组 uninitialized_fill 函数的实现


template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& val, mystl::__true_type)
{
    mystl::fill(first, last, val);
}

template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& val, mystl::__false_type)
{
    ForwardIterator cur = first;
    try
    {
        while(cur < last)
        {
            mystl::construct(cur, val);
            ++cur;
        }
    }
    catch(...)
    {
        while(first < cur) 
        {
            mystl::destory(first);
            ++first;
        }
    }
    
}

template <typename ForwardIterator, typename T, typename _Tp>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& val, _Tp*)
{
    typedef typename mystl::__type_traits<_Tp>::is_POD_type   is_POD;
    __uninitialized_fill_aux(first, last, val, is_POD());
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& val)
{
    __uninitialized_fill(first, last, val, value_type(first));
}



}


#endif