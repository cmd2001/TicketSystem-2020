#include <map>
#include <ctime>
#include "../BPlusTree/database.hpp"
#include "../BPlusTree/example.h"
    using namespace std;


    bool test1() { // insert & query
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        // for(int i = 0; i < 1000; ++i) {
        //     int a  = rand(), b = rand();
        //     if(!D0.count(a)) {
        //         D.insert(a, b);
        //         D0.insert(map<int, int>::value_type(a, b));
        //     }
        // }
        cout << D0.size() << " " << D.size() << endl;
        if(D0.size() != D.size()) cout << "size wrong!" << endl;
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) { cout << "No match" << endl; goto WRONG; }
            if(ret.second != it.second) goto WRONG;
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    bool test2() { // modify
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        for(auto &it: D0) {
            int b = rand();
            D0[it.first] = b;
            D.modify(it.first, b);
        }
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) { cout << "No match" << endl; goto WRONG; }
            if(ret.second != it.second) goto WRONG;
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    bool test3() { // erase
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        for(int i = 0; i < 10000; ++i) {
            int a = rand();
            if(D0.count(a)) {
                D0.erase(a);
                D.erase(a);
            }
        }
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) {
                cout<<ret.second<<endl;
                cout << "No match" << endl;goto WRONG; }
            if(ret.second != it.second) {
                cout<<ret.second<<" "<<it.second<<endl;
                cerr<<"w2"<<endl;
                goto WRONG;
            }
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    bool test4() { // range
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        auto list1 = D.range(1, 1000);
        auto list2 = D.range(5000, 50000);
        auto it = list1.begin();
        for(int i = 1; i <= 1000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        it = list2.begin();
        for(int i = 5000; i <= 50000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    bool test5() { // clear
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        D.clear(); D0.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        for(int i = 0; i < 1000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        cout << D0.size() << " " << D.size() << endl;
        if(D0.size() != D.size()) cout << "size wrong!" << endl;
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) { cout << "No match" << endl; goto WRONG; }
            if(ret.second != it.second) goto WRONG;
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    bool test6() { // multiple
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        for(int i = 0; i < 10000; ++i) {
            int a = rand();
            if(D0.count(a)) {
                D0.erase(a);
                D.erase(a);
            }
        }
        for(int i = 0; i < 10000; ++i) {
            int a = rand(), b = rand();
            if(D0.count(a)) {
                D0[a] = b;
                D.modify(a, b);
            }
        }
        auto list1 = D.range(1, 1000);
        auto list2 = D.range(5000, 50000);
        auto it = list1.begin();
        for(int i = 1; i <= 1000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        it = list2.begin();
        for(int i = 5000; i <= 50000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) { cout << "No match" << endl; goto WRONG; }
            if(ret.second != it.second) goto WRONG;
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }
    bool test7(){
        database<int, int> D("file_test");
        map<int, int> D0;
        D.clear();
        for(int i = 0; i < 10000; ++i) {
            int a  = rand(), b = rand();
            if(!D0.count(a)) {
                D.insert(a, b);
                D0.insert(map<int, int>::value_type(a, b));
            }
        }
        /*
        for(int i = 0; i < 10000; ++i) {
            int a = rand();
            if(D0.count(a)) {
                D0.erase(a);
                D.erase(a);
            }
        }
        */
        for(int i = 0; i < 10000; ++i) {
            int a = rand(), b = rand();
            if(D0.count(a)) {
                D0[a] = b;
                D.modify(a, b);
            }
        }
        auto list1 = D.range(1, 1000);
        auto list2 = D.range(5000, 50000);
        auto it = list1.begin();
        for(int i = 1; i <= 1000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        it = list2.begin();
        for(int i = 5000; i <= 50000; ++i) {
            if(D0.count(i)) {
                if(D0[i] != *it) goto WRONG;
                ++it;
            }
        }
        for(auto &it: D0) {
            auto ret = D.query(it.first);
            if(!ret.first) { cout << "No match" << endl; goto WRONG; }
            if(ret.second != it.second) goto WRONG;
        }
        return 1;

        WRONG: cout << "wrong!" << endl;
        return 0;
    }

    int main() {
        srand(time(NULL));
        //cout << (test1()? "test1 passed." : "test1 failed") << endl;
        //cout << (test2()? "test2 passed." : "test2 failed") << endl;
        for(int i=1;i<=100;++i)
        cout << (test3()? "test3 passed." : "test3 failed") << endl;
        //cout << (test4()? "test4 passed." : "test4 failed") << endl;
        //cout << (test5()? "test5 passed." : "test5 failed") << endl;
        //cout << (test6()? "test6 passed." : "test6 failed") << endl;
        //cout << (test7()? "test7 passed." : "test7 failed") << endl;
    }