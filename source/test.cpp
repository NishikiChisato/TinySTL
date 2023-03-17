#include <iostream>

#include "alloc.h"
#include "iterator.h"
#include "uninitialized.h"
#include "construct.h"

#include <vector>

void print(mystl::__true_type)
{
    std::cout << "YES" << std::endl;
}

int main()
{

#if 0
    print(mystl::__type_traits<int>::is_POD_type());

    std::vector<int>v = {1,2,3,4,5,6,7,8,9};

    std::vector<int>::iterator it = v.begin();

    std::cout << *it << std::endl;

    mystl::advance(it, 2);

    std::cout << *it << std::endl;
#endif

#if 0
    std::vector<int>iv(5);
    mystl::uninitialized_fill_n(iv.begin(), 5, 10);

    std::cout << iv.size() << std::endl;

    for(auto x : iv)
        std::cout << x << " ";
    std::cout << std::endl;

    std::vector<int>ic(5);

    mystl::uninitialized_fill(ic.begin(), ic.end(), 6);

    for(auto x : ic)
        std::cout << x << " ";

    std::cout << std::endl;
#endif
    

    std::vector<int> iv;
    

    return 0;
}