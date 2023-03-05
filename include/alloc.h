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




#endif