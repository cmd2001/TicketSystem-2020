#ifndef TICKETSYSTEM_2020_DATABASE_TEST_H
#define TICKETSYSTEM_2020_DATABASE_TEST_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include "../include/List.h"
#include <fstream>

template <typename Key,typename Value>
class database_test{
private:
    string Filename;
public:
    typedef pair<Key,Value> Pair;

    void insert(Key K,Value V){
        ifstream in(Filename,ios::binary|ios::in);
        if(!in){cerr<<"insert F open error"<<endl;}

        ofstream out("tmp", ios::trunc | ios::binary);
        if(!out){cerr<<"insert tmp open error"<<endl;}

        Pair P;
        bool flag=false;
        in.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!in.eof()){
            if(!flag&&K<P.first){
                Pair news(K,V);
                out.write(reinterpret_cast<char*>(&news),sizeof(news));
                flag=true;
            }
            out.write(reinterpret_cast<char*>(&P),sizeof(P));
            in.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        if(!flag){
            Pair news(K,V);
            out.write(reinterpret_cast<char*>(&news),sizeof(news));
        }
        in.close(),out.close();

        in.open("tmp",ios::binary|ios::in);
        out.open(Filename, ios::trunc | ios::binary);

        in.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!in.eof()){
            out.write(reinterpret_cast<char*>(&P),sizeof(P));
            in.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        in.close(),out.close();
    }

    bool erase(Key K,Value V){
        ifstream in(Filename,ios::binary|ios::in);
        if(!in){cerr<<"erase F open error"<<endl;}

        ofstream out("tmp", ios::trunc | ios::binary);
        if(!out){cerr<<"erase tmp open error"<<endl;}

        Pair P;
        bool flag=false;
        in.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!in.eof()){
            if(K==P.first&&V==P.second){
                flag=true;
                in.read(reinterpret_cast<char*>(&P),sizeof(P));
                continue;
            }
            out.write(reinterpret_cast<char*>(&P),sizeof(P));
            in.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        in.close(),out.close();

        if(!flag)
            return false;


        in.open("tmp",ios::binary|ios::in);
        out.open(Filename, ios::trunc | ios::binary);

        in.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!in.eof()){
            out.write(reinterpret_cast<char*>(&P),sizeof(P));
            in.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        in.close(),out.close();

        return true;
    };

    bool modify(Key K,Value OldV,Value NewV){
        fstream io(Filename,ios::binary|ios::in|ios::out);
        if(!io){cerr<<"modify F open error"<<endl;}

        Pair P;
        bool flag=false;
        io.seekg(0);
        size_t pos=0;
        io.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!io.eof()){
            if(K==P.first&&OldV==P.second){
                P.second=NewV;
                io.seekp(pos*sizeof(P));
                io.write(reinterpret_cast<char*>(&P),sizeof(P));
                flag=true;
                continue;
            }
            io.read(reinterpret_cast<char*>(&P),sizeof(P));
            pos++;
        }
        io.close();
        return flag;
    };

    /*
     * function query
     * return Value type and a bool
     * bool true refers to query ok
     * if fail, return a default Value and false
     * */
    pair<bool,Value> query(Key K){
        fstream io(Filename,ios::binary|ios::in|ios::out);
        if(!io){cerr<<"query F open error"<<endl;}

        Pair P;
        io.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!io.eof()){
            if(K==P.first){
                return make_pair(true,P.second);
            }
            io.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        io.close();
        return make_pair(false,Value());
    };

    List<Value> range(Key K1,Key K2){
        fstream io(Filename,ios::binary|ios::in|ios::out);
        if(!io){cerr<<"query F open error"<<endl;}

        List<Value> newList;

        Pair P;
        bool flag=false;
        io.read(reinterpret_cast<char*>(&P),sizeof(P));
        while(!io.eof()){
            if(K1<=P.first&&K2>=P.first){
                newList.push_back(P.second);
            }
            io.read(reinterpret_cast<char*>(&P),sizeof(P));
        }
        io.close();

        return newList;
    };

    void clear(){
        ofstream out;
        out.open("tmp", ios::trunc | ios::binary);
        out.close();
        out.open(Filename, ios::trunc | ios::binary);
        out.close();
    }
public:
    database_test(const string &F): Filename(F) {
        ofstream out1;
        out1.open("tmp", ios::trunc | ios::binary); //create tmp and clear
        if (!out1) { cerr << "tmp create error" << endl; }
        out1.close();

        ofstream out2;
        out2.open(F, ios::trunc | ios::binary); //create Filename and clear
        if (!out2) { cerr << "F create error" << endl; }
        out2.close();
    }
};

#endif //TICKETSYSTEM_2020_DATABASE_TEST_H