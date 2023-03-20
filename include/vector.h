#ifndef __MYSTL_VECTOR_H__
#define __MYSTL_VECTOR_H__

#include "alloc.h"
#include "construct.h"
#include "iterator.h"
#include "uninitialized.h"
#include "type_traits.h"

#include <memory>

namespace mystl
{

template <typename T, typename Alloc = mystl::alloc>
class vector
{
public:
    typedef                 T                       value_type;
    typedef                 value_type*             pointer;
    typedef                 value_type*             iterator;
    typedef                 const value_type*       const_iterator;
    typedef                 value_type&             reference;
    typedef                 size_t                  size_type;
    typedef                 ptrdiff_t               difference_type;

protected:

public:
    typedef     mystl::simple_alloc<value_type, Alloc>       data_allocate;

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
    iterator        begin()                         { return start; }
    const_iterator  begin()         const           { return start; }                
    iterator        end()                           { return finish; }
    const_iterator  end()           const           { return finish; }
    size_type       size()          const           { return static_cast<size_type>(end() - begin()); }
    size_type       capacity()      const           { return static_cast<size_type>(end_of_storage - begin()); }
    bool            empty()         const           { return begin() == end(); }
    reference       operator[](size_type idx)       { return *(begin() + idx); }

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

    reference   front()   { return *begin(); }
    reference   back()    { return *(end() - 1); }
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
        --finish;
        mystl::destory(finish);
    }

    void insert(iterator pos, const T& val)     { insert_aux(pos, val); }
    void insert(iterator pos, size_type n, const T& val);

    iterator erase(iterator pos)
    {
        if(pos + 1 != finish) std::copy(pos + 1, finish, pos);
        --finish;
        mystl::destory(finish);
        return pos;
    }
    iterator erase(iterator first, iterator last)
    {
        if(first != last)
        {
            if(last != finish) std::move(last, finish, first);
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

template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, const T& val)
{
    if(finish != end_of_storage)//还有剩余空间
    {
        mystl::construct(finish, *(finish - 1));
        std::copy_backward(pos, finish - 2, finish - 1);
        *pos = val;
    }
    else
    {

        size_type old_size = size();
        size_type new_size = (old_size == 0) ? 1 : 2 * old_size;
        iterator new_start = data_allocate::allocate(new_size);
        iterator new_finish = new_start;
        try
        {            
            new_finish = std::uninitialized_copy(start, pos, new_start);
            mystl::construct(new_finish, val);
            ++new_finish;
            new_finish =  std::uninitialized_copy(pos, finish, new_finish);
        }
        catch(...)
        {
            mystl::destory(new_start, new_finish);
            data_allocate::deallocate(new_start, new_size);
            throw;
        }

        mystl::destory(begin(), begin());
        this->deallocate();

        this->start = new_start;
        this->finish = new_finish;
        this->end_of_storage = finish;
    }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator pos, size_type n, const T& val)
{
    if(n > 0)
    {
        //剩余空间足够
        if(static_cast<size_type>(end_of_storage - finish) >= n)
        {
            iterator old_finish = finish;
            size_type elem_after = static_cast<size_type>(finish - pos);
            //插入点之后的元素个数大于n
            if(elem_after > n)
            {
                //先移动[finish-n,finish)到finish后面
                std::uninitialized_copy(finish - n, finish, finish);
                finish += n;
                //移动[pos, old_finish-n)到pos+n
                std::copy_backward(pos, old_finish - n, old_finish);
                //对[pos, pos+n)进行填充
                mystl::fill(pos, pos + n, val);
            }
            else//插入点之后的元素个数小于等于n
            {
                //先在finish后面填充n-elem_after个元素
                mystl::uninitialized_fill_n(finish, n - elem_after, val);
                finish += n - elem_after;
                //接着将[pos,old_finish)移动到当前finish后面
                std::uninitialized_copy(pos, old_finish, finish);
                finish += elem_after;
                //对[pos,old_finish)进行填充
                mystl::fill(pos, old_finish, val);
            }
        }
        else
        {
            size_type old_size = size();
            size_type new_size = old_size + std::max(old_size, n);
            iterator new_start = data_allocate::allocate(new_size);
            iterator new_finish = new_start;
            try
            {
                new_finish =  std::uninitialized_copy(start, pos, new_start);
                new_finish = mystl::uninitialized_fill_n(new_finish, n, val);
                new_finish = std::uninitialized_copy(pos, finish, new_finish);
            }
            catch(...)
            {
                mystl::destory(new_start, new_finish);
                this->deallocate();
                throw;
            }

            mystl::destory(begin(), end());
            this->deallocate();
            
            this->start = new_start;
            this->finish = new_finish;
            this->end_of_storage = new_finish;
        }
    }
}




}











#endif