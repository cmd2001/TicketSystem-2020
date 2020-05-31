#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include "include/List.h"
#include "BPlusTree/database_test.hpp"
int main(){
    std::cout<<"cmd2001nb!"<<std::endl;
    database_test<int,int> a("123123");
    a.insert(1,1);
    a.insert(5,3);
    a.insert(3,4);
    a.modify(3,4,8);
    a.insert(4,12);
    a.erase(1,1);
    std::cout<<a.query(5).first<<" "<<a.query(5).second<<std::endl;
    std::cout<<a.query(1).first<<" "<<a.query(1).second<<std::endl;
    List<int> test=a.range(1,6);
    std::cout<<test<<std::endl;
    std::cout<<test.size()<<std::endl;
    std::cout<<"???"<<std::endl;
    List<int>::iterator x=test.begin();
    std::cout<<*x<<std::endl;
    x++;
    std::cout<<*x<<std::endl;
    List<int>::iterator y=test.end();
    --y;
    std::cout<<*y<<std::endl;
    --y;
    std::cout<<(x==y)<<std::endl;
    a.clear();
}