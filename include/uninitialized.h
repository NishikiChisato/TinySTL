#ifndef __MYSTL_UNINITIALIZED_H__
#define __MYSTL_UNINITIALIZED_H__

#include "type_traits.h"

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
    return fill_n(first, n, x);
}

template <typename ForwardIterator, typename Size, typename T, typename _Tp>
inline ForwardIterator 
__uninitialized_fill_n(ForwardIterator first, Size n, const T& x, _Tp*)
{
    //用__type_traits来判断_Tp型别是否含有POD
    typedef typename __type_traits<_Tp>::is_POD_type     is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator 
uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}




}


#endif