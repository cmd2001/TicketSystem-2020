#ifndef TICKET_CLASS_HPP
#define TICKET_CLASS_HPP

#include "../BPlusTree/database_test.hpp"
#include "exceptions.hpp"
#include "tools.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::string;

namespace MLJ {

/**各函数返回值
 * 0：失败
 * 1：正常无输出
 * 2：正常有输出
 * 异常抛出与操作失败分离
 */
class Ticket {
    // bool First_User;
    // static inline bool is_letter(const char &ch) { return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'); }

public:
    typedef class wrapped_cstr {
        friend ostream &operator<<(ostream &os, const wrapped_cstr &a) {
            os << a.str;
            return os;
        }
    public:
        char str[22];
        wrapped_cstr() = default;
        wrapped_cstr(const string &s) { strcpy(str, s.c_str()); }
        wrapped_cstr(const char *s) { strcpy(str, s); }
        bool operator<(const wrapped_cstr &o) const { return strcmp(str, o.str) < 0; }
    }type_userName, type_trainID, type_stationName;

    class datentime {
    public:
        int date = 0;
        int minu = 0;

        // 时间 + 日期
        datentime(const string &t = "", const string &d = "") {
            if(d != "") {
                int month = d[1] - '0' - 6 + (d[0] - '0') * 10;
                switch(month) {
                    case 6: date += 30; // Nov.
                    case 5: date += 31; // Oct.
                    case 4: date += 30; // Sept.
                    case 3: date += 31; // Aug.
                    case 2: date += 31; // July
                    case 1: date += 30; // June
                    case 0: date += 0; break;
                    default: date = 0;
                }
                date += (d[3] - '0') * 10 + d[4] - '0';
            }
            if(t != "") {
                minu = 60 * ((t[0] - '0') * 10 + t[1] - '0') + (t[3] - '0') * 10 + t[4] - '0';
            }
        }
        datentime operator+(const int &m) const {
            datentime ans = *this;
            ans.minu += m;
            ans.date += ans.minu / 1440; ans.minu = ans.minu % 1440; // FIXME:
            return ans;
        }
        datentime operator-(const int &m) const {
            datentime ans = *this;
            ans.minu -= m;
            while(ans.minu < 0) --ans.date, ans.minu += 1440;
            return ans;
        }
        datentime operator+=(const int &m) {
            minu += m;
            while(minu >= 1440) ++date, minu -= 1440;
            return *this;
        }
        datentime operator-=(const int &m) {
            minu -= m;
            while(minu < 0) --date, minu += 1440;
            return *this;
        }
        int operator-(const datentime &o) const {
            return (date - o.date) * 1440 + minu - o.minu;
        }
        void add_date(const int &d) { date += d; } // 加日期
        datentime plusdate(const int &d) const { datentime ans = *this; ans.date += d; return ans; } // 返回加日期的datentime
        datentime minusdate(const int &d) const { datentime ans = *this; ans.date -= d; return ans; } // 返回减日期的datentime
        inline bool operator<(const datentime &o) const {
            return (date == o.date)? minu < o.minu : date < o.date;
        }
        inline bool operator>(const datentime &o) const {
            return (date == o.date)? minu > o.minu : date > o.date;
        }
        inline bool operator==(const datentime &o) const {
            return date == o.date && minu == o.minu;
        }
        inline string get_date() const {
            string ret;
            int data = date;
            if(date <= 30) ret =  "06-";
            else if(date <= 61) ret = "07-", data -= 30;
            else if(date <= 92) ret = "08-", data -= 61;
            else if(date <= 122) ret = "09-", data -= 92;
            else if(date <= 153) ret = "10-", data -= 122;
            else if(date <= 183) ret = "11-", data -= 153;
            else if(date <= 214) ret =  "12-", data -= 183;
            else throw illegal_date();
            return ret + (data < 10 ? "0" : "") + std::to_string(data);
        }

        inline string get_time() const {
            int h, m;
            h = minu / 60; m = minu % 60;
            return (h < 10 ? "0" : "") + std::to_string(h) + (m < 10 ? ":0" : ":") + std::to_string(m);
        }
        inline string get() const {
            return get_date() + " " + get_time();
        }
    };
    class type_user {
        friend std::ostream& operator << (std::ostream &os, const type_user &a) {
            os << a.userName << " " << a.name << " " << a.mailAddr << " " << a.privilege;
            return os;
        }
        friend bool operator == (const type_user &a, const type_user &b) {
            return !(strcmp(a.userName, b.userName) || strcmp(a.password, b.password) || strcmp(a.name, b.name) || strcmp(a.mailAddr, b.mailAddr)) && a.privilege == b.privilege;
        }
    public:
        char userName[22];
        char password[32];
        char name[22]; // 汉字
        char mailAddr[32];
        int privilege; // 0-10

        int orderNum; // 储存该用户订单的数量
        type_user() { privilege = 0; orderNum = 0; } // FIXME: memset 0?
    };
    typedef std::pair<bool, type_user> user_return;
    typedef std::pair<bool, int> cur_return;

    class type_train {
    private:
    public:
        char trainID[28];
        int stationNum = 0; // 0-100
        char stations[101][22];
        int pre_prices[100]; //票价前缀和
        int seatNum = 0;    // 0-100000
        datentime startTime;
        datentime arriving[100]; // 前缀和 但不包含startdate
        datentime leaving[100];  // 前缀和 但不包含startdate
        datentime saleDate[2]; // 从2020年6月0日开始算
        char type;

        bool is_released;

        type_train() {
            is_released = 0;
            memset(arriving, 0, sizeof(arriving));
            memset(leaving, 0, sizeof(leaving));
        }
        inline bool operator==(const type_train &o) const {
            return strcmp(trainID, o.trainID) == 0;
        }
        // add_train后售票初始化
        // inline void init() {
        //     pre_prices[0] = prices[0]; seats[0] = seatNum;
        //     for(int i = 1; i < stationNum; ++i) pre_prices[i] = pre_prices[i - 1] + prices[i], seats[i] = seatNum;
        // }
    };
    typedef std::pair<bool, type_train> train_return;

    class type_train_release {
    public:
        char runtimeID[28];
        datentime startdate; //始发日期，仅在列车release之后有
        int seats[100]; // 每站的座位数
        int stationNum;

        type_train_release() = default;
        type_train_release(const int &_stationNum, const int &_seatNum)
        : stationNum(_stationNum) {
            for(int i = 0; i < stationNum; ++i) seats[i] = _seatNum;
        }
        inline bool operator==(const type_train_release &o) const {
            return strcmp(runtimeID, o.runtimeID) == 0;
        }

        // TODO: 性能优化
        inline void buy(const int l, const int r, const int k) {   //左闭右开
            for(int i = l; i < r; ++i) seats[i] -= k;
        }
        inline int queryseats(const int l, const int r) const {         //左闭右开
            int mn = __INT_MAX__;
            for(int i = l; i < r; ++i) mn = std::min(mn, seats[i]);
            return mn;
        }
    };
    typedef std::pair<bool, type_train_release> release_return;

    class type_stationName_startTime {
    public:
        char stationName[22];
        datentime startTime;
        // type_stationName_startTime (const type_stationName_startTime &o) { *this = o; }
        // type_stationName_startTime &operator=(const type_stationName_startTime &o) {
        //     strcpy(stationName, o.stationName);
        //     startTime = o.startTime;
        //     return *this;
        // }
        type_stationName_startTime() = default;
        type_stationName_startTime(const char *_n, const datentime &_t) {
            strcpy(stationName, _n);
            startTime = _t;
        }
        inline bool operator<(const type_stationName_startTime &o) const {
            int _ = strcmp(stationName, o.stationName);
            if(_ == 0) return startTime < o.startTime;
            else return _ < 0;
            // return (startTime == o.startTime)? strcmp(stationName, o.stationName) < 0 : startTime < o.startTime;
        }
    };

    class type_train_tnc { // 用来排序，精简版type_train
    public:
        string out;
        int time;
        int cost;
        type_train_tnc() { out = ""; }
        type_train_tnc(const string &_out, const int &_t, const int &_c): time(_t), cost(_c) {
            out = _out;
        }
    };
    class cmp_time {
    public:
        bool operator()(const type_train_tnc &a, const type_train_tnc &b) { return a.time < b.time; }
    };
    class cmp_cost {
    public:
        bool operator()(const type_train_tnc &a, const type_train_tnc &b) { return a.cost < b.cost; }
    };

    class type_userName_orderID {
    public:
        char userName[22];
        int ID = 0;
        // type_userName_orderID (const type_userName_orderID &o) { *this = o; }
        // type_userName_orderID &operator=(const type_userName_orderID &o) {
        //     strcpy(userName, o.userName);
        //     ID = o.ID;
        //     return *this;
        // }
        type_userName_orderID() = default;
        type_userName_orderID(const char *_n, const int &_id) {
            strcpy(userName, _n);
            ID = _id;
        }
        inline bool operator<(const type_userName_orderID &o) const {
            int ans = strcmp(userName, o.userName);
            return (ans == 0)? ID > o.ID : ans < 0; // 倒序排列，所以ID越大越先
        }
    };
    enum Ordertype { success, pending, refunded };
    class type_order {
        // friend std::ostream& operator << (std::ostream &os, const type_order &a) {
        //     switch(a._type) {
        //     case success: os << "[success] "; break;
        //     case pending: os << "[pending] "; break;
        //     case refunded: os << "[refunded]"; break;
        //     }
        //     os << a.trainID << " " << a.startS << " " << a.leavingTime.get() << " -> " << a.endS << " " << a.arrivingTime.get() << a.price << " " << a.num;
        //     return os;
        // }
    public:
        Ordertype _type;
        char userName[22];
        char trainID[28];
        char startS[22], endS[22];
        datentime leavingTime, arrivingTime;
        int price;
        int num;
        type_order() = default;
        type_order(const Ordertype &_ty, const char *_n, const char *_t, const char *_sS, const char *_eS, const datentime &_lT, const datentime &_aT, const int &_prc, const int &_num)
        : _type(_ty), leavingTime(_lT), arrivingTime(_aT), price(_prc), num(_num) {
            strcpy(userName, _n);
            strcpy(trainID, _t);
            strcpy(startS, _sS); strcpy(endS, _eS);
        }
        // type_order (const type_order &o) { *this = o; }
        // type_order &operator=(const type_order &o) {
        //     _type = o._type;
        //     leavingTime = o.leavingTime; arrivingTime = o.arrivingTime;
        //     price = o.price; num = o.num;
        //     strcpy(userName, o.userName);
        //     strcpy(trainID, o.trainID);
        //     strcpy(startS, o.startS); strcpy(endS, o.endS);
        //     return *this;
        // }
        inline bool operator==(const type_order &o) const {
            return _type == o._type && strcmp(userName, o.userName) == 0 && strcmp(trainID, o.trainID) == 0
            && strcmp(startS, o.startS) == 0 && strcmp(endS, o.endS) == 0 && num == o.num;
        }
        inline string get() const {
            string ans = "";
            switch(_type) {
            case success: ans = "[success] "; break;
            case pending: ans = "[pending] "; break;
            case refunded: ans = "[refunded]"; break;
            }
            ans += (string)trainID + " " + startS + " " + leavingTime.get() + " -> " + endS + " " + arrivingTime.get() + " " + std::to_string(price) + " " + std::to_string(num);
            return ans;
        }
    };
    typedef std::pair<bool, type_order> order_return;

    database_test<type_userName, type_user> Users; // FIXME: 文件读入
    database_test<type_userName, int> Cur_users; // 作为一个索引，仅判断是否在当前列表中，查询详细信息还要在Users中查询
    database_test<type_trainID, type_train> Trains_base;
    database_test<type_trainID, type_train_release> Trains_released;
    database_test<type_stationName_startTime, std::pair<type_trainID, int>> Database_stations; // 按车站和出发时间记录车次和该站编号
    database_test<type_userName_orderID, type_order> Database_orders;

    Ticket():
        Users("file_users"),
        Cur_users("file_cur_users"),
        Trains_base("file_trains_base"), Trains_released("file_trains_released"),
        Database_stations("file_stations"),
        Database_orders("file_orders") {
        // First_User = 1;
    }

private:
    inline type_train_release get_release(const string &str) {
        release_return this_t_release = Trains_released.query(str);
        if(!this_t_release.first) throw unknown_wrong();
        return this_t_release.second;
    }

public:
// *************************************** User Operations **************************************

    string add_user(const string* cmd, const int &siz) {
        int i = 1;
        bool vis[6] = {0};
        type_user newuser;
        cur_return cur_u;
        string username;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-c") {
                if(!vis[0]) {
                    vis[0] = 1;
                    username = cmd[++i];
                    cur_u = Cur_users.query(type_userName(username));
                } else throw illegal_arg();
            } else if(cmd[i] == "-u") {
                if(!vis[1]) {
                    vis[1] = 1;
                    strcpy(newuser.userName, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[2]) {
                    vis[2] = 1;
                    strcpy(newuser.password, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-n") {
                if(!vis[3]) {
                    vis[3] = 1;
                    strcpy(newuser.name, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-m") {
                if(!vis[4]) {
                    vis[4] = 1;
                    strcpy(newuser.mailAddr, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-g") {
                if(!vis[5]) {
                    vis[5] = 1;
                    int tmp = 0;
                    ++i;
                    for(int j = 0; j < cmd[i].size(); ++j) tmp = tmp * 10 + cmd[i][j] - '0';
                    if(tmp >= 0 && tmp <= 10) newuser.privilege = tmp;
                    else throw bad_para();
                }
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 1; i < 5; ++i) if(!vis[i]) throw illegal_arg();
        if(Users.empty()) { // First user
            newuser.privilege = 10;
            // cout << "First user!" << endl;
        } else {
            if(!vis[0] || !vis[5]) throw illegal_arg();
            if(!cur_u.first) return "-1";
            user_return this_u = Users.query(type_userName(username));
            if(!this_u.first) throw unknown_wrong();
            const int &pri = newuser.privilege;
            if(pri < 0 || pri > 10 || pri >= this_u.second.privilege) return "-1";
        }

        Users.insert(newuser.userName, newuser);
        return "0";
    }

    string login(const string* cmd, const int &siz) {
        int i = 1;
        bool vis[2] = {0};
        cur_return cur_u;
        user_return this_u;
        string username;
        string given_pswd;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis[0]) {
                    vis[0] = 1;
                    username = cmd[++i];
                    this_u = Users.query(username);
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[1]) {
                    vis[1] = 1;
                    given_pswd = cmd[++i];
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 2; ++i) if(!vis[i]) throw illegal_arg();
        if(!this_u.first) return "-1";
        if(given_pswd != this_u.second.password) return "-1";
        cur_u = Cur_users.query(username);
        if(cur_u.first) return "-1";

        Cur_users.insert(this_u.second.userName, 0);
        return "0";
    }

    string logout(const string* cmd, const int &siz) {
        int i = 1;
        bool vis = 0;
        cur_return in_cur;
        string username;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis) {
                    vis = 1;
                    username = cmd[++i];
                    in_cur = Cur_users.query(username);
                }
            } else throw illegal_arg();
            ++i;
        }
        if(!vis) throw illegal_arg();
        if(!in_cur.first) return "-1";

        Cur_users.erase(username, 0);
        return "0";
    }

    string query_profile(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1;
        bool vis[2] = {0};
        cur_return cur_u;
        user_return this_u, that_u;
        string username;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-c") {
                if(!vis[0]) {
                    vis[0] = 1;
                    username = cmd[++i];
                    cur_u = Cur_users.query(username);
                } else throw illegal_arg();
            } else if(cmd[i] == "-u") {
                if(!vis[1]) {
                    vis[1] = 1;
                    that_u = Users.query(cmd[++i]);
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 2; ++i) if(!vis[i]) throw illegal_arg();
        if(!cur_u.first || !that_u.first) return "-1";
        this_u = Users.query(username);
        if(strcmp(this_u.second.userName, that_u.second.userName) != 0 && this_u.second.privilege <= that_u.second.privilege) return "-1";
        // 可查询当且仅当this_u权限大于that.u，或this_u和that_u是一个人
        const type_user &u = that_u.second;
        ans = (string)u.userName + " " + u.name + " " + u.mailAddr + " " + std::to_string(u.privilege);
        return ans;
    }

    string modify_profile(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[6] = {0};
        cur_return cur_u;
        string cur_username;
        user_return this_u, that_u;
        type_user newuser;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-c") {
                if(!vis[0]) {
                    vis[0] = 1;
                    cur_username = cmd[++i];
                    cur_u = Cur_users.query(cur_username);
                } else throw illegal_arg();
            } else if(cmd[i] == "-u") {
                if(!vis[1]) {
                    vis[1] = 1;
                    that_u = Users.query(cmd[++i]);
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[2]) {
                    vis[2] = 1;
                    strcpy(newuser.password, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-n") {
                if(!vis[3]) {
                    vis[3] = 1;
                    strcpy(newuser.name, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-m") {
                if(!vis[4]) {
                    vis[4] = 1;
                    strcpy(newuser.mailAddr, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-g") {
                if(!vis[5]) {
                    vis[5] = 1;
                    int tmp = 0;
                    ++i;
                    for(int j = 0; j < cmd[i].size(); ++j) tmp = tmp * 10 + cmd[i][j] - '0';
                    if(tmp >= 0 && tmp <= 10) newuser.privilege = tmp;
                    else throw bad_para();
                }
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 2; ++i) if(!vis[i]) throw illegal_arg();
        if(!cur_u.first || !that_u.first) return "-1";
        this_u = Users.query(cur_username);
        if(!this_u.first) throw unknown_wrong();
        if(strcmp(this_u.second.userName, that_u.second.userName) != 0 && (this_u.second.privilege <= that_u.second.privilege || this_u.second.privilege <= newuser.privilege)) return "-1";
        // 可修改当且仅当this_u权限大于that.u，或this_u和that_u是一个人

        strcpy(newuser.userName, that_u.second.userName);
        if(!vis[2]) strcpy(newuser.password, that_u.second.password);
        if(!vis[3]) strcpy(newuser.name, that_u.second.name);
        if(!vis[4]) strcpy(newuser.mailAddr, that_u.second.mailAddr);
        if(!vis[5]) newuser.privilege = that_u.second.privilege;

        ans = (string)newuser.userName + " " + newuser.name + " " + newuser.mailAddr + " " + std::to_string(newuser.privilege);
        if(!(newuser == that_u.second)) Users.modify(newuser.userName, that_u.second, newuser);
        return ans;
    }


// *************************************** Train Operations **********************************************

    string add_train(const string* cmd, const int &siz) {
        int i = 1;
        bool vis[10] = {0};
        type_train newtrain;
        int travelTimes[100], stopoverTimes[100];
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-i") {
                if(!vis[0]) {
                    vis[0] = 1;
                    strcpy(newtrain.trainID, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-n") {
                if(!vis[1]) {
                    vis[1] = 1;
                    newtrain.stationNum = atoi(cmd[++i].c_str());
                    if(newtrain.stationNum >= 100 || newtrain.stationNum < 0) throw bad_para();
                } else throw illegal_arg();
            } else if(cmd[i] == "-m") {
                if(!vis[2]) {
                    vis[2] = 1;
                    newtrain.seatNum = atoi(cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-s") {
                if(!vis[3]) {
                    vis[3] = 1;
                    int _siz = 0;
                    string tmp[100];
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) {
                        strcpy(newtrain.stations[k], tmp[k].c_str());
                    }
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[4]) {
                    vis[4] = 1;
                    int _siz = 0;
                    string tmp[100];
                    split(cmd[++i], tmp, _siz, '|');
                    newtrain.pre_prices[0] = 0;
                    for(int k = 0; k < _siz; ++k) newtrain.pre_prices[k + 1] = newtrain.pre_prices[k] + atoi(tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-x") {
                if(!vis[5]) {
                    vis[5] = 1;
                    newtrain.startTime = datentime(cmd[++i]);
                } else throw illegal_arg();
            } else if(cmd[i] == "-t") {
                if(!vis[6]) {
                    vis[6] = 1;
                    int _siz = 0;
                    string tmp[100];
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) travelTimes[k] = atoi(tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-o") {
                if(!vis[7]) {
                    vis[7] = 1;
                    int _siz = 0;
                    string tmp[100];
                    stopoverTimes[0] = 0;
                    if(cmd[++i] != "_") { // 特判仅两站
                        split(cmd[i], tmp, _siz, '|');
                        for(int k = 0; k < _siz; ++k) stopoverTimes[k + 1] = atoi(tmp[k].c_str());
                    }
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[8]) {
                    vis[8] = 1;
                    int _siz = 0;
                    string tmp[100];
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) {
                        newtrain.saleDate[k] = datentime("", tmp[k]);
                    }
                } else throw illegal_arg();
            } else if(cmd[i] == "-y") {
                if(!vis[9]) {
                    vis[9] = 1;
                    newtrain.type = cmd[++i][0];
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 10; ++i) if(!vis[i]) throw illegal_arg();
        if(Trains_base.query(newtrain.trainID).first) return "-1"; // 已用的ID，操作失败
        newtrain.arriving[0] = newtrain.leaving[0] = newtrain.startTime;
        for(int i = 1; i < newtrain.stationNum; ++i) {
            newtrain.arriving[i] = newtrain.leaving[i - 1] + travelTimes[i - 1];
            newtrain.leaving[i] = newtrain.arriving[i] + stopoverTimes[i];
        }
        // for(int i = 0; i < newtrain.stationNum; ++i) {
        //     cout << newtrain.arriving[i].get() << " " << newtrain.leaving[i].get() << endl;
        // }
        Trains_base.insert(newtrain.trainID, newtrain);
        return "0";
    }

    // 把Train_unreleased移到Train中，并添加Database_stations
    string release_train(const string* cmd, const int &siz) {
        int i = 1; bool vis = 0;
        string trainID;
        train_return this_t;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-i") {
                if(!vis) {
                    vis = 1;
                    trainID = cmd[++i];
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        this_t = Trains_base.query(trainID);
        if(!this_t.first) return "-1";
        if(this_t.second.is_released) return "-1";
        type_train t = this_t.second;
        t.is_released = 1;
        Trains_base.modify(trainID, this_t.second, t); // modify Train_base, is_released = 1

        string copyID;
        datentime everyday_starttime = t.saleDate[0];
        type_train_release t_release(t.stationNum, t.seatNum);
        for(int i = 0; i <= t.saleDate[1].date - t.saleDate[0].date; ++i) {
            t_release.startdate = everyday_starttime;
            copyID = (string)t.trainID + "#" + t_release.startdate.get_date();
            cout << copyID << endl;
            strcpy(t_release.runtimeID, copyID.c_str());
            Trains_released.insert(copyID, t_release); // 后置数字后列入Train_release(例如：trainA#06-20)

            for(int j = 0; j < t.stationNum; ++j) { // 按车站和出发时间（含天）列入Database_stations
                cout << t.stations[j] << " " << t.leaving[j].plusdate(everyday_starttime.date).get() << " " << copyID << " " << j << endl;
                Database_stations.insert(type_stationName_startTime(t.stations[j], t.leaving[j].plusdate(everyday_starttime.date)), std::make_pair(copyID, j));
            }
            everyday_starttime.add_date(1);
        }
        return "0";
    }

    string query_train(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[2] = {0};
        string trainID;
        datentime query_date;
        train_return this_t;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-i") {
                if(!vis[0]) {
                    vis[0] = 1;
                    trainID = cmd[++i];
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[1]) {
                    vis[1] = 1;
                    query_date = datentime("", cmd[++i]);
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        this_t = Trains_base.query(trainID);
        if(!this_t.first) return "-1";
        const type_train &t = this_t.second;
        if(query_date < t.saleDate[0] || query_date > t.saleDate[1]) return "-1";

        release_return this_t_release;
        if(!t.is_released) this_t_release.second = type_train_release(t.stationNum, t.seatNum);
        else {
            this_t_release = Trains_released.query(trainID + "#" + query_date.get_date());
            if(!this_t_release.first) throw unknown_wrong();
        }
        // cout << this_t.second.saleDate[0].get() << " " << this_t.second.saleDate[1].get() << endl;
        const type_train_release &t_release = this_t_release.second;

        ans = (string)t.trainID + " " + t.type;
        for(int i = 0; i < t.stationNum; ++i) {
            ans += "\n";
            ans += (string)t.stations[i] + " ";
            if(i == 0) ans += "xx-xx xx:xx ";
            else ans += t.arriving[i].plusdate(query_date.date).get() + " ";
            ans += "-> ";
            if(i == t.stationNum - 1) ans += "xx-xx xx:xx " + std::to_string(t.pre_prices[i]) + " x";
            else ans += t.leaving[i].plusdate(query_date.date).get() + " " + std::to_string(t.pre_prices[i]) + " " + std::to_string(t_release.seats[i]);
        }
        return ans;
    }

    string delete_train(const string* cmd, const int &siz) {
        int i = 1; bool vis = 0;
        string trainID;
        train_return this_t;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-i") {
                if(!vis) {
                    vis = 1;
                    trainID = cmd[++i];
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        this_t = Trains_base.query(trainID);
        if(!this_t.first || this_t.second.is_released) return "-1"; // 不存在或已发行
        Trains_base.erase(trainID, this_t.second);
        return "0";
    }

    string query_ticket(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[4] = {0};
        string startS, endS;
        datentime date;
        bool flag = 0;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-s") {
                if(!vis[0]) {
                    vis[0] = 1;
                    startS = cmd[++i];
                } else throw illegal_arg();
            } else if(cmd[i] == "-t") {
                if(!vis[1]) {
                    vis[1] = 1;
                    endS = cmd[++i];
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[2]) {
                    vis[2] = 1;
                    date = datentime("", cmd[++i]);
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[3]) {
                    vis[3] = 1;
                    if(cmd[++i] == "cost") flag = 1;
                    else flag = 0;
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 3; ++i) if(!vis[i]) throw illegal_arg();

        // 思路：查询当天经过起始站的车次ID，在Trains中查出车次信息，找到之后经过到达站的车次，存入list中排序
        auto st = Database_stations.range(type_stationName_startTime(startS.c_str(), date), type_stationName_startTime(startS.c_str(), date + 1439));
        if(st.empty()) return "0";
        train_return this_t;
        type_train_tnc *list = new type_train_tnc[st.size()]; // 此处有new
        int cnt = 0;
        // for(it = st.begin(); it != st.end(); ++it) {
        //     cout << (*it).first << " " << (*it).second << endl;
        // }
        for(auto it = st.begin(); it != st.end(); ++it) {
            string trainID = (*it).first.str;
            this_t = Trains_base.query(trainID.substr(0, trainID.size() - 6)); // 取出车次名（不带日期）
            if(!this_t.first) throw unknown_wrong();
            type_train &t = this_t.second;
            for(int i = (*it).second + 1; i < t.stationNum; ++i) if(t.stations[i] == endS) {
                const type_train_release t_release = get_release(trainID);
                int price = t.pre_prices[i] - t.pre_prices[(*it).second], seats = t_release.queryseats((*it).second, i);
                // 先把输出信息写好存进string
                string out = (string)t.trainID + " " + startS + " " + t.leaving[(*it).second].plusdate(t_release.startdate.date).get() + " -> " + endS + " " + t.arriving[i].plusdate(t_release.startdate.date).get() + " " + std::to_string(price) + " " + std::to_string(seats);
                list[cnt++] = type_train_tnc(out, t.arriving[i] - t.leaving[(*it).second], price);
                break;
            }
        }
        if(flag) MLJ::sort(list, list + cnt, cmp_cost(), quicksort);
        else MLJ::sort(list, list + cnt, cmp_time(), quicksort);
        ans = std::to_string(cnt);
        for(int i = 0; i < cnt; ++i) ans += "\n" + list[i].out;
        delete [] list;
        return ans;
    }

    string query_transfer(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[4] = {0};
        string startS, endS;
        datentime date;
        bool flag = 0;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-s") {
                if(!vis[0]) {
                    vis[0] = 1;
                    startS = cmd[++i];
                } else throw illegal_arg();
            } else if(cmd[i] == "-t") {
                if(!vis[1]) {
                    vis[1] = 1;
                    endS = cmd[++i];
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[2]) {
                    vis[2] = 1;
                    date = datentime("", cmd[++i]);
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[3]) {
                    vis[3] = 1;
                    if(cmd[++i] == "cost") flag = 1;
                    else flag = 0;
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 3; ++i) if(!vis[i]) throw illegal_arg();
        // TODO:
        // st查询起始站对应有哪些车次，et查询从当天至世界末日终点站对应有哪些车次
        // 思路：枚举st车次经过的站点，枚举et逆向经过的站点，两者对应且符合时间先后则可行
        auto st = Database_stations.range(type_stationName_startTime(startS.c_str(), date), type_stationName_startTime(startS.c_str(), date + 1439));
        auto et = Database_stations.range(type_stationName_startTime(endS.c_str(), date), type_stationName_startTime(endS.c_str(), datentime("23:59", "12-31")));
        if(st.empty() || et.empty()) return "0";
        train_return first_t, second_t;
        type_train_tnc choosed_transfer;
        for(auto it = st.begin(); it != st.end(); ++it) {
            first_t = Trains_base.query(string((*it).first.str).substr(0, strlen((*it).first.str) - 6)); // 取出车次名（不带日期）
            if(!first_t.first) throw unknown_wrong();
            type_train &t_1 = first_t.second;
            for(int i = (*it).second + 1; i < t_1.stationNum; ++i) {
                for(auto it_2 = et.begin(); it_2 != et.end(); ++it_2) {
                    second_t = Trains_base.query(string((*it_2).first.str).substr(0, strlen((*it_2).first.str) - 6));
                    if(!second_t.first) throw unknown_wrong();
                    type_train &t_2 = second_t.second;
                    for(int j = (*it_2).second - 1; j >= 0 && t_2.leaving[j] > t_1.arriving[i]; --j)
                    if(strcmp(t_2.stations[j], t_1.stations[i]) == 0) {

                        const type_train_release t_release_1 = get_release((*it).first.str);
                        const type_train_release t_release_2 = get_release((*it_2).first.str);
                        int price_1 = t_1.pre_prices[i] - t_1.pre_prices[(*it).second], price_2 =  t_2.pre_prices[(*it_2).second] - t_2.pre_prices[j];
                        int seats_1 = t_release_1.queryseats((*it).second, i), seats_2 = t_release_2.queryseats(j, (*it_2).second);
                        // 先把输出信息写好存进string
                        string out = (string)t_1.trainID + " " + startS + " " + t_1.leaving[(*it).second].plusdate(t_release_1.startdate.date).get() + " -> " + t_1.stations[i] + " " + t_1.arriving[i].plusdate(t_release_1.startdate.date).get() + " " + std::to_string(price_1) + " " + std::to_string(seats_1);
                        out += (string)t_2.trainID + " " + t_2.stations[j] + " " + t_2.leaving[j].plusdate(t_release_2.startdate.date).get() + " -> " + endS + " " + t_2.arriving[(*it_2).second].plusdate(t_release_2.startdate.date).get() + " " + std::to_string(price_2) + " " + std::to_string(seats_2);
                        type_train_tnc newtnc(out, price_1 + price_2, seats_1 + seats_2);
                        if(choosed_transfer.out == "") choosed_transfer = newtnc;
                        else {
                            if(flag) {
                                if(cmp_cost()(newtnc, choosed_transfer)) choosed_transfer = newtnc;
                            } else {
                                if(cmp_time()(newtnc, choosed_transfer)) choosed_transfer = newtnc;
                            }
                        }
                        break;
                    }
                }
            }
        }
        if(choosed_transfer.out == "") return "0";
        else return choosed_transfer.out;
    }

    string buy_ticket(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[7] = {0}, flag = 0;
        type_order o;
        datentime query_date;
        cur_return cur_u;
        user_return this_u;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis[0]) {
                    vis[0] = 1;
                    strcpy(o.userName, cmd[++i].c_str());
                    cur_u = Cur_users.query(o.userName);
                } else throw illegal_arg();
            } else if(cmd[i] == "-i") {
                if(!vis[1]) {
                    vis[1] = 1;
                    strcpy(o.trainID, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[2]) {
                    vis[2] = 1;
                    query_date = datentime("", cmd[++i]);
                } else throw illegal_arg();
            } else if(cmd[i] == "-n") {
                if(!vis[3]) {
                    vis[3] = 1;
                    o.num = atoi(cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-f") {
                if(!vis[4]) {
                    vis[4] = 1;
                    strcpy(o.startS, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-t") {
                if(!vis[5]) {
                    vis[5] = 1;
                    strcpy(o.endS, cmd[++i].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-q") {
                if(!vis[6]) {
                    vis[6] = 1;
                    if(cmd[++i] == "true") flag = 1;
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        for(int i = 0; i < 6; ++i) if(!vis[i]) throw illegal_arg();
        if(!cur_u.first) return "-1";
        // cout << o.userName << endl;
        this_u = Users.query(o.userName); // 查一下该用户的信息
        if(!this_u.first) throw unknown_wrong();
        train_return this_t = Trains_base.query(o.trainID);
        if(!this_t.first) return "-1";
        const type_train &t = this_t.second;
        if(!t.is_released) return "-1"; // 未发行

        // cout << o.startS << " " << o.endS << endl;

        int p = -1, q = -1; // 找到这两站的编号
        datentime leavingtime_from_startS; // 记录起始站的时间
        for(int i = 0; i < t.stationNum; ++i) {
            if(strcmp(o.startS, t.stations[i]) == 0) {
                if(q != -1) return "-1";
                else {
                    p = i;
                    leavingtime_from_startS = t.leaving[i];
                }
            } else if(strcmp(o.endS, t.stations[i]) == 0) {
                if(p == -1) return "-1";
                q = i;
            }
        }
        // cout << p << " " << q << endl;
        if(p == -1 || q == -1) return "-1";
        if(leavingtime_from_startS.date + t.saleDate[0].date > query_date.date
        || leavingtime_from_startS.date + t.saleDate[1].date < query_date.date) return "-1"; // 保证在售票天数内
        datentime startdate = datentime("", query_date.minusdate(leavingtime_from_startS.date).get_date()); // 发车日期
        // cout << startdate.get() << endl;

        string ID_with_date = (string)t.trainID + "#" + startdate.get_date();
        release_return this_t_release = Trains_released.query(ID_with_date);
        if(!this_t_release.first) throw unknown_wrong();
        type_train_release t_release = this_t_release.second;
        int seats_available = t_release.queryseats(p, q);
        if(seats_available < o.num) {
            if(flag) { // 加入候补队列
                ans = "queue";
                // TODO:
            } else return "-1";
        } else {
            o.price = t.pre_prices[q] - t.pre_prices[p];
            o.leavingTime = t.leaving[p].plusdate(startdate.date);
            o.arrivingTime = t.arriving[q].plusdate(startdate.date);
            long long total_cost = (long long)o.num * o.price;
            type_user u = this_u.second;
            Database_orders.insert(type_userName_orderID(o.userName, ++u.orderNum), o); // ++该用户的orderNum并添加Database_orders
            Users.modify(this_u.second.userName, this_u.second, u); // 更新该用户的orderNum
            t_release.buy(p, q, o.num); // 改变座位数
            Trains_released.modify(ID_with_date, this_t_release.second, t_release); // 更新该车次的信息

            ans = std::to_string(total_cost);
        }
        return ans;
    }

    string query_order(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis = 0;
        cur_return cur_u;
        user_return this_u;
        string username;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis) {
                    vis = 1;
                    username = cmd[++i];
                    cur_u = Cur_users.query(username);
                }
            } else throw illegal_arg();
            ++i;
        }
        if(!vis) throw illegal_arg();
        if(!cur_u.first) return "-1";
        this_u = Users.query(username);
        if(!this_u.first) throw unknown_wrong();
        List<type_order> orders = Database_orders.range(type_userName_orderID(this_u.second.userName, this_u.second.orderNum), type_userName_orderID(this_u.second.userName, 1));
        ans = std::to_string(this_u.second.orderNum);
        for(List<type_order>::iterator it = orders.begin(); it != orders.end(); ++it) {
            ans += (string)"\n" + (*it).get();
        }
        return ans;
    }

    string refund_ticket(const string* cmd, const int &siz) {
        int i = 1; bool vis[2] = {0};
        string username;
        int which_order = 1;
        cur_return cur_u; user_return this_u;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis[0]) {
                    vis[0] = 1;
                    username = cmd[++i];
                    cur_u = Cur_users.query(username);
                } else throw illegal_arg();
            } else if(cmd[i] == "-n") {
                if(!vis[1]) {
                    vis[1] = 1;
                    which_order = atoi(cmd[++i].c_str());
                } else throw illegal_arg();
            } else throw illegal_arg();
            ++i;
        }
        if(!vis[0]) throw illegal_arg();
        if(!cur_u.first) return "-1";
        this_u = Users.query(username);
        if(!this_u.first) throw unknown_wrong();
        which_order = this_u.second.orderNum - which_order + 1;
        order_return this_o = Database_orders.query(type_userName_orderID(this_u.second.userName, which_order));
        if(!this_o.first) return "-1";
        if(this_o.second._type == refunded) return "-1"; // 已经退票了

        type_order o = this_o.second; o._type = refunded;
        Database_orders.modify(type_userName_orderID(this_u.second.userName, which_order), this_o.second, o);
        // TODO: queue中的操作
        return "0";
    }

    string clean(const string* cmd, const int &siz) {
        // First_User = 1;
        Users.clear();
        Cur_users.clear();
        Trains_base.clear();
        Trains_released.clear();
        Database_stations.clear();
        Database_orders.clear();
        return "0";
    }

    string exit(const string* cmd, const int &siz) {
        Cur_users.clear(); // 清空在线用户列表
        return "bye";
    }
};

}

#endif
