#ifndef __MYSTL_LIST_H__
#define __MYSTL_LIST_H__

namespace mystl
{

//list节点
template <typename T>
struct __list_node
{
    typedef void*   void_ptr;
    void_ptr        prev;
    void_ptr        next;
    T               data;
};

//list迭代器












}


#endif