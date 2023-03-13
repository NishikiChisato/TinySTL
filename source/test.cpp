#include <iostream>

#include "alloc.h"
#include "iterator.h"

#include <vector>

void print(mystl::__true_type)
{
    std::cout << "YES" << std::endl;
}

int main()
{
    print(mystl::__type_traits<int>::is_POD_type());

    std::vector<int>v = {1,2,3,4,5,6,7,8,9};

    std::vector<int>::iterator it = v.begin();

    std::cout << *it << std::endl;

    mystl::advance(it, 2);

    std::cout << *it << std::endl;

    return 0;
}