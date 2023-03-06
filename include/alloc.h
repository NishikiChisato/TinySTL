#ifndef __MYSTL__ALLOC__
#define __MYSTL__ALLOC__

#ifndef __THROW_BAD_ALLOC
#include <iostream>
#define __THROW_BAD_ALLOC                          \
        std::cerr << "out of memory" << std::endl; \
        exit(1)

#endif

#include <cstddef>
#include <cstdlib>
#include <new>

// 一级配置器
class __malloc_alloc
{
public:
    static void *allocate(size_t);
    static void *reallocate(void *, size_t);
    static void deallocate(void *);
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

void *__malloc_alloc::reallocate(void *p, size_t n)
{
    void *res = realloc(p, n);
    if (res == nullptr)
        res = oom_reallocate(p, n);
    return res;
}

void __malloc_alloc::deallocate(void *p)
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


enum { __ALIGN = 8 };                           //空间以8对齐
enum { __MAX_BYTES = 512 };                     //小区块空间最大上界
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };  //小区块的数量

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
    //假设n以上调至__ALIG的倍数，至少返回一个大小为n的对象并为free list新增节点
    static void* re_fill(size_t n);
                                        //配置一大块区域，理想情况为n个size的空间，n可用改变
    static void* chunk_alloc(size_t size, int& n);

    static char* memory_start;          //起始地址
    static char* memory_end;            //结束地址
    static size_t memory_size;          //堆栈大小

public:
    static void* allocate(size_t);
    static void deallocate(void*);
    static void* reallocate(void*, size_t);
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

size_t __default_alloc::FREELIST_INDEX(size_t sz)   //sz 需要 >0
{
    return (sz + __ALIGN - 1) / __ALIGN - 1;        //下标从0开始
}



#endif