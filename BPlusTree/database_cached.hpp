#ifndef TICKETSYSTEM_2020_DATABASE_CACHED_HPP
#define TICKETSYSTEM_2020_DATABASE_CACHED_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "database.hpp"
#include "../include/List.hpp"
#define debug cerr
using namespace std;

namespace mapA { // from https://github.com/battlin6/My_STLite/blob/master/mapA/map.hpp
    struct invalid_iterator {};
    struct index_out_of_bound{};
    template<class Key,class T,class Compare = std::less<Key>>
    class map {
    public:
        typedef pair<const Key, T> value_type;

        template <class U>
        void swap(U &a,U &b){
            U temp=a;
            a=b;
            b=temp;
        }  //define a swap function

    private:
        enum ColorType{Red,Black};
        struct Node{
            Node *prev,*next;
            Node *fa,*child[2];
            ColorType color;
            value_type *value;
            bool ok;  //ok==0 is left child, ok==1 is right child

            Node():value(nullptr){}
            Node(Node *other, Node* fa,Node *prev,Node *next):
                    value(new value_type(*(other->value))),fa(fa),
                    prev(prev),next(next),ok(other->ok),color(other->color){
                child[0]=child[1]= nullptr;
                prev->next=next->prev=this;
                if(other->child[0]!= nullptr)
                    child[0]= new Node(other->child[0],this,prev,this);
                //else child[0]= nullptr;
                if(other->child[1]!= nullptr)
                    child[1]= new Node(other->child[1],this,this,next);
                //else child[1]= nullptr;
            }
            Node(const value_type &value,Node *prev,Node* next,Node *fa,Node *lc,Node *rc,bool ok)
                    :value(new value_type(value)),prev(prev),next(next),fa(fa),ok(ok),color(Red){
                child[0]=lc,child[1]=rc;
                complish();
            }
            Node(const Key &key,Node *prev,Node *next,Node* fa,Node *lc,Node* rc,bool ok)
                    :Node(value_type(key,T()),prev,next,fa,lc,rc,ok){}

            void complish(){
                next->prev = prev->next = this;
                if(fa!= nullptr) fa->child[ok]=this;
                if(child[0]!= nullptr){
                    child[0]->fa=this;
                    child[0]->ok=0;
                }
                if(child[1]!= nullptr){
                    child[1]->fa=this;
                    child[1]->ok=1;
                }
            }
            Node *brother(){
                return fa->child[!ok];
            }
            ~Node(){
                delete value;
            }
        };

    public:
        class const_iterator;
        class iterator {
            friend void map::erase(iterator);
            friend const_iterator;
        private:
            map *nowMap;
            Node *nowNode;
        public:
            iterator():nowMap(nullptr),nowNode(nullptr){}
            iterator(map *a1, Node *a2):nowMap(a1),nowNode(a2){};
            iterator(const iterator &other)= default;
            iterator &operator=(const iterator &other)= default;
            operator const_iterator(){
                return const_iterator(*this);
            }

            iterator operator++(int) {
                iterator now(*this);
                operator++();
                return now;
            }

            iterator & operator++() {
                if(nowNode== nullptr||nowNode==nowMap->tail) throw invalid_iterator();
                nowNode=nowNode->next;
                return *this;
            }

            iterator operator--(int) {
                iterator now(*this);
                operator--();
                return now;
            }

            iterator & operator--() {
                if(nowNode== nullptr||nowNode==nowMap->head->next) throw invalid_iterator();
                nowNode=nowNode->prev;
                return *this;
            }

            value_type & operator*() const {
                return *operator->();
            }

            bool operator==(const iterator &rhs) const {
                return nowNode==rhs.nowNode;
            }
            bool operator==(const const_iterator &rhs) const {
                return nowNode==rhs.nowNode;
            }

            bool operator!=(const iterator &rhs) const {
                return nowNode!=rhs.nowNode;
            }
            bool operator!=(const const_iterator &rhs) const {
                return nowNode!=rhs.nowNode;
            }
            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                //if(nowNode== nullptr||nowNode==nowMap->tail) throw invalid_iterator();
                return nowNode->value;
            }
        };
        class const_iterator {
            friend iterator;
        private:
            const map *nowMap;
            const Node *nowNode;
        public:
            const_iterator():nowMap(nullptr),nowNode(nullptr){}
            const_iterator(const map* a1,const Node *a2):nowMap(a1),nowNode(a2){}
            const_iterator(const const_iterator &other) = default;
            explicit const_iterator(const iterator &other):nowMap(other.nowMap),nowNode(other.nowNode){}
            const_iterator &operator=(const const_iterator &other) = default;

            const_iterator operator++(int) {
                const_iterator now(*this);
                operator++();
                return now;
            }

            const_iterator & operator++() {
                if(nowNode== nullptr||nowNode==nowMap->tail) throw invalid_iterator();
                nowNode=nowNode->next;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator now(*this);
                operator--();
                return now;
            }

            const_iterator & operator--() {
                if(nowNode== nullptr||nowNode==nowMap->head->next) throw invalid_iterator();
                nowNode=nowNode->prev;
                return *this;
            }

            value_type & operator*() const {
                return *operator->();
            }

            bool operator==(const iterator &rhs) const {
                return nowNode==rhs.nowNode;
            }
            bool operator==(const const_iterator &rhs) const {
                return nowNode==rhs.nowNode;
            }

            bool operator!=(const iterator &rhs) const {
                return nowNode!=rhs.nowNode;
            }
            bool operator!=(const const_iterator &rhs) const {
                return nowNode!=rhs.nowNode;
            }

            value_type* operator->() const noexcept {
                //if(nowNode== nullptr||nowNode==nowMap->tail) throw invalid_iterator();
                return nowNode->value;
            }
        };

    private:
        Node *head,*tail, *root;
        size_t curSize;
        Compare compare_func;

        static void link(Node *a,Node *b){
            a->next=b;
            b->prev=a;
        } //link the prev and next;

        void Swap_Node(Node* a,Node* b){
            if(root==a)
                root=b;
            else if(root==b)
                root=a;

            swap(a->prev,b->prev);
            swap(a->next,b->next);
            swap(a->fa,b->fa);
            swap(a->child[0],b->child[0]);
            swap(a->child[1],b->child[1]);
            swap(a->color,b->color);
            swap(a->ok,b->ok);

            // problem?? if a is close to b
            if (a->prev == a) a->prev = b;
            if (a->next == a) a->next = b;
            if (a->fa == a) a->fa = b;
            if (a->child[0] == a) a->child[0] = b;
            if (a->child[1] == a) a->child[1] = b;

            if (b->prev == b) b->prev = a;
            if (b->next == b) b->next = a;
            if (b->fa == b) b->fa = a;
            if (b->child[0] == b) b->child[0] = a;
            if (b->child[1] == b) b->child[1] = a;


            a->complish(),b->complish();
        }//swap two Node in a map

        void Rotate(Node *x, bool ok){
            Node *y =x->child[!ok];
            if(root==x) root=y;

            x->child[!ok]=y->child[ok];
            y->fa=x->fa;
            y->ok=x->ok;
            x->fa=y;
            x->ok=ok;

            x->complish(),y->complish();
        }//rotate

        void up(Node *x){
            Node *fa=x->fa;
            if(fa== nullptr){
                x->color=Black;
                return;
            }

            if(fa->color==Black) return;

            Node *grandpa=fa->fa;
            Node *uncle=x->fa->brother();

            if(uncle== nullptr||uncle->color==Black){
                if(x->ok==fa->ok){
                    fa->color=Black;
                    grandpa->color=Red;
                    Rotate(grandpa,!(x->ok));
                }else{
                    x->color=Black;
                    grandpa->color=Red;
                    Rotate(fa,!(x->ok));
                    Rotate(grandpa,!(x->ok));
                }
            }else{
                fa->color=uncle->color =Black;
                grandpa->color=Red;
                up(grandpa);
            }
        }

        void down(Node *x, bool cas=false){
            if(x->color==Red && !cas) return;

            Node *child =x->child[x->child[0]== nullptr];   //get a non-nullptr

            if(child!= nullptr && child->color==Red && !cas){
                child->color=Black;
                return;
            }

            if(root==x){
                x->color=Black;
                return;
            }

            Node *fa=x->fa;
            Node *brother=x->brother();
            Node **cousin=brother->child;  //two side array

            if(fa->color==Black && brother->color ==Black &&
               (cousin[0] == nullptr || cousin[0]->color == Black) &&
               (cousin[1] == nullptr || cousin[1]->color == Black)){
                brother->color= Red;
                down(fa,true);
                return;
            }

            if(brother->color== Red){
                fa->color=Red;
                brother->color=Black;
                Rotate(fa,x->ok);
                brother=x->brother();
                cousin=brother->child;
            }

            if(fa->color==Red && brother->color==Black &&
               (cousin[0] == nullptr || cousin[0]->color == Black) &&
               (cousin[1] == nullptr || cousin[1]->color == Black)){
                fa->color=Black;
                brother->color=Red;
                return;
            }

            if(cousin[!(x->ok)]== nullptr || cousin[!(x->ok)]->color==Black){
                cousin[x->ok]->color=Black;
                brother->color=Red;
                Rotate(brother ,!(x->ok));
                brother=x->brother();
                cousin=brother->child;
            }

            swap(fa->color,brother->color);
            cousin[!(x->ok)]->color=Black;
            Rotate(fa,x->ok);
        }

        template <class O>
        pair<Node *,bool> Insert(const Key &key,const O &value){
            if(root== nullptr){
                Node *newNode = new Node (value,head,tail, nullptr, nullptr, nullptr,0);
                root = newNode;
                curSize++;
                up(newNode);
                return {newNode,true};
            }

            Node *curNode=root;
            bool cas;
            while(true){
                cas = compare_func(curNode->value->first,key);
                if(!cas && !compare_func(key,curNode->value->first)) return {curNode,false};
                if(curNode->child[cas]== nullptr) break;
                curNode=curNode->child[cas];
            }

            Node *newNode = new Node(value,cas?curNode:curNode->prev,cas?curNode->next:curNode,curNode, nullptr, nullptr,cas);
            curSize++;
            up(newNode);
            return {newNode,true};
        }

        pair<Node*,bool> Insert(const value_type &value){
            return Insert(value.first,value);
        }

        Node* Insert(const Key &key){
            return Insert(key,key).first;
        }

        void Erase(Node *x){
            curSize--;
            if(x->child[0]!= nullptr && x->child[1]!= nullptr){
                Swap_Node(x,x->next);
            }   //x->next must be a leave
            down(x);
            link(x->prev,x->next);

            Node *child=x->child[x->child[0]== nullptr];
            if(x==root) root=child;
            else x->fa->child[x->ok]=child;

            if(child!= nullptr){
                child->fa=x->fa;
                child->ok=x->ok;
            }

            delete x;
        }

    public:
        map():head(new Node),tail(new Node),root(nullptr),curSize(0){
            head->next=tail;
            tail->prev=head;
        }
        map(const map &other):map(){
            if(other.curSize==0) return;
            curSize=other.curSize;
            root= new Node(other.root, nullptr,head,tail);
        }

        map & operator=(const map &other) {
            if(this==&other) return *this;
            clear();
            curSize=other.curSize;
            if(other.curSize==0) return *this;
            root= new Node (other.root, nullptr,head,tail);
            return *this;
        }

        ~map() {
            clear();
            delete head;
            delete tail;
        }

        T & at(const Key &key) {
            iterator now= find(key);
            if(now==end()) throw index_out_of_bound();
            return now->second;
        }
        const T & at(const Key &key) const {
            const_iterator now= find(key);
            if(now==cend()) throw index_out_of_bound();
            return now->second;
        }
        /**
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            return Insert(key)->value->second;
        }

        const T & operator[](const Key &key) const {
            return at(key);
        }

        iterator begin() {
            return iterator(this,head->next);
        }
        const_iterator cbegin() const {
            return const_iterator(this,head->next);
        }
        iterator end() {
            return iterator(this,tail);
        }
        const_iterator cend() const {
            return const_iterator(this,tail);
        }

        bool empty() const {
            return curSize==0;
        }

        size_t size() const {
            return curSize;
        }

        void clear() {
            curSize=0;
            Node * curNode =head->next;
            while(curNode!=tail){
                Node *tmp=curNode;
                curNode=curNode->next;
                delete tmp;
            }
            head->next=tail;
            tail->prev=head;
            root= nullptr;
        }

        pair<iterator, bool> insert(const value_type &value) {
            auto result=Insert(value);
            return {iterator(this,result.first),result.second};
        }

        void erase(iterator pos) {
            if(pos.nowMap!=this||pos==end()) throw invalid_iterator();
            Erase(pos.nowNode);
        }

        size_t count(const Key &key) const {
            return find(key)==cend()?0:1;
        }

        iterator find(const Key &key) {
            Node* curNode=root;
            while(curNode!= nullptr){
                if(compare_func(key,curNode->value->first))
                    curNode=curNode->child[0];
                else if(compare_func(curNode->value->first,key))
                    curNode=curNode->child[1];
                else
                    return iterator(this,curNode);
            }
            return end();
        }
        iterator lower_bound(const Key &key) { // lower_bound by Amagi_Yukisaki.
            Node *curNode=root, *mem = nullptr;
            while(curNode!= nullptr){
                if(!compare_func(curNode->value->first,key)) mem = curNode;
                if(compare_func(key,curNode->value->first)) curNode=curNode->child[0];
                else if(compare_func(curNode->value->first,key)) curNode=curNode->child[1];
                else return iterator(this,curNode);
            }
            return mem == nullptr ? end() : iterator(this, mem);
        }

        const_iterator find(const Key &key) const {
            Node* curNode=root;
            while(curNode!= nullptr){
                if(compare_func(key,curNode->value->first))
                    curNode=curNode->child[0];
                else if(compare_func(curNode->value->first,key))
                    curNode=curNode->child[1];
                else
                    return const_iterator(this,curNode);
            }
            return cend();
        }
    };

}

namespace __Amagi {
    constexpr size_t max_Cache_Size = 30;

    template <typename type_key,typename type_value>
    class database_cached {
    private:
        database<type_key, type_value> core;
        struct cache_Node {
            int type; // 0 for read, 1 for insert, 2 for modify, 3 for delete
            size_t quePos;
            type_value value;
            cache_Node(const int &_type, const size_t &_quePos, const type_value &_value): type(_type), quePos(_quePos), value(_value) {}
            cache_Node() = default;
        };
        /* read + read = impossible, read + insert = insert, insert + modify = modify, read + delete = delete
         * insert + read = impossible, insert + insert = impossible, insert + modify = INSERT, insert + delete = NOTHING
         * modify + read = impossible, modify + insert = impossible, modify + modify = modify, modify + delete = delete
         * delete + read = impossible, delete + insert = modify, delete + modify = impossible, delete + delete = impossible
         */
        int type_table[4][4] = {{-1, 1, 2, 3}, {-1, -1, 1, 4}, {-1, -1, 2, 3}, {-1, 2, -1, -1}};
        mapA::map<type_key, cache_Node> cache;
        mapA::map<size_t, type_key> que;
        size_t uid, cur_size;
        void popQue() {
            auto x = que.begin()->second;
            que.erase(que.begin());
            assert(cache.find(x) != cache.end());
            auto y = cache[x];
            cache.erase(cache.find(x));
            assert(que.size() == cache.size());
            if(y.type == 0) return; // nothing to do.
            else if(y.type == 1) core.insert(x, y.value);
            else if(y.type == 2) core.modify(x, y.value);
            else if(y.type == 3) core.erase(x);
        }
        void pushQue(const type_key &key, const type_value &value, const int &type) {
            if(cache.find(key) != cache.end()) {
                auto y = cache[key];
                assert(que.find(y.quePos) != que.end());
                que.erase(que.find(y.quePos));
                int new_type = type_table[y.type][type];
                assert(new_type != -1);
                if(new_type == 4) {
                    cache.erase(cache.find(key));
                    return;
                }
                y = cache_Node(new_type, ++uid, value);
                cache[que[uid] = key] = y;
            } else {
                if(cache.size() == max_Cache_Size) popQue();
                assert(que.size() == cache.size());
                que[++uid] = key;
                cache[key] = cache_Node(type, uid, value);
            }
        }
        List<pair<type_key, type_value> > cache_Range(const type_key &k1, const type_key &k2) {
            List<pair<type_key, type_value> > ret;
            auto cur = cache.lower_bound(k1);
            while(cur != cache.end() && !(k2 < cur->first)) {
                if(cur->second.type == 1) ret.push_back(make_pair(cur->first, cur->second.value));
                ++cur;
            }
            return ret;
        }
        List<type_value> merge_Sort(List<pair<type_key, type_value> > &a, List<pair<type_key, type_value> > b) {
            List<type_value> ret;
            auto p1 = a.begin(), p2 = b.begin();
            while(p1 != a.end() && p2 != b.end()) {
                if((*p1).first < (*p2).first) ret.push_back((*p1).second), ++p1;
                else ret.push_back((*p2).second), ++p2;
            }
            while(p1 != a.end()) ret.push_back((*p1).second), ++p1;
            while(p2 != b.end()) ret.push_back((*p2).second), ++p2;
            return ret;
        }
        List<pair<type_key, type_value> > merge_Sort2(List<pair<type_key, type_value> > &a, List<pair<type_key, type_value> > b) {
            List<pair<type_key, type_value> > ret;
            auto p1 = a.begin(), p2 = b.begin();
            while(p1 != a.end() && p2 != b.end()) {
                if((*p1).first < (*p2).first) ret.push_back(*p1++);
                else ret.push_back(*p2++);
            }
            while(p1 != a.end()) ret.push_back(*p1++);
            while(p2 != b.end()) ret.push_back(*p2++);
            return ret;
        }
    public:
        string __name;
        database_cached(const string &s): core(s), uid(0), __name(s) {cur_size = core.size();}
        void insert(const type_key &key, const type_value &value) {
            if(!max_Cache_Size) return core.insert(key, value); // defeat cache
            ++cur_size;
            pushQue(key, value, 1);
        }
        bool erase(const type_key &key) {
            if(!max_Cache_Size) return core.erase(key);
            pushQue(key, type_value(), 3);
            --cur_size;
            return 1; // assert key exists.
        }
        bool modify(const type_key &key, const type_value &value) {
            if(!max_Cache_Size) return core.modify(key, value);
            pushQue(key, value, 2);
            return 1; // assert key exists.
        }
        pair<bool, type_value> query(const type_key key) {
            if(!max_Cache_Size) return core.query(key);
            if(cache.find(key) != cache.end()) { // found in cache
                auto y = cache[key];
                if(y.type == 3) return make_pair(false, type_value()); // erased
                return make_pair(true, y.value);
            } else { // not found in cache
                auto ret = core.query(key);
                if(ret.first) pushQue(key, ret.second, 0);
                return ret;
            }
        }
        List<type_value> range(const type_key &k1, const type_key &k2) {
            if(!max_Cache_Size) return core.range(k1, k2);
            auto ref = core.range2(k1, k2);
            List<pair<type_key, type_value> > ret;
            for(auto x: ref) {
                if(cache.find(x.first) == cache.end()) ret.push_back(x);
                else {
                    auto tp = cache[x.first];
                    assert(tp.type != 1) ;
                    if(tp.type != 3) ret.push_back(make_pair(x.first, tp.value));
                }
            }
            return merge_Sort(ret, cache_Range(k1, k2));
        }
        List<pair<type_key, type_value> > range2(const type_key &k1, const type_key &k2) {
            if(!max_Cache_Size) return core.range2(k1, k2);
            auto ref = core.range2(k1, k2);
            List<pair<type_key, type_value> > ret;
            for(auto x: ref) {
                if(cache.find(x.first) == cache.end()) ret.push_back(x);
                else {
                    auto tp = cache[x.first];
                    assert(tp.type != 1) ;
                    if(tp.type != 3) ret.push_back(make_pair(x.first, tp.value));
                }
            }
            return merge_Sort2(ret, cache_Range(k1, k2));
        }
        size_t size() {
            if(!max_Cache_Size) return core.size();
            return cur_size;
        }
        bool empty() {
            if(!max_Cache_Size) return core.empty();
            return !size();
        }
        void clear() {
            if(!max_Cache_Size) return core.clear();
            core.clear(), cache.clear(), que.clear(), cur_size = 0;
        }
        ~database_cached() { // write back all cached data
            for(auto x: cache) {
                if(x.second.type == 0) continue;
                else if(x.second.type == 1) core.insert(x.first, x.second.value);
                else if(x.second.type == 2) core.modify(x.first, x.second.value);
                else if(x.second.type == 3) core.erase(x.first);
            }
        }
    };
}

#endif