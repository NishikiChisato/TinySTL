#ifndef __MYSTL_ALLOC_
#define __MYSTL_ALLOC_

#ifndef __THROW_BAD_ALLOC
#include <iostream>
#define __THROW_BAD_ALLOC                          \
        std::cerr << "out of memory" << std::endl; \
        exit(1)

#endif

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
    // 处理out of memory函数
    static void *oom_allocate(size_t);
    static void *oom_reallocate(void *, size_t);
    static void (*__malloc_alloc_oom_handler)();
};

void (*__malloc_alloc::__malloc_alloc_oom_handler)() = nullptr;

void *__malloc_alloc::allocate(size_t n)
{
    void *res = malloc(n);
    if (res == nullptr)
        res = oom_allocate(n);
    return res;
}

//该函数中的old_size无效
void *__malloc_alloc::reallocate(void *p, size_t /*old_size*/ , size_t new_size)
{
    void *res = realloc(p, new_size);
    if (res == nullptr)
        res = oom_reallocate(p, new_size);
    return res;
}

//释放大小为n的空间，n同样无效
void __malloc_alloc::deallocate(void *p, size_t n)
{
    free(p);
}

void (*__malloc_alloc::set_malloc_alloc_handler(void (*f)()))()
{
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return old;
}

void* __malloc_alloc::oom_allocate(size_t n)
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

void* __malloc_alloc::oom_reallocate(void *p, size_t n)
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

size_t __default_alloc::ROUND_UP(size_t bytes)
{
    return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);  //先向上进位，随后对后三位清零
}

size_t __default_alloc::FREELIST_INDEX(size_t size)   //sz 需要 >0
{
    return (size + __ALIGN - 1) / __ALIGN - 1;        //下标从0开始
}

void* __default_alloc::allocate(size_t n)
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
void __default_alloc::deallocate(void* p, size_t n)
{
    if(n > static_cast<size_t>(__MAX_BYTES))
    {
        malloc_alloc::deallocate(p, n);
        return;
    }
    obj** my_free_list;
    obj* tmp = static_cast<obj*>(p);
    my_free_list = free_list + FREELIST_INDEX(n);
    tmp->freelist_link = *my_free_list;
    *my_free_list = tmp;
}

//重新分配p指针指向的空间，old_size为原空间大小，new_size为新空间大小
void* __default_alloc::reallocate(void* p, size_t old_size, size_t new_size)
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

//n已上调为__ALIGN的倍数
void* __default_alloc::refill(size_t n)
{
    int cnt_chunk = __CHUNK_COUNT;
    char* chunk = static_cast<char*>(chunk_alloc(n, cnt_chunk));
    if(1 == cnt_chunk) return chunk;//如果只能分配一个节点，那么返回给客端，free list中无新增节点
    obj** my_free_list = free_list + ROUND_UP(n);
    obj* res = reinterpret_cast<obj*>(chunk);//第一块返还给客端
    obj* cur_ptr = nullptr, *next_ptr = nullptr;
    //将free list指向新开辟的空间
    *my_free_list = next_ptr = reinterpret_cast<obj*>(chunk + n);
    //迭代，将各个区块以n为单位链接起来
    for(int i = 1; ; i ++)
    {
        cur_ptr = next_ptr;
        next_ptr = reinterpret_cast<obj*>(reinterpret_cast<char*>(next_ptr) + i * n);
        if(cnt_chunk - 1 > i)
            cur_ptr->freelist_link = next_ptr;
        else
        {
            cur_ptr->freelist_link = nullptr;
            break;
        }
    }
    return res;
}

void* __default_alloc::chunk_alloc(size_t size, int& cnt_chunk)
{

}

}



#endif