#ifndef TICKETSYSTEM_2020_LIST_H
#define TICKETSYSTEM_2020_LIST_H


#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
using namespace std;

template <typename K>
class List{
public:
    struct Node{
        K value;
        Node *next;
    };
public:
    Node* head;

public:
    List():head(nullptr){};
    ~List(){
        Node *x=head;
        while(x!= nullptr){
            Node* tmp=x;
            x=x->next;
            delete tmp;
        }
    }
    void push_back(K v){
        auto news=new Node;
        news->value=v;
        news->next=head;
        head=news;
    }
};

/*
 * template class ostream refers to https://www.cnblogs.com/xkfz007/articles/2534322.html
 * */
template <typename K>
ostream& operator<<(ostream & os, const List<K> & L)
{
    auto x=L.head;
    while(x!= nullptr){
        os<<x->value<<" ";
        x=x->next;
    }
    //os<<endl;
    return os;
}
#endif //TICKETSYSTEM_2020_LIST_H
