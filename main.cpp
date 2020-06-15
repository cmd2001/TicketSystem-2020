#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include "include/List.h"
#include "BPlusTree/database_test.hpp"
#include "BPlusTree/database.hpp"
int main(){
    std::cout<<"cmd2001nb!"<<std::endl;
    database<int,int> a("123123");
    for(int i=1;i<=1233;++i){
        a.insert(i,i+2);
    }
    a.modify(5,14);
    int qwer;
    for(int i=13;i<=450;++i){
        qwer=a.erase(i);
    }
    std::cout<<a.query(5).first<<" "<<a.query(5).second<<std::endl;
    std::cout<<a.query(4).first<<" "<<a.query(4).second<<std::endl;
    List<int> test=a.range(2,610);
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
    std::cout<<(x==(--y))<<std::endl;
    a.clear();

}