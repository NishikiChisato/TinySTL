#ifndef __MYSTL_VECTOR_H__
#define __MYSTL_VECTOR_H__

#include "alloc.h"
#include "construct.h"
#include "iterator.h"
#include "uninitialized.h"
#include "type_traits.h"

namespace mystl
{

template <typename T, typename Alloc = mystl::alloc>
class vector
{
public:
    typedef                 T                       value_type;
    typedef                 value_type*             pointer;
    typedef                 value_type*             iterator;
    typedef                 value_type&             reference;
    typedef                 size_t                  size_type;
    typedef                 ptrdiff_t               difference_type;

protected:
    typedef     mystl::simple_alloc<T, Alloc>       data_allocate;

    iterator    start;
    iterator    finish;
    iterator    end_of_storage;

    void insert_aux(iterator pos, const T& val);

    //释放空间
    void deallocate()
    {
        if(start) data_allocate::deallocate(start, end_of_storage - start);
    }

    iterator allocate_and_fill(size_type n, const T& val)
    {
        iterator res = data_allocate::allocate(n);
        mystl::uninitialized_fill_n(res, n, val);
        return res;
    }

    //填充n个对象，用于构造函数的调用
    void fill_initialize(size_type n, const T& val)
    {
        start = allocate_and_fill(n, val);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator    begin()                         { return start; }
    iterator    end()                           { return finish; }
    size_type   size()          const           { return size_type(end() - begin()); }
    size_type   capacity()      const           { return size_type(end_of_storage - begin()); }
    bool        empty()         const           { return begin() == end(); }
    reference   operator[](size_type idx)       { return *(begin() + idx); }

    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {  }
    vector(size_type n, const T& val)   { fill_initialize(n, val); }
    vector(int n, const T& val)         { fill_initialize(n, val); }
    vector(long n, const T& val)        { fill_initialize(n, val); }
    explicit vector(size_type n)        { fill_initialize(n, T()); }

    ~vector()
    {
        mystl::destory(start, finish);
        deallocate();
    }

    reference front()   { return *begin(); }
    reference back()    { return *(end() - 1); }
    void push_back(const T& val) 
    {
        if(finish != end_of_storage)
        {
            mystl::construct(finish, val);
            ++finish;
        }
        else
            insert_aux(end(), val);
    }
    void pop_back()
    {
        --finish();
        mystl::destory(finish);
    }

    iterator insert(iterator pos, const T& val);
    iterator insert(iterator pos, size_type n, const T& val);

    iterator erase(iterator pos)
    {
        if(pos + 1 != finish) mystl::copy(pos + 1, finish, pos);
        --finish;
        mystl::destory(finish);
        return pos;
    }
    iterator erase(iterator first, iterator last)
    {
        if(first != last)
        {
            if(last != finish) mystl::move(last, finish, first);
            iterator nxt_last = first + finish - last;
            iterator nxt_finish = nxt_last;
            while(nxt_last < finish)
            {
                mystl::destory(nxt_last);
                ++nxt_last;
            }
            finish = nxt_finish;
            return first;
        }
    }

    void resize(size_type new_size, const T& val)
    {
        if(new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), val);
    }
    void resize(size_type new_size)
    {
        resize(new_size, T());
    }

    void clear()
    {
        erase(begin(), end());
    }
    


};






}











#endif