#ifndef TICKETSYSTEM_2020_DATABASE_H
#define TICKETSYSTEM_2020_DATABASE_H


#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <fstream>
#include "../include/List.hpp"

template <typename Key>
bool isequal(const Key&a,const Key &b){
    return !((a<b)||(b<a));
}

template <typename Key,typename Value>
class database{
private:
    string Filename;
    static const int maxKeyNum=16;
    static const int miniKeyNum=4;
    static const int MaxSize=maxKeyNum+2;
    std::fstream Fileio;

private:
    /*key number is 0-base
     * */
    struct idxNode{
        Key key[MaxSize],miniKey;
        int isLeaf;
        int offset[MaxSize],keyNum;

        idxNode(){
            isLeaf=false;
            keyNum=-1;
        }
    };
    struct dataNode{
        Key key[MaxSize];
        Value data[MaxSize];
        int offset,nextoffset,keyNum;
        dataNode(){
            keyNum=0;
            nextoffset=-1;
        }
        dataNode(const dataNode &o) = default;
    };

    const int idxNodeSize=sizeof(idxNode);
    const int dataNodeSize=sizeof(dataNode);

private:
    /*关于leftHand：
     * 本来我是想实现类似于迭代器的dataNode实现
     * 并且以leftHand作为begin
     * 然后发现没有什么用.......
     * 可以在文件中忽略掉它
     * */
    idxNode *root;
    dataNode *leftHead;
    int _offset;
    int curSize;

public:
    database():root(nullptr),leftHead(nullptr),_offset(0),curSize(0){}
    /*
     * reference https://blog.csdn.net/hcf999/article/details/77864456
     * 可以判断几种不同模式下是否自动创建文件以及open的成败与否
     * */
    database(const string &s):Filename(s){
        Fileio.open(Filename,ios::binary|ios::ate|ios::in|ios::out);
        //这种打开在文件不存在的情况下会open失败

        if(Fileio.is_open()){  //the file exists
            //read the message from an existing file

            leftHead=new dataNode;
            root=new idxNode;

            Fileio.seekg(0, std::ios::beg);
            Fileio.read(reinterpret_cast<char *>(&curSize), sizeof(int));
            Fileio.read(reinterpret_cast<char *>(&_offset), sizeof(int));
            Fileio.read(reinterpret_cast<char *>(leftHead), dataNodeSize);
            Fileio.read(reinterpret_cast<char *>(root), idxNodeSize);
        }else{
            leftHead=new dataNode;
            root=new idxNode;
            _offset=2*sizeof(int)+dataNodeSize + idxNodeSize;
            curSize=0;

            Fileio.open(Filename,ios::out);
            Fileio.close();  //create the file

            Fileio.open(Filename,ios::binary|ios::ate|ios::in|ios::out);
            Fileio.seekp(0, std::ios::beg);
            Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
            Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
            Fileio.write(reinterpret_cast<char *>(leftHead), dataNodeSize);
            Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);
            Fileio.flush();
        }
    }
    ~database(){
        Fileio.seekp(0, ios::beg);
        Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
        Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
        //File.write(reinterpret_cast<char *>(leftHead), dataNodeSize);   //no use
        Fileio.seekp(sizeof(int) * 2 + dataNodeSize, std::ios::beg);
        Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);
        Fileio.flush();
        delete root;
        delete leftHead;
        Fileio.close();
    }

public:
    bool empty() const {
        return root->keyNum==-1;
    }
    size_t size() const {
        return curSize;
    }
    void clear(){
        delete root;
        delete leftHead;

        root=new idxNode;
        leftHead=new dataNode;
        _offset = 2 * sizeof(int) + dataNodeSize + idxNodeSize;
        curSize=0;

        if(Fileio.is_open())
            Fileio.close();
        Fileio.open(Filename, ios::out);
        Fileio.close();

        Fileio.open(Filename,ios::binary | ios::ate | ios::out | ios::in);
        Fileio.seekp(0, ios::beg);
        Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
        Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
        Fileio.write(reinterpret_cast<char *>(leftHead), dataNodeSize);
        Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);
        Fileio.flush();
    }

public:
    class Iterator {
        friend class database;

    private:
        database *tree_ptr;
        dataNode node;
        int pos;
    public:
        Iterator(database *a1 = nullptr, const dataNode &a2 = dataNode(), int a3 = -1)
        :tree_ptr(a1), node(a2),pos(a3){}
        Iterator(const database &other)
        :tree_ptr(other.tree_ptr), node(other.node), pos(other.pos){}

        Iterator operator++(int) {
            auto tmp = this;
            pos++;
            if (pos >= node.keyNum) {
                if (node.nextoffset == -1) {
                    pos = -1;
                } else {
                    tree_ptr->Fileio.seekg(node.nextoffset, ios::beg);
                    tree_ptr->Fileio.read(reinterpret_cast<char *>(&node), tree_ptr->dataNodeSize);
                    pos = 0;
                }
            }
            return *tmp;
        }

        Iterator &operator++() {
            pos++;
            if (pos >= node.keyNum) {
                if (node.nextoffset == -1) {
                    pos = -1;
                } else {
                    tree_ptr->File.seekg(node.nextoffset, ios::beg);
                    tree_ptr->File.read(reinterpret_cast<char *>(&node), tree_ptr->dataNodeSize);
                    pos = 0;
                }
            }
            return *this;
        }

        Value &operator*(){
            return node.value[pos];
        }
        Value *operator->(){
            return node.value+pos;
        }
        bool valid(){
            return pos>=0;
        }
        Key key(){
            return node.key[pos];
        }
        Value value(){
            return node.data[pos];
        }
    };

public:
    /*search a key through idx
     * */
    Iterator *idxSearch(const Key &k,const idxNode &t){
        int i;
        for (i = 0; i < t.keyNum; i++) {
            if (k < t.key[i])	break;
        }
        Fileio.seekg(t.offset[i], ios::beg);
        if (t.isLeaf) {
            dataNode p;
            Fileio.read(reinterpret_cast<char *>(&p), dataNodeSize);
            return dataSearch(k, p);
        }
        else {
            idxNode p;
            Fileio.read(reinterpret_cast<char *>(&p), idxNodeSize);
            return idxSearch(k, p);
        }
    }
    /*search a key in leaf
     * */
    Iterator *dataSearch(const Key &k,const dataNode &t){
        Iterator *tmp;
        for(int i=0;i<t.keyNum;++i){
            if(isequal(t.key[i],k)){
                tmp=new Iterator(this,t,i);
                return tmp;
            }
        }
        tmp=new Iterator(this,t,-1);
        return tmp;
    }
    /*search the first key >=k through idx
     * */
    Iterator *Lower_boundidx(const Key &k,const idxNode &t) {
        int i;
        for (i = t.keyNum - 1; i >= 0 && (k < t.key[i]||isequal(k,t.key[i])); --i);

        Fileio.seekg(t.offset[i + 1], ios::beg);
        if (t.isLeaf) {
            dataNode p;
            Fileio.read(reinterpret_cast<char *>(&p), dataNodeSize);
            return Lower_bounddata(k, p);
        } else {
            idxNode p;
            Fileio.read(reinterpret_cast<char *>(&p), idxNodeSize);
            return Lower_boundidx(k, p);
        }
    }
    /*search the first key >=k in leaf
     * in fact, in this function, it cannot find the right node
     * maybe the the right node-1
     * so I refine it in function "Lower_bound"
     * */
    Iterator *Lower_bounddata(const Key &k,const dataNode &t) {
        Iterator *tmp;
        for (int i = t.keyNum - 1; i >= 0; --i) {
            if (t.key[i] < k || isequal(t.key[i], k)) {
                tmp = new Iterator(this, t, i);
                return tmp;
            }
        }
        tmp = new Iterator(this, t, 0);
        return tmp;
    }
    /*insert a <key,value> through idx
     * */
    idxNode *Insertidx(const Key& k,const Value& data,idxNode *t){
        void *newNode;

        int i = 0;
        for (i = 0; i < t->keyNum; ++i)
            if (k < t->key[i])
                break;

        if (t->isLeaf) {
            Fileio.seekg(t->offset[i], ios::beg);
            auto *p = new dataNode;
            Fileio.read(reinterpret_cast<char *>(p), dataNodeSize);
            newNode = Insertdata(k, data, p);

            if (i == 0) t->miniKey = p->key[0];
            else t->key[i - 1] = p->key[0];

            Fileio.seekp(t->offset[i], ios::beg);
            Fileio.write(reinterpret_cast<char *>(p), dataNodeSize);
            Fileio.flush();
            delete p;
        }else{
            Fileio.seekg(t->offset[i], ios::beg);
            auto *p = new idxNode;
            Fileio.read(reinterpret_cast<char *>(p), idxNodeSize);
            newNode = Insertidx(k, data, p);

            if (i == 0) t->miniKey = p->miniKey;
            else t->key[i - 1] = p->miniKey;

            Fileio.seekp(t->offset[i], ios::beg);
            Fileio.write(reinterpret_cast<char *>(p), idxNodeSize);
            Fileio.flush();
            delete p;
        }
        if(newNode== nullptr) return nullptr;
        else{
            if(t->isLeaf) return addBlockdata((dataNode*)newNode,t);
            else return addBlockidx((idxNode*)newNode,t);
        }
    }
    /*insert a <key,value> in leaf
     * */
    dataNode* Insertdata(const Key& k,const Value& data,dataNode *t){
        int i = 0;
        for (i = 0; i < t->keyNum; ++i) {
            if (isequal(t->key[i],k)) {
                t->data[i] = data;
                return nullptr;
            }
        }  //complete the function of modify

        for (i = t->keyNum; i > 0 && k < t->key[i - 1]; i--) {
            t->key[i] = t->key[i - 1];
            t->data[i] = t->data[i - 1];
        }  //向右挪一位

        t->key[i] = k;
        t->data[i] = data;
        t->keyNum++;
        curSize++;
        //complete the data here

        Fileio.seekp(0, ios::beg);
        Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
//		Fileio.flush();

        if (t->keyNum <= maxKeyNum) return nullptr;
        else {
            //split
            int newKeyNum = maxKeyNum / 2;
            dataNode *newNode = new dataNode;
            newNode->keyNum = t->keyNum - newKeyNum;
            for (int j = 0; j < newNode->keyNum; j++) {
                newNode->key[j] = t->key[j + newKeyNum];
                newNode->data[j] = t->data[j + newKeyNum];
            } //copy

            t->keyNum = newKeyNum;
            newNode->nextoffset = t->nextoffset;
            t->nextoffset = _offset;
            newNode->offset = _offset;
            return newNode;
        }
    }
    /*when insert, add a idx block
     * */
    idxNode* addBlockidx(idxNode *newNode,idxNode *t){
        Key min = newNode->miniKey;

        int i;
        for (i = t->keyNum; i > 0 && min < t->key[i - 1]; --i) {
            t->key[i] = t->key[i - 1];
            t->offset[i + 1] = t->offset[i];
        }

        t->key[i] = min;
        t->offset[i + 1] = _offset;
        t->keyNum++;

        Fileio.seekp(_offset, ios::beg);
        Fileio.write(reinterpret_cast<char *>(newNode), idxNodeSize);
//			File.flush();
        _offset += idxNodeSize;  //update _offset
        Fileio.seekp(sizeof(int), ios::beg);
        Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
//			File.flush();

        if (t->keyNum <= maxKeyNum) {
            delete newNode;
            return nullptr;
        }
        else {
            //split
            int newKeyNum = maxKeyNum / 2;
            idxNode *newIdx = new idxNode;

            newIdx->isLeaf = 0;
            newIdx->keyNum = t->keyNum - 1 - newKeyNum;
            newIdx->miniKey = t->key[newKeyNum];
            for (int j = 0; j < newIdx->keyNum; j++) {
                newIdx->key[j] = t->key[j + newKeyNum + 1];
                newIdx->offset[j] = t->offset[j + newKeyNum + 1];
            }

            newIdx->offset[newIdx->keyNum] = t->offset[t->keyNum];
            t->keyNum = newKeyNum;

            delete newNode;
            return newIdx;
        }
    }
    /*when insert, add a data block
     * */
    idxNode* addBlockdata(dataNode *newNode,idxNode *t){
        int i;
        for (i = t->keyNum; i > 0 && newNode->key[0] < t->key[i - 1]; i--) {
            t->key[i] = t->key[i - 1];
            t->offset[i + 1] = t->offset[i];
        }

        t->key[i] = newNode->key[0];
        t->offset[i + 1] = _offset;
        t->keyNum++;

        Fileio.seekp(_offset, ios::beg);
        Fileio.write(reinterpret_cast<char *>(newNode), dataNodeSize);
        _offset += dataNodeSize;
        Fileio.seekp(sizeof(int), ios::beg);
        Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
//			File.flush();


        if (t->keyNum <= maxKeyNum) {
            delete newNode;
            return nullptr;
        }
        else {
            //split
            int newKeyNum = maxKeyNum / 2;
            idxNode *newIdx = new idxNode;

            newIdx->keyNum = t->keyNum - newKeyNum - 1;
            newIdx->miniKey = t->key[newKeyNum];
            newIdx->isLeaf = 1;

            for (int j = 0; j < newIdx->keyNum; j++) {
                newIdx->key[j] = t->key[j + newKeyNum + 1];
                newIdx->offset[j] = t->offset[j + newKeyNum + 1];
            }

            newIdx->offset[newIdx->keyNum] = t->offset[t->keyNum];
            t->keyNum = newKeyNum;

            delete newNode;
            return newIdx;
        }
    }
    /*erase through idx
     * */
    idxNode* Eraseidx(const Key &k , idxNode *t) {
        void *newNode;

        int i;
        for (i = 0; i < t->keyNum; i++)
            if (k < t->key[i])
                break;

        if (!t->isLeaf) {
            Fileio.seekg(t->offset[i], ios::beg);
            auto *p = new idxNode;
            Fileio.read(reinterpret_cast<char *>(p), idxNodeSize);
            newNode = Eraseidx(k, p);

            if (i == 0) t->miniKey = p->miniKey;
            else t->key[i - 1] = p->miniKey;

            Fileio.seekp(t->offset[i], ios::beg);
            Fileio.write(reinterpret_cast<char *>(p), idxNodeSize);
            //Fileio.flush();

            delete p;

        } else {
            Fileio.seekg(t->offset[i], ios::beg);
            auto *p = new dataNode;
            Fileio.read(reinterpret_cast<char *>(p), dataNodeSize);
            newNode = Erasedata(k, p);

            if (i == 0) t->miniKey = p->key[0];
            else t->key[i - 1] = p->key[0];

            Fileio.seekp(t->offset[i], ios::beg);
            Fileio.write(reinterpret_cast<char *>(p), dataNodeSize);
            //Fileio.flush();

            delete p;

        }
        if (newNode == nullptr)
            return nullptr;
        else {
            if (t->isLeaf == 0)	return eraseBlockidx((idxNode*)newNode, t);
            else return eraseBlockdata((dataNode*)newNode, t);
        }
    }
    /*erase in leaf
     * */
    dataNode* Erasedata(const Key&k, dataNode*t){
        int i;
        bool flag = 0;
        for (i = 0; i < t->keyNum; ++i) {
            if (isequal(t->key[i],k)) {
                flag = 1;
                break;
            }
        }
        if (!flag) return nullptr;

        for (; i < t->keyNum - 1; i++) {
            t->key[i] = t->key[i + 1];
            t->data[i] = t->data[i + 1];
        }
        t->keyNum--;
        curSize--;

        Fileio.seekp(0);
        Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
        Fileio.flush();

        if (t->keyNum >= miniKeyNum)
            return nullptr;
        else {
            auto newnode = new dataNode(*t);
            return newnode;
        }
    }
    /*erase a idx block
     * */
    idxNode* eraseBlockidx(idxNode* n, idxNode* t){
        int min;
        if (t == root) min = 1;
        else min = miniKeyNum;

        int i;
        for (i = t->keyNum; i > 0 && n->key[0] < t->key[i - 1]; --i);

        if (i != t->keyNum) {
            // cout<<"Blockidx case1"<<endl;
            idxNode *next = new idxNode;
            Fileio.seekg(t->offset[i + 1], ios::beg);
            Fileio.read(reinterpret_cast<char *>(next), idxNodeSize);
            if (next->keyNum > miniKeyNum) {
                //borrow an element from next
                n->key[n->keyNum] = next->miniKey;
                next->miniKey = next->key[0];
                n->offset[n->keyNum + 1] = next->offset[0];
                n->keyNum++;
                next->keyNum--;

                int j;
                for (j = 0; j < next->keyNum; j++) {
                    next->key[j] = next->key[j + 1];
                    next->offset[j] = next->offset[j + 1];
                }
                next->offset[j] = next->offset[j + 1];
                t->key[i] = next->miniKey;

                Fileio.seekg(t->offset[i], ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), idxNodeSize);
                Fileio.seekp(t->offset[i + 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(next), idxNodeSize);
                Fileio.flush();
            }else{
                //merge
                n->key[n->keyNum] = next->miniKey;

                int j;
                for (j = 0; j <= next->keyNum; ++j) {
                    if(j!=next->keyNum)
                        n->key[n->keyNum + j + 1] = next->key[j];
                    n->offset[n->keyNum + j + 1] = next->offset[j];
                }
                //n->offset[n->keyNum + j + 1] = next->offset[j];
                n->keyNum += next->keyNum + 1;

                t->keyNum--;
                for (j = i; j < t->keyNum; j++) {
                    t->key[j] = t->key[j + 1];
                    t->offset[j + 1] = t->offset[j + 2];
                }
                if(i == 0) t->miniKey = n->miniKey;
                else t->key[i - 1] = n->miniKey;   //when i==0 it is a quite small problem

                Fileio.seekp(t->offset[i], std::ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), idxNodeSize);
                Fileio.flush();
            }

            delete next;

        }else if (i != 0){
            // cout<<"Blockidx case2"<<endl;
            idxNode *pre = new idxNode;
            Fileio.seekg(t->offset[i - 1], ios::beg);
            Fileio.read(reinterpret_cast<char *>(pre), idxNodeSize);
            if (pre->keyNum > miniKeyNum) {
                //borrow an element from pre
                n->offset[n->keyNum + 1] = n->offset[n->keyNum];

                int j;
                for (j = n->keyNum; j > 0; j--) {
                    n->key[j] = n->key[j - 1];
                    n->offset[j] = n->offset[j - 1];
                }

                n->key[0] = n->miniKey;
                n->offset[0] = pre->offset[pre->keyNum];
                n->keyNum++;
                pre->keyNum--;
                n->miniKey = pre->key[pre->keyNum];
                t->key[i - 1] = n->miniKey;

                Fileio.seekg(t->offset[i - 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(pre), idxNodeSize);
                Fileio.seekp(t->offset[i], ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), idxNodeSize);
                Fileio.flush();
            }else {
                //merge
                pre->key[pre->keyNum] = n->miniKey;

                int j;
                for (j = 0; j <= n->keyNum; j++) {
                    if(j!=n->keyNum)
                        pre->key[pre->keyNum + j + 1] = n->key[j];
                    pre->offset[pre->keyNum + j + 1] = n->offset[j];
                }
                //pre->offset[pre->keyNum + j + 1] = n->offset[j];
                pre->keyNum += n->keyNum + 1;

                t->keyNum--;
                for (j = i - 1; j < t->keyNum; j++) {
                    t->key[j] = t->key[j + 1];
                    t->offset[j + 1] = t->offset[j + 2];
                }
                Fileio.seekp(t->offset[i - 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(pre), idxNodeSize);
                Fileio.flush();
            }

            delete pre;

        }
        delete n;

        if (t->keyNum >= min)
            return nullptr;
        else {
            auto newnode = new idxNode(*t);
            return newnode;
        }
    }
    /*erase a data block
     * */
    idxNode* eraseBlockdata(dataNode* n ,idxNode* t){
        int min;
        if (t == root) min = 1;
        else min = miniKeyNum;

        int i;
        for (i = t->keyNum; i > 0 && n->key[0] < t->key[i - 1]; --i){}

        if (i != t->keyNum) {
            // cout<<"Blockdata case1"<<endl;
            dataNode *next = new dataNode;
            Fileio.seekg(t->offset[i + 1], ios::beg);
            Fileio.read(reinterpret_cast<char *>(next), dataNodeSize);
            if (next->keyNum > miniKeyNum) {
                //borrow an element from next
                n->key[n->keyNum] = next->key[0];
                n->data[n->keyNum] = next->data[0];
                n->keyNum++;
                next->keyNum--;
                for (int j = 0; j < next->keyNum; j++) {
                    next->key[j] = next->key[j + 1];
                    next->data[j] = next->data[j + 1];
                }

                t->key[i] = next->key[0];
                //t->key[i - 1] = n->key[0];
                //cout<<t->isLeaf<<endl;
                Fileio.seekp(t->offset[i], ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), dataNodeSize);
                Fileio.seekp(t->offset[i + 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(next), dataNodeSize);
                Fileio.flush();
            }else{
                //merge
                for (int j = 0; j < next->keyNum; j++) {
                    n->key[n->keyNum + j] = next->key[j];
                    n->data[n->keyNum + j] = next->data[j];
                }
                //cout<<n->keyNum<<endl;
                n->keyNum += next->keyNum;
                //cout<<n->keyNum<<endl;
                n->nextoffset = next->nextoffset;

                t->keyNum--;

                for (int j = i; j < t->keyNum; j++) {
                    t->key[j] = t->key[j + 1];
                    t->offset[j + 1] = t->offset[j + 2];
                }
                //t->key[i - 1] = n->key[0];

                Fileio.seekp(t->offset[i], ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), dataNodeSize);
                Fileio.flush();
            }

            delete next;

        }else if (i != 0) {
            // cout<<"Blockdata case2"<<endl;
            dataNode *pre = new dataNode;
            Fileio.seekg(t->offset[i - 1], ios::beg);
            Fileio.read(reinterpret_cast<char *>(pre), dataNodeSize);
            if (pre->keyNum > miniKeyNum) {
                //borrow an element from pre

                for (int j = n->keyNum; j > 0; --j) {
                    n->key[j] = n->key[j - 1];
                    n->data[j] = n->data[j - 1];
                }

                n->keyNum++;
                pre->keyNum--;

                n->key[0] = pre->key[pre->keyNum];
                n->data[0] = pre->data[pre->keyNum];
                t->key[i - 1] = n->key[0];

                Fileio.seekp(t->offset[i - 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(pre), dataNodeSize);
                Fileio.seekp(t->offset[i], ios::beg);
                Fileio.write(reinterpret_cast<char *>(n), dataNodeSize);
                Fileio.flush();
            }else {
                //merge
                for (int j = 0; j < n->keyNum; j++) {
                    pre->key[pre->keyNum + j] = n->key[j];
                    pre->data[pre->keyNum + j] = n->data[j];
                }

                pre->keyNum += n->keyNum;
                pre->nextoffset = n->nextoffset;

                t->keyNum--;
                for (int j = i - 1; j < t->keyNum; j++) {
                    t->key[j] = t->key[j + 1];
                    t->offset[j + 1] = t->offset[j + 2];
                }

                Fileio.seekp(t->offset[i - 1], ios::beg);
                Fileio.write(reinterpret_cast<char *>(pre), dataNodeSize);
                Fileio.flush();
            }

            delete pre;

        }

        delete n;

        if (t->keyNum >= min)
            return nullptr;
        else {
            auto newnode = new idxNode(*t);
            return newnode;
        }
    }

private:
    /*a series of print function
     * run Print() and it will print the whole tree
     * it is no use for user
     * all for test
     * */
    void Print() {
        puts("---------------------Print Tree----------------------------");
        Printidx(*root);
        puts("----------------------End Tree------------------------------");
    }

    void Printidx(const idxNode &t) {
        putchar('{');
        printidx(t);
        if (!t.isLeaf) {
            for (int i = 0; i <= t.keyNum; i++) {
                idxNode p;
                Fileio.seekg(t.offset[i]);
                Fileio.read(reinterpret_cast<char *>(&p), idxNodeSize);
                Printidx(p);
            }
        }
        else {
            for (int i = 0; i <= t.keyNum; i++) {
                dataNode p;
                Fileio.seekg(t.offset[i]);
                Fileio.read(reinterpret_cast<char *>(&p), dataNodeSize);
                printdata(p);
            }
        }
        puts("}\n");
    }

    void putData(int offset) {
        dataNode t;
        Fileio.seekg(offset);
        Fileio.read(reinterpret_cast<char *>(&t), dataNodeSize);
        puts("[");
        printdata(t);
        puts("]");
    }

    void putidx(int offset) {
        idxNode t;
        Fileio.seekg(offset);
        Fileio.read(reinterpret_cast<char *>(&t), idxNodeSize);
        putchar('{');
        cout << endl;
        printidx(t);
        puts("}");
    }

    void printidx(const idxNode &t) {
        printf("idxNode(%d, %d):", t.keyNum, t.miniKey);
        for (int i = 0; i < t.keyNum; i++)
            cout << t.key[i] << ' ';
        cout << endl;
    }

    void printdata(const dataNode &t) {
        printf("dataNode(%d, %d):", t.keyNum, t.key[0]);
        for (int i = 0; i < t.keyNum; i++)
            printf("(%d, %d) ", t.key[i], t.data[i]);
        cout << endl;
    }

public:
    /*search a key
     *return an Iterator
     * */
    Iterator search(const Key&k){
        if(empty()) return Iterator(this,dataNode(),-1);
        auto t=idxSearch(k,*root);
        auto res=*t;
        delete t;
        return res;
    }

    /*find a key
     * return true/false
     * */
    bool find(const Key &k){
        return search(k).valid();
    }

    /*search the first key >=k
     * fix the problem in Lower_bounddata
     * */
    Iterator Lower_bound(const Key &k){
        auto t=Lower_boundidx(k,*root);
        auto tmp=*t;
        delete t;

        //fix problem
        if(tmp.key()<k)
            tmp++;

        return tmp;
    }

public:
    pair<bool,Value> query(const Key &k){
        if(search(k).valid())
            return make_pair(true,search(k).value());
        else
            return make_pair(false,Value());
    }

    void insert(const Key &k, const Value & data){
        //add root
        if(root->keyNum==-1){
            root->isLeaf = true;
            root->keyNum++;
            root->miniKey = k;

            leftHead->keyNum = 1;
            leftHead->key[0] = k;
            leftHead->data[0] = data;
            leftHead->offset = 2 * sizeof(int);
            root->offset[0] = 2 * sizeof(int);

            Fileio.seekp(2 * sizeof(int), ios::beg);
            Fileio.write(reinterpret_cast<char *>(leftHead), dataNodeSize);
            Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);

            curSize++;

            Fileio.seekp(0, ios::beg);
            Fileio.write(reinterpret_cast<char *>(&curSize), sizeof(int));
            Fileio.flush();
            return;
        }

        idxNode *q=Insertidx(k,data,root);

        //change new root
        if(q!= nullptr) {
            idxNode *t = root;
            root = new idxNode;
            root->keyNum += 2;
            root->miniKey = t->miniKey;
            root->key[0] = q->miniKey;  //fix it
            root->offset[0] = _offset;
            root->offset[1] = _offset + idxNodeSize;
            Fileio.seekp(2 * sizeof(int) + dataNodeSize, ios::beg);
            Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);
            Fileio.seekp(_offset, std::ios::beg);
            Fileio.write(reinterpret_cast<char *>(t), idxNodeSize);
            Fileio.write(reinterpret_cast<char *>(q), idxNodeSize);
            _offset += 2 * idxNodeSize;
            Fileio.seekp(sizeof(int), std::ios::beg);
            Fileio.write(reinterpret_cast<char *>(&_offset), sizeof(int));
            Fileio.flush();
            delete t;
            delete q;
        }
        Fileio.flush();
    }

    bool modify(const Key &k,const Value &data){
        if(!find(k))
            return false;
        else
            insert(k,data);
        return true;
    }

    List<Value> range(const Key &k1, const Key &k2){
        List<Value> newList;

        if(size()==0) //when file is empty,specially judge
            return newList;

        auto now=Lower_bound(k1);
        //加入now.valid的终止条件
        //以防迭代器越界 陷入死循环 草 被坑惨了
        while(!(k2<now.key())&&now.valid()){
            newList.push_back(now.value());
            //cout<<now.value()<<endl;
            now++;
        }
        return newList;
    }

    bool erase(const Key &k){
        //Print();
        if(root->keyNum==-1) return false;   //there is nothing in the file
        if(!find(k)) return false;  //no k in the file

        idxNode* r=Eraseidx(k,root);

        if(r!= nullptr){
            if(r->keyNum!=0){
                //change root
                Fileio.seekg(root->offset[0], ios::beg);
                Fileio.read(reinterpret_cast<char *>(root), idxNodeSize);
            }
            Fileio.seekp(2 * sizeof(int) + dataNodeSize, ios::beg);
            Fileio.write(reinterpret_cast<char *>(root), idxNodeSize);
            Fileio.flush();

            //delete r is necessary
            //because r refers to a real space
            delete r;
        }
        return true;
    }

};



#endif //TICKETSYSTEM_2020_DATABASE_H