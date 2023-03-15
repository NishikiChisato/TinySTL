#ifndef __MYSTL_ALLOC_H__
#define __MYSTL_ALLOC_H__

#ifndef __THROW_BAD_ALLOC
#include <iostream>
#define __THROW_BAD_ALLOC                          \
        std::cerr << "out of memory" << std::endl; \
        exit(1)

#endif

/**
 * @file alloc.h
 * @author NishikiChisato
 * @brief 负责空间的配置与释放
 * @version 0.1
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstddef>
#include <cstdlib>
#include <new>

namespace mystl
{

// 一级配置器
class __malloc_alloc
{
public:
    static void *allocate(size_t);
    static void *reallocate(void *, size_t /*old_size*/ , size_t /*new_size*/);
    static void deallocate(void *, size_t);
    // 用于设置处理错误函数，返回值与参数均为函数指针，用于提供对外接口，返回值为原先的错误处理函数
    static void (*set_malloc_alloc_handler(void (*f)()))();

private:
    //处理out of memory
    static void *oom_allocate(size_t);
    static void *oom_reallocate(void *, size_t);
    static void (*__malloc_alloc_oom_handler)();
};

void (*__malloc_alloc::__malloc_alloc_oom_handler)() = nullptr;

void *__malloc_alloc::allocate(size_t n)
{
    void *res = std::malloc(n);
    if (res == nullptr)
        res = oom_allocate(n);
    return res;
}

//该函数中的old_size无效
inline void *__malloc_alloc::reallocate(void *p, size_t /*old_size*/ , size_t new_size)
{
    void *res = std::realloc(p, new_size);
    if (res == nullptr)
        res = oom_reallocate(p, new_size);
    return res;
}

//释放大小为n的空间，n同样无效
inline void __malloc_alloc::deallocate(void *p, size_t n)
{
    std::free(p);
}

inline void (*__malloc_alloc::set_malloc_alloc_handler(void (*f)()))()
{
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return old;
}

inline void* __malloc_alloc::oom_allocate(size_t n)
{
    void (* my_oom_handler)();
    void * res;
    //尝试调用错误处理函数后，再进行内存分配
    for(;;)
    {
        my_oom_handler = __malloc_alloc_oom_handler;
        if(nullptr == my_oom_handler) { __THROW_BAD_ALLOC; }
        (*my_oom_handler)();
        res = malloc(n);
        if(res) return res;
    }
}

inline void* __malloc_alloc::oom_reallocate(void *p, size_t n)
{
    void (* my_oom_handler)();
    void *res;
    for(;;)
    {
        my_oom_handler = __malloc_alloc_oom_handler;
        if(nullptr == my_oom_handler) { __THROW_BAD_ALLOC; }
        (* my_oom_handler)();
        res = realloc(p, n);
        if(res) return res;
    } 
}

typedef __malloc_alloc malloc_alloc;

//二级配置器

enum { __ALIGN = 8 };                           //空间以8对齐
enum { __MAX_BYTES = 512 };                     //小区块空间最大上界
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };  //小区块的数量
enum { __CHUNK_COUNT = 20 };                    //free list中无可用节点时默认分配节点数量

class __default_alloc
{
private:
    union obj
    {
        obj* freelist_link;              //指向下一个节点的指针
        char data[1];                    //当前节点的首地址
    };

    static size_t ROUND_UP(size_t);      //将字节按8向上对齐
    static obj* free_list[__NFREELISTS]; //用于管理各个小区块
    static size_t FREELIST_INDEX(size_t);//对应区块大小再free list中的映射

    //free list当中无可用节点，调用该函数
    //假设n以上调至__ALIG的倍数，至少返回一个大小为n的节点给客端，期望在free list中新增__CHUNK_COUNT-1个节点
    static void* refill(size_t n);
                                        //配置一大块区域，理想情况为n个size的空间，n可用改变
    static void* chunk_alloc(size_t, int&);

    static char* memory_start;          //memory pool 起始地址
    static char* memory_end;            //memory pool 结束地址
    static size_t memory_size;          //memory pool 堆栈大小

public:
    static void* allocate(size_t);
    static void deallocate(void*, size_t);
    static void* reallocate(void*, size_t /*old_size*/, size_t /*new_size*/);
};

char* __default_alloc::memory_start = nullptr;

char* __default_alloc::memory_end = nullptr;

size_t __default_alloc::memory_size = 0;

__default_alloc::obj* __default_alloc::free_list[__NFREELISTS] = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr
    };

inline size_t __default_alloc::ROUND_UP(size_t bytes)
{
    return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);  //先向上进位，随后对后三位清零
}

inline size_t __default_alloc::FREELIST_INDEX(size_t size)   //sz 需要 >0
{
    return (size + __ALIGN - 1) / __ALIGN - 1;        //下标从0开始
}

inline void* __default_alloc::allocate(size_t n)
{
    if(n > static_cast<size_t>(__MAX_BYTES))
        return malloc_alloc::allocate(n);
    obj** my_free_list = nullptr;
    obj* res = nullptr;

    my_free_list = free_list + FREELIST_INDEX(n);
    res = *my_free_list;
    if(nullptr == res)
    {
        res = static_cast<obj*>(refill(ROUND_UP(n)));
        return res;
    }
    *my_free_list = res->freelist_link;
    return res;
}

//回收大小为n的对象，小于__MAX_BYTES回归memory pool
inline void __default_alloc::deallocate(void* p, size_t n)
{
    if(n > static_cast<size_t>(__MAX_BYTES))
    {
        malloc_alloc::deallocate(p, n);
        return;
    }
    obj** my_free_list = nullptr;
    obj* tmp = static_cast<obj*>(p);
    my_free_list = free_list + FREELIST_INDEX(n);
    tmp->freelist_link = *my_free_list;
    *my_free_list = tmp;
}

//重新分配p指针指向的空间，old_size为原空间大小，new_size为新空间大小
inline void* __default_alloc::reallocate(void* p, size_t old_size, size_t new_size)
{
    if(new_size > static_cast<size_t>(__MAX_BYTES))
        return malloc_alloc::reallocate(p, old_size, new_size);
    obj** old_free_list = nullptr, ** new_free_list = nullptr;
    obj* res = static_cast<obj*>(p);
    old_free_list = free_list + FREELIST_INDEX(old_size);
    new_free_list = free_list + FREELIST_INDEX(new_size);
    res->freelist_link = *old_free_list;
    *old_free_list = res;
    res = *new_free_list;
    *new_free_list = res->freelist_link;
    return res;
}

//n已上调为__ALIGN的倍数，返回大小为n的对象
inline void* __default_alloc::refill(size_t n)
{
    int cnt_chunk = __CHUNK_COUNT;
    char* chunk = static_cast<char*>(chunk_alloc(n, cnt_chunk));
    if(1 == cnt_chunk) return chunk;//如果只能分配一个节点，那么返回给客端，free list中无新增节点
    obj** my_free_list = free_list + FREELIST_INDEX(n);
    obj* res = reinterpret_cast<obj*>(chunk);//第一块返还给客端
    obj* cur_ptr = nullptr, *next_ptr = nullptr;
    //将free list指向新开辟的空间
    *my_free_list = next_ptr = reinterpret_cast<obj*>(chunk + n);
    //迭代，将各个区块以n为单位链接起来
    for(int i = 1; ; i ++)
    {
        cur_ptr = next_ptr;
        next_ptr = reinterpret_cast<obj*>(reinterpret_cast<char*>(next_ptr) + n);
        if(cnt_chunk - 1 != i)
            cur_ptr->freelist_link = next_ptr;
        else
        {
            cur_ptr->freelist_link = nullptr;
            break;
        }
    }
    return res;
}

//分配cnt_chunk个size大小的区块
inline void* __default_alloc::chunk_alloc(size_t size, int& cnt_chunk)
{
    char* res = nullptr;
    size_t left_bytes = memory_end - memory_start;
    size_t total_bytes = size * cnt_chunk;
    if(left_bytes >= total_bytes)
    {
        //memory pool 剩余空间足够
        res = memory_start;
        memory_size += total_bytes;
        return res;
    }
    else if(left_bytes >= size)
    {
        //memory pool 中的空间足够至少分配一个区块
        //cnt_chunk只在本部分才会被修改，用于协助后面递归调用对cnt_chunk的修正
        cnt_chunk = left_bytes / size;
        res = memory_start;
        memory_start += cnt_chunk * size;
        return res;
    }
    else
    {
        //memory pool 中空间不足以分配一个区块
        //先将memory pool 中的空间配置给free list
        if(left_bytes > 0)
        {
            obj** my_free_list = free_list + FREELIST_INDEX(left_bytes);
            obj* p = reinterpret_cast<obj*>(memory_start);
            p->freelist_link = *my_free_list;
            *my_free_list = p;
        }
        //分配两倍total_bytes加一个随分配次数而增加的附加量
        size_t bytes_get = 2 * total_bytes + ROUND_UP(memory_size >> 4);
        memory_start = (char*)malloc(bytes_get);
        if(nullptr == memory_start)
        {
            //分配失败的处理
            //考虑将free list中的区块编入memory pool
            obj** my_free_list = nullptr;
            for(int i = size; i <= __MAX_BYTES; i += __ALIGN)
            {
                my_free_list = free_list + FREELIST_INDEX(i);
                if(nullptr != *my_free_list)
                {
                    //将my_free_list指向下一个free list当中的小区块
                    *my_free_list = (*my_free_list)->freelist_link;
                    memory_start = reinterpret_cast<char*>(*my_free_list);
                    memory_end = memory_start + i;
                    //递归调用，修正cnt_chunk
                    return chunk_alloc(size, cnt_chunk);
                }
            }
            //free list中也无法再分配出一个区块
            memory_end = nullptr;
            //寄希望与一级配置器的oom_allocate函数，虽然会导致exception的抛出
            memory_start = reinterpret_cast<char*>(malloc_alloc::allocate(size));
        }
        memory_end = memory_start + bytes_get;
        memory_size += bytes_get;
        //若分配成功，则表示memory pool中已得到新的空间
        //递归调用本函数，修正cnt_chunk
        return chunk_alloc(size, cnt_chunk);
    }
}

typedef __default_alloc alloc;


template <typename T, typename Alloc>
class simple_alloc
{
public:
    static T* allocate() { return static_cast<T*>(Alloc::allocate(sizeof (T))); }
    static T* allocate(size_t n) { return 0 == n ? nullptr : static_cast<T*>(Alloc::allocate(n * sizeof (T))); }
    static void deallocate(T* p) { Alloc::deallocate(p, sizeof (T)); }
    static void deallocate(T* p, size_t n) { if(0 != n) Alloc::deallocate(p, n * sizeof (T)); }
};


}

#endif