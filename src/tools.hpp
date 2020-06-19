#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>

namespace MLJ {

int _strcmp_(const char *a, const char *b) {
    int len = (strlen(a) < strlen(b))? strlen(b) : strlen(a);
    for(int i = 0; i < len; ++i) {
        if((int)a[i] < (int)b[i]) return -1;
        else if((int)a[i] > (int)b[i]) return 1;
        else continue;
    }
    return 0;
}

// 按照ch分隔字符串，参考python中的 a = s.split(' ');
void split(const std::string &src, std::string *arg, int &cnt, const char &ch) {
    int i;
    cnt = i = 0;
    while(i < src.size() && src[i] == ch) ++i;
    for(; i < src.size(); ++i) {
        if(src[i] == ch) {
            while(i < src.size() && src[i] == ch) ++i;
            if(i < src.size()) ++cnt;
        }
        if(i < src.size()) arg[cnt] = arg[cnt] + src[i];
    }
    ++cnt;
}

template<typename T, typename comparator>
void quick_sort(T *head, T *tail, comparator cmp) {
    if(head < tail) {
        T p = *head;
        T *i = head, *j = tail;
        while(i < j) {
            while(i < j && !cmp(*j, p)) --j;
            if(i < j) *i++ = *j;
            while(i < j && cmp(*i, p)) ++i;
            if(i < j) *j-- = *i;
        }
        *i = p;
        quick_sort(head, i - 1, cmp);
        quick_sort(i + 1, tail, cmp);
    }
}

template<typename T, typename comparator>
void merge(T head[], int l, int mid, int r, comparator cmp) {
    T *tmp = new T[r - l + 1];
    int i = l, j = mid, k = 0;
    while(i < mid && j <= r) {
        if(cmp(head[i], head[j])) tmp[k++] = head[i++];
        else tmp[k++] = head[j++];
    }
    while(i < mid) tmp[k++] = head[i++];
    while(j <= r) tmp[k++] = head[j++];
    for(i = 0, k = l; k <= r; ) head[k++] = tmp[i++];
    delete [] tmp;
}

template<typename T, typename comparator>
void merge_sort(T head[], int l, int r, comparator cmp) {
    int mid = (l + r) >> 1;
    if(l >= r) return;
    merge_sort(head, l, mid, cmp);
    merge_sort(head, mid + 1, r, cmp);
    merge(head, l, mid + 1, r, cmp);
}

enum sort_ways { quicksort, mergesort };
template<typename T, typename comparator>
void sort(T *head, T *tail, comparator cmp, sort_ways way) {
    switch(way) {
    case quicksort: quick_sort(head, tail - 1, cmp); break;
    case mergesort: merge_sort(head, 0, tail - head - 1, cmp); break;
    }
}

}

#endif