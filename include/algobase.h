#ifndef __MYSTL_ALGOBASE_H__
#define __MYSTL_ALGOBASE_H__

/**
 * @file algobase.h
 * @author NishikiChisato (NishikiChisato@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "construct.h"


namespace mystl
{

//对于trivial对象，调用该函数，对区间[first, last)进行填充
template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& val)
{
    for(; first < last; ++first)
        *first = val;
}

//连续填充n个trivial对象，返回值为dest + n
template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator dest, Size n, const T& val)
{
    for(; n > 0; n--, ++dest)
        *dest = val;
    return dest;
}



} // namespace mystl










#endif