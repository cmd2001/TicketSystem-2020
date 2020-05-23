#ifndef TICKET_CLASS_HPP
#define TICKET_CLASS_HPP

#include "database.hpp"
#include "exceptions.hpp"
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

// 按照ch分隔字符串，参考python中的 a = s.split(' ');
static void split(const string &src, string *arg, int &cnt, const char &ch) {
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

// TODO: 性能优化

class datentime {
public:
    int date = 0;
    int minu = 0;

    // 时间 + 日期
    datentime(const string &t = "", const string &d = "") {
        if(d != "") {
            int month = (d[1] - '0' - 6);
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
    datentime operator+(const int &m) {
        datentime ans;
        ans.minu = minu + m;
        ans.date += ans.minu / 1440; ans.minu = ans.minu % 1440; // FIXME:
        return ans;
    }
    datentime operator-(const int &m) {
        datentime ans;
        ans.minu = minu - m;
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
    inline int operator-(const datentime &o) {
        return (date - o.date) * 1440 + minu - o.minu;
    }
    inline void add_date(const int &d) { date += d; } // 加日期
    datentime plusdate(const int &d) { datentime ans = *this; ans.date += d; return ans; } // 返回加日期的datentime
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
        if(date <= 30) return "06-" + std::to_string(date);
        else if(date <= 61) return "07-" + std::to_string(date - 30);
        else if(date <= 92) return "08-" + std::to_string(date - 61);
        else if(date <= 122) return "09-" + std::to_string(date - 92);
        else if(date <= 153) return "10-" + std::to_string(date - 122);
        else if(date <= 183) return "11-" + std::to_string(date - 153);
        else if(date <= 214) return "12-" + std::to_string(date - 183);
        else throw illegal_date();
    }

    inline string get_time() const {
        int h, m;
        h = minu / 60; m = minu % 60;
        if(h < 10) return "0" + std::to_string(h) + ":" + std::to_string(m);
        else return std::to_string(h) + ":" + std::to_string(m);
    }
    inline string get() const {
        return get_date() + " " + get_time();
    }
};



/**各函数返回值
 * 0：失败
 * 1：正常无输出
 * 2：正常有输出
 * 异常抛出与操作失败分离
 */
class Ticket {
    bool First_User;
    static inline bool is_letter(const char &ch) { return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'); }

public:
    typedef std::string type_userName;
    typedef std::string type_trainID;
    typedef std::string type_stationName;
    class type_user {
        friend std::ostream &operator<<(std::ostream &os, const type_user &a) {
            os << a.userName << " " << a.name << " " << a.mailAddr << " " << a.privilege;
            return os;
        }
        friend bool operator==(const type_user &a, const type_user &b) {
            return !(strcmp(a.userName, b.userName) || strcmp(a.password, b.password) || strcmp(a.name, b.name) || strcmp(a.mailAddr, b.mailAddr)) && a.privilege == b.privilege;
        }
    public:
        char userName[22];
        char password[32];
        char name[12]; // 汉字
        char mailAddr[25];
        int privilege; // 0-10

        int orderNum; // 储存该用户订单的数量
        // type_user(const char *&_usrnm, const char *&_pswd, const char *&_nm, const char *&_mAddr, const int &_prvl)
        type_user() { privilege = 0; orderNum = 0; }
        type_user(const type_user &o) {
            strcpy(userName, o.userName);
            strcpy(password, o.password);
            strcpy(name, o.name);
            strcpy(mailAddr, o.mailAddr);
            privilege = o.privilege;
            orderNum = o.orderNum;
        }
    };
    typedef std::pair<bool, type_user> user_return;
    typedef std::pair<bool, int> cur_return;

    class type_train {
    private:
    public:
        char trainID[22];
        int stationNum = 0; // 0-100
        char stations[101][22];
        int seatNum = 0;    // 0-100000
        int prices[100];// 0-100000
        datentime startTime;
        datentime arriving[100]; // 前缀和 但不包含startdate
        datentime leaving[100];  // 前缀和 但不包含startdate
        datentime saleDate[2]; // 从2020年6月0日开始算
        char type;

        datentime startdate; //始发日期，仅在列车release之后有
        int pre_prices[100]; //票价前缀和
        int seats[100]; // 每站的座位数

        type_train() {
            memset(arriving, 0, sizeof(arriving));
            memset(leaving, 0, sizeof(leaving));
        }
        type_train(const type_train &o) {
            strcpy(trainID, o.trainID);
            stationNum = o.stationNum;
            for(int i = 0; i < stationNum; ++i) {
                strcpy(stations[i], o.stations[i]);
                prices[i] = o.prices[i];
                pre_prices[i] = o.pre_prices[i];
                seats[i] = o.seats[i];
                arriving[i] = o.arriving[i]; leaving[i] = o.leaving[i];
            }
            seatNum = o.seatNum;
            startTime = o.startTime;
            saleDate[0] = o.saleDate[0]; saleDate[1] = o.saleDate[1];
            type = o.type;
            startdate = o.startdate;
        }
        inline bool operator==(const type_train &o) const {
            return strcmp(trainID, o.trainID) == 0 && startdate == o.startdate;
        }
        // TODO: 性能优化
        inline void buy(const int l, const int r, const int k) {   //左闭右开
            for(int i = l; i < r; ++i) seats[i] -= k;
        }
        inline int query(const int l, const int r) const {         //左闭右开
            int mn = __INT_MAX__;
            for(int i = l; i < r; ++i) mn = std::min(mn, seats[i]);
            return mn;
        }
        // add_train后售票初始化
        inline void init() {
            pre_prices[0] = prices[0]; seats[0] = seatNum;
            for(int i = 1; i < stationNum - 1; ++i) pre_prices[i] = pre_prices[i - 1] + prices[i], seats[i] = seatNum;
        }
    };
    typedef std::pair<bool, type_train> train_return;

    class type_stationName_startTime {
    public:
        char stationName[22];
        datentime startTime;
        type_stationName_startTime(const type_stationName_startTime &o) {
            strcpy(stationName, o.stationName);
            startTime = o.startTime;
        }
        type_stationName_startTime(const char *_n, const datentime &_t) {
            strcpy(stationName, _n);
            startTime = _t;
        }
        inline bool operator<(const type_stationName_startTime &o) const {
            return (startTime == o.startTime)? strcmp(stationName, o.stationName) < 0 : startTime < o.startTime;
        }
    };

    class type_train_tnc { // 用来排序，精简版type_train
    public:
        string out;
        int time;
        int cost;
        type_train_tnc() {}
        type_train_tnc(const string &_out, const int &_t, const int &_c): time(_t), cost(_c) {
            out = _out;
        }
    };
    inline bool cmp_time(const type_train_tnc &a, const type_train_tnc &b) { return a.time < b.time; }
    inline bool cmp_cost(const type_train_tnc &a, const type_train_tnc &b) { return a.cost < b.cost; }

    class type_userName_orderID {
    public:
        char userName[22];
        int ID = 0;
        type_userName_orderID(const type_userName_orderID &o) {
            strcpy(userName, o.userName);
            ID = o.ID;
        }
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
        friend std::ostream &operator<<(std::ostream &os, const type_order &a) {
            switch(a._type) {
            case success: os << "[success] "; break;
            case pending: os << "[pending] "; break;
            case refunded: os << "[refunded]"; break;
            }
            os << a.trainID << " " << a.startS << " " << a.leavingTime.get() << " -> " << a.endS << " " << a.arrivingTime.get() << a.price << " " << a.num;
            return os;
        }
    public:
        Ordertype _type;
        char userName[22];
        char trainID[22];
        char startS[22], endS[22];
        datentime leavingTime, arrivingTime;
        int price;
        int num;
        type_order() {}
        type_order(const Ordertype &_ty, const char *_n, const char *_t, const char *_sS, const char *_eS, const datentime &_lT, const datentime &_aT, const int &_prc, const int &_num)
        : _type(_ty), leavingTime(_lT), arrivingTime(_aT), price(_prc), num(_num) {
            strcpy(userName, _n);
            strcpy(trainID, _t);
            strcpy(startS, _sS); strcpy(endS, _eS);
        }
        type_order(const type_order &o) {
            _type = o._type;
            leavingTime = o.leavingTime; arrivingTime = o.arrivingTime;
            price = o.price; num = o.num;
            strcpy(userName, o.userName);
            strcpy(trainID, o.trainID);
            strcpy(startS, o.startS); strcpy(endS, o.endS);
        }
        inline bool operator==(const type_order &o) const {
            return _type == o._type && strcmp(userName, o.userName) == 0 && strcmp(trainID, o.trainID) == 0
            && strcmp(startS, o.startS) == 0 && strcmp(endS, o.endS) == 0 && num == o.num;
        }
    };
    typedef std::pair<bool, type_order> order_return;

    Database<type_userName, type_user> Users; // FIXME: 文件读入
    Database<type_userName, int> Cur_users; // 作为一个索引，仅判断是否在当前列表中，查询详细信息还要在Users中查询
    Database<type_trainID, type_train> Trains_unreleased, Trains;
    Database<type_stationName_startTime, std::pair<type_trainID, int>> Database_stations; // 按车站和出发时间记录车次和该站编号
    Database<type_userName_orderID, type_order> Database_orders;

    Ticket() {
        First_User = 1;
    }

// *************************************** User Operations **************************************

    int add_user(const string* cmd, const int siz) {
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
                    cur_u = Cur_users.query(username);
                } else throw illegal_arg();
            } else if(cmd[i] == "-u") {
                if(!vis[1]) {
                    vis[1] = 1;
                    if(cmd[++i].size() <= 20 && is_letter(cmd[i][0])) strcpy(newuser.userName, cmd[i].c_str());
                    else throw bad_para();
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[2]) {
                    vis[2] = 1;
                    if(cmd[++i].size() >= 6 && cmd[i].size() <= 30) strcpy(newuser.password, cmd[i].c_str());
                    else throw bad_para();
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
        if(First_User) First_User = 0, newuser.privilege = 10;
        else {
            if(!vis[0] || !vis[5]) throw illegal_arg();
            if(!cur_u.first) return 0;
            user_return this_u = Users.query(username);
            if(!this_u.first) throw unknown_wrong();
            const int &pri = newuser.privilege;
            if(pri < 0 || pri > 10 || pri >= this_u.second.privilege) return 0;
        }

        Users.insert(newuser.userName, newuser);
        return 1;
    }

    int login(const string* cmd, const int siz) {
        int i = 1;
        bool vis[2] = {0};
        user_return this_u;
        string given_pswd;
        while(i < siz) {
            if(i == siz - 1) throw illegal_arg();
            if(cmd[i] == "-u") {
                if(!vis[0]) {
                    vis[0] = 1;
                    this_u = Users.query(cmd[++i]);
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
        if(!this_u.first) return 0;
        if(given_pswd != this_u.second.password) return 0;

        Cur_users.insert(this_u.second.userName, 0);
        return 1;
    }

    int logout(const string* cmd, const int siz) {
        int i = 1; bool vis = 0;
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
        if(!in_cur.first) return 0;

        Cur_users.erase(username, 0);
        return 1;
    }

    int query_profile(const string* cmd, const int siz) {
        int i = 1; bool vis[2] = {0};
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
        if(!cur_u.first || !that_u.first) return 0;
        this_u = Users.query(username);
        if(strcmp(this_u.second.userName, that_u.second.userName) != 0 && this_u.second.privilege <= that_u.second.privilege) return 0;

        cout << that_u.second << endl;
        return 2;
    }

    int modify_profile(const string* cmd, const int siz) {
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
                    if(cmd[++i].size() >= 6 && cmd[i].size() <= 30) strcpy(newuser.password, cmd[i].c_str());
                    else throw bad_para();
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
        if(!cur_u.first || !that_u.first) return 0;
        this_u = Users.query(cur_username);
        if(!this_u.first) throw unknown_wrong();
        if(strcmp(this_u.second.userName, that_u.second.userName) != 0 && (this_u.second.privilege <= that_u.second.privilege || this_u.second.privilege <= newuser.privilege)) return 0;

        strcpy(newuser.userName, that_u.second.userName);
        if(!vis[2]) strcpy(newuser.password, that_u.second.password);
        if(!vis[3]) strcpy(newuser.name, that_u.second.name);
        if(!vis[4]) strcpy(newuser.mailAddr, that_u.second.mailAddr);
        if(!vis[5]) newuser.privilege = that_u.second.privilege;
        cout << newuser << endl;
        Users.modify(newuser.userName, that_u.second, newuser);
        return 2;
    }


// *************************************** Train Operations **********************************************

    int add_train(const string* cmd, const int siz) {
        int i = 1; bool vis[10] = {0};
        type_train newtrain; string tmp[100];
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
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) strcpy(newtrain.stations[k], tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[4]) {
                    vis[4] = 1;
                    int _siz = 0;
                    split(cmd[++i], tmp, _siz, '|');
                    newtrain.prices[0] = 0;
                    for(int k = 0; k < _siz; ++k) newtrain.prices[k + 1] = atoi(tmp[k].c_str());
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
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) travelTimes[k] = atoi(tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-o") {
                if(!vis[7]) {
                    vis[7] = 1;
                    int _siz = 0;
                    split(cmd[++i], tmp, _siz, '|');
                    stopoverTimes[0] = 0;
                    for(int k = 0; k < _siz; ++k) stopoverTimes[k + 1] = atoi(tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-d") {
                if(!vis[8]) {
                    vis[8] = 1;
                    int _siz = 0;
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) newtrain.saleDate[k] = datentime("", tmp[k]);
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
        if(Trains_unreleased.query(newtrain.trainID).first) return 0; // 已用的ID，操作失败
        newtrain.arriving[0] = newtrain.leaving[0] = newtrain.startTime;
        for(int i = 1; i < newtrain.stationNum; ++i) {
            newtrain.arriving[i] = newtrain.leaving[i - 1] + travelTimes[i - 1];
            newtrain.leaving[i] = newtrain.arriving[i] + stopoverTimes[i];
        }
        newtrain.init();
        Trains_unreleased.insert(newtrain.trainID, newtrain);
        return 1;
    }

    // 把Train_unreleased移到Train中，并添加Database_stations
    int release_train(const string* cmd, const int siz) {
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
        this_t = Trains_unreleased.query(trainID);
        if(!this_t.first) return 0;

        type_train &t = this_t.second;
        // t.init(); // 售票初始化
        string copyID;
        char suf[5]; suf[0] = '#';
        datentime everyday_starttime = t.saleDate[0];
        for(int i = 0; i <= t.saleDate[1].date - t.saleDate[0].date; ++i) {
            t.startdate = t.saleDate[0] + i;
            strcpy(suf + 1, t.startdate.get_date().c_str());
            copyID = t.trainID + (string)suf;
            Trains.insert(copyID, t); // 后置数字后列入Train(例如：trainA#06-20)

            for(int j = 0; j < t.stationNum; ++j) { // 按车站和出发时间（含天）列入Database_stations
                Database_stations.insert(type_stationName_startTime(t.stations[j], t.leaving[j] + everyday_starttime.date * 1440), std::make_pair(copyID, j));
            }
            everyday_starttime.add_date(1);
        }
        Trains_unreleased.erase(t.trainID, t); // 从unreleased列表删除
        return 1;
    }

    int query_train(const string* cmd, const int siz) {
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
        this_t = Trains.query(trainID + '#' + query_date.get_date());
        if(!this_t.first) {
            this_t = Trains_unreleased.query(trainID);
            if(!this_t.first || this_t.second.saleDate[0] > query_date || this_t.second.saleDate[1] < query_date) return 0;
        }
        type_train &t = this_t.second;
        cout << t.trainID << " " << t.type << endl;
        for(int i = 0; i < t.stationNum; ++i) {
            cout << t.stations[i] << " ";
            if(i == 0) cout << "xx-xx xx:xx ";
            else cout << t.arriving[i].plusdate(query_date.date).get() << " ";
            cout << "-> ";
            if(i == t.stationNum - 1) cout << "xx-xx xx:xx ";
            else cout << t.leaving[i].plusdate(query_date.date).get() << " ";
            cout << t.pre_prices[i] << " " << t.seats[i] << endl;
        }
        return 2;
    }

    int delete_train(const string* cmd, const int siz) {
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
        this_t = Trains_unreleased.query(trainID);
        if(!this_t.first) return 0;
        Trains_unreleased.erase(trainID, this_t.second);
        return 1;
    }

    int query_ticket(const string* cmd, const int siz) {
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
        List<std::pair<type_trainID, int>> st; List<std::pair<type_trainID, int>>::iterator it;
        st = Database_stations.range(type_stationName_startTime(startS.c_str(), date), type_stationName_startTime(startS.c_str(), date + 1439));
        train_return this_t;
        type_train_tnc *list = new type_train_tnc[st.size()]; int cnt = 0;
        for(it = st.begin(); it != st.end(); ++it) {
            this_t = Trains.query((*it).first);
            if(!this_t.first) throw unknown_wrong();
            type_train &t = this_t.second;
            for(int i = (*it).second + 1; i < t.stationNum; ++i) if(t.stations[i] == endS) {
                int price = t.pre_prices[i] - t.pre_prices[(*it).second], seats = t.query((*it).second, i);
                // 先把输出信息写好存进string
                string out = (string)t.trainID + " " + startS + " " + t.leaving[(*it).second].plusdate(t.startdate.date).get() + " -> " + endS + " " + t.arriving[i].plusdate(t.startdate.date).get() + " " + std::to_string(price) + " " + std::to_string(seats);
                list[cnt++] = type_train_tnc(out, t.arriving[i] - t.leaving[(*it).second], price);
                break;
            }
        }
        sort(list, flag, way); // TODO: sort函数实现
        cout << cnt << endl;
        for(int i = 0; i < cnt; ++i) cout << list[i].out << endl;
        delete [] list;
        return 2;
        // startS.append("#"); startS.append(date.get_date());
        // endS.append("#"); endS.append(date.get_date)
    }

    int query_transfer(const string* cmd, const int siz) {
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
    }

    int buy_ticket(const string* cmd, const int siz) {
        int i = 1; bool vis[7] = {0}, flag = 0;
        type_order o;
        datentime date;
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
                    date = datentime("", cmd[++i]);
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
        if(!cur_u.first) return 0;
        this_u = Users.query(o.userName); // 查一下该用户的信息
        if(!this_u.first) throw unknown_wrong();
        train_return this_t = Trains.query((string)o.trainID + "#" + date.get_date());
        if(!this_t.first) return 0;
        type_train t = this_t.second; // 这里是复制，因为之后要modify这个车次的信息（seats）
        int p = 0, q = 0;
        for(int i = 0; i < t.stationNum; ++i) {
            if(o.startS == t.stations[i]) {
                if(q) return 0;
                else p = i;
            }
            if(o.endS == t.stations[i]) {
                if(!p) return 0;
                q = i;
            }
        }
        if(!p || !q) return 0;

        int seats_available = t.query(p, q);
        if(seats_available < o.num) {
            if(flag) { // 加入候补队列
                cout << "queue" << endl;
                // TODO:
            } else return 0;
        } else {
            o.price = t.pre_prices[q] - t.pre_prices[p];
            o.leavingTime = t.leaving[p].plusdate(date.date);
            o.arrivingTime = t.arriving[q].plusdate(date.date);
            long long total_cost = (long long)o.num * o.price;
            cout << total_cost << endl;
            Database_orders.insert(type_userName_orderID(o.userName, ++this_u.second.orderNum), o); // ++该用户的orderNum
            t.buy(p, q, o.num); // 改变座位数
            Trains.modify((string)o.trainID + "#" + date.get_date(), this_t.second, t);
        }
        return 2;
    }

    int query_order(const string* cmd, const int siz) {
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
        if(!cur_u.first) return 0;
        this_u = Users.query(username);
        if(!this_u.first) throw unknown_wrong();
        List<type_order> orders = Database_orders.range(type_userName_orderID(this_u.second.userName, this_u.second.orderNum), type_userName_orderID(this_u.second.userName, 1));
        cout << this_u.second.orderNum;
        for(List<type_order>::iterator it = orders.begin(); it != orders.end(); ++it) {
            cout << *it << endl;
        }
        return 2;
    }

    int refund_ticket(const string* cmd, const int siz) {
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
        if(!cur_u.first) return 0;
        this_u = Users.query(username);
        if(!this_u.first) throw unknown_wrong();
        which_order = this_u.second.orderNum - which_order + 1;
        order_return this_o = Database_orders.query(type_userName_orderID(this_u.second.userName, which_order));
        if(!this_o.first) return 0;
        if(this_o.second._type == refunded) return 0; // 已经退票了

        type_order o = this_o.second; o._type = refunded;
        Database_orders.modify(type_userName_orderID(this_u.second.userName, which_order), this_o.second, o);
        // TODO: queue中的操作
        return 1;
    }

    int clean(const string* cmd, const int siz) {
        First_User = 1;
        Users.clear();
        Cur_users.clear();
        Trains_unreleased.clear();
        Trains.clear();
        Database_stations.clear();
        Database_orders.clear();
        return 1;
    }
};

}

#endif
