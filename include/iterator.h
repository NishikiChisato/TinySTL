#ifndef __MYSTL_ITERATOR_H__
#define __MYSTL_ITERATOR_H__


/**
 * @file iterator.h
 * @author NishikiChisato (NishikiChisato@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstddef>
#include "type_traits.h"


namespace mystl
{

/**
 * 五种基本迭代器类型，每一个迭代器内部必须定义该迭代器属于那种类型
 * 前三种只支持++
 * 第四种支持++与--
 * 第五种支持所有的指针操作，p + n, p - n, p1 - p2, p1 > p2
 * 
 */
struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iteraotr_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iteraotr_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };



//自定义迭代器如需兼容STL需要继承此类
template <typename Category, 
          typename T, 
          typename Distance = ptrdiff_t, 
          typename Pointer = T*, 
          typename Reference = T&>
struct iterator
{
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
};

//萃取器
template <typename Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      defference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

//native pointer版本
template <typename T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag      iterator_category;
    typedef T                               value_type;
    typedef ptrdiff_t                       difference_type;
    typedef T*                              pointer;
    typedef T&                              reference;
};

//native pointer to const版本
template <typename T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag      iterator_category;
    typedef T                               value_type;
    typedef ptrdiff_t                       difference_type;
    typedef T*                              pointer;
    typedef T&                              reference;
};

//快速求出迭代器的iterator_category
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&)
{
    //迭代器内部会自己定义tag结构体，直接返回对象即可
    return typename iterator_traits<Iterator>::iterator_category();
}

//快速求出迭代器的difference_type，注意返回值为defference_type*
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//distance函数组件
//该函数用于计算两迭代器之间的距离
//pass by value即可

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n;
    while(first != last) 
        ++first, ++n;
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::defference_type
distance(Iterator frist, Iterator last)
{
    return __distance(frist, last, typename iterator_traits<Iterator>::iterator_category());
}

//advance函数组件
//该函数用于将迭代器前进n个单位

//需要pass by reference
template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& it, Distance n, input_iterator_tag)
{
    while(n--) ++it;
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& it, Distance n, bidirectional_iterator_tag)
{
    if(n >= 0) while(n--) ++it;
    else while(n++) --it;
}

template <typename Random_Access_Iterator, typename Distance>
inline void __advance(Random_Access_Iterator& it, Distance n, random_access_iterator_tag)
{
    it += n;
}

template <typename Iterator, typename Distance>
inline void advance(Iterator& it, Distance n)
{
    __advance(it, n, iterator_category(it));
}




}


#endif