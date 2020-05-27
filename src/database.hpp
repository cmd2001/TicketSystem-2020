#ifndef DATABASE_CLASS_HPP
#define DATABASE_CLASS_HPP
#include <iostream>

template<typename T>
class List {
public:
    class iterator {
    public:
        iterator operator++() { return *this; }
        bool operator!=(const iterator &o) const {return 0;}
        T operator*() {return T();}
    };
    iterator begin() const {return iterator();}
    iterator end() const {return iterator();}
    void push_back(const T &x) {}
    size_t size() const {return 0;}
};

template<typename Key, typename T>
class Database {
public:
    Database() {}
    void insert(const Key &k, const T &t) {}
    void erase(const Key &k, const T &t) {}
    std::pair<bool, T> query(const Key &k) { return std::make_pair(1, T()); }
    void modify(const Key &k, const T & t1, const T &t2) {}
    List<T> range(const Key &k1, const Key &k2) { return List(); }
    void clear() {}
};

#endif