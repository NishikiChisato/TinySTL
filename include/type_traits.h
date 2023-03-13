#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

namespace mystl
{

/**
 * 本头文件用于判断是否含义trivial函数，如trivial default ctor, trivial copy ctor, trivial assignment operator
 * trivial dtor以及POD
 */

//用结构体类型进行判断，使得在编译期便确定其类型
struct __true_type { };
struct __false_type { };

template <typename type>
struct __type_traits
{
    //该成员需要在第一个，用于确保编译器看到一个与此处__type_traits同名但无任何关联的template时正常工作
    typedef     __true_type     dummy_mumber;

    //以下定义均为保守定义，你可以为每个型别进行特化处理
    typedef     __false_type    has_trivial_default_constructor;
    typedef     __false_type    has_copy_constructor;
    typedef     __false_type    has_trivial_copy_assignment_operator;
    typedef     __false_type    has_trivial_destructor;
    typedef     __false_type    is_POD_type;
};

//以下针对C++所有基本型别char, signed char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long
//float, double, long double, long long进行特化处理

template <> struct __type_traits <char>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <signed char>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <unsigned char>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <short>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <unsigned short>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <int>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <unsigned int>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <long>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <unsigned long>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <float>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <double>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <long double>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

template <> struct __type_traits <long long>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};

//针对原生指针的特化版本
template <typename T> 
struct __type_traits <T*>
{
    typedef     __true_type     has_trivial_default_constructor;
    typedef     __true_type     has_copy_constructor;
    typedef     __true_type     has_trivial_copy_assignment_operator;
    typedef     __true_type     has_trivial_destructor;
    typedef     __true_type     is_POD_type;
};








}


#endif