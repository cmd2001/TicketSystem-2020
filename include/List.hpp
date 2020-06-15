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
        Node *prev;
    };
public:
    Node* head;
    Node* tail;
private:
    std::size_t curSize;

public:
    List():head(new Node),tail(new Node){
        head->next=tail;
        tail->prev=head;
        curSize=0;
    }
    // List(const List &o): List() {
    //     Node *p = o.head->next;
    //     while(p != o.tail) {
    //         Node *tmp = new Node;
    //         tmp->value = p->value;
    //         tmp->next = tail;
    //         tmp->prev = tail->prev;
    //         tail->prev->next = tmp;
    //         tail->prev = tmp;
    //     }
    //     curSize = o.curSize;
    // }
    List(List &&o) {
        head = o.head;
        tail = o.tail;
        curSize = o.curSize;
        o.head = o.tail = nullptr;
    }
    List &operator=(List &&o) {
        this->~List();
        head = o.head;
        tail = o.tail;
        curSize = o.curSize;
        o.head = o.tail = nullptr;
        return *this;
    }
    ~List(){
        if(head == nullptr && tail == nullptr) return;
        Node *x=head->next;
        while(x!=tail){
            Node* tmp=x;
            x=x->next;
            delete tmp;
        }
        curSize=0;
        delete head;
        delete tail;
    }
    void push_back(const K &v){
        auto news=new Node;
        news->value=v;

        news->next=head->next;
        news->prev=head;
        head->next->prev=news;
        head->next=news;

        curSize++;
    }
    size_t size()const{
        return curSize;
    };

    bool empty()const{
        return curSize==0;
    }

public:
    class iterator{
    private:
        Node* nowNode;
    public:
        iterator() = default;
        iterator(const iterator &other)= default;
        iterator(Node *_Node):nowNode(_Node){}
    public:
        bool operator==(const iterator &other) const {
            return nowNode==other.nowNode;
        }

        bool operator!=(const iterator&other) const {
            return nowNode!=other.nowNode;
        }
        K &operator*() const {
            return (nowNode->value);
        }

        const iterator operator++(int) {
            auto x = iterator(*this);
            nowNode = nowNode->prev;
            return x;
        }
        const iterator operator--(int) {
            auto x = iterator(*this);
            nowNode = nowNode->next;
            return x;
        }

        iterator &operator++() {
            nowNode = nowNode->prev;
            return *this;
        }
        iterator &operator--() {
            nowNode = nowNode->next;
            return *this;
        }

    };

    iterator begin(){
        return iterator(tail->prev);
    }

    /*end() points to the next of the last element
     * */
    iterator end(){
        return iterator(head);
    }
};

/*
 * template class ostream refers to https://www.cnblogs.com/xkfz007/articles/2534322.html
 * */
template <typename K>
ostream& operator<<(ostream & os, const List<K> & L) {
    auto x = L.tail->prev;
    while (x != L.head) {
        os << x->value << " ";
        x = x->prev;
    }
    return os;
}
#endif //TICKETSYSTEM_2020_LIST_H
