#ifndef TICKET_CLASS_HPP
#define TICKET_CLASS_HPP

#include "../BPlusTree/database_cached.hpp"
using __Amagi::database_cached;
#include "exceptions.hpp"
#include "tools.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::string;

namespace MLJ {

/**
 * 各函数返回值均为std::string
 * 异常抛出与操作失败分离
 */
class Ticket {

    static const int username_SIZE = 22;
    static const int pswd_SIZE = 32;
    static const int name_SIZE = 22;
    static const int mailAddr_SIZE = 32;
    static const int trainID_SIZE = 28;
    static const int station_SIZE = 42;
    static const int MAXstationNum = 102;
    static const int MAXseatNum = 100005;

public:
    typedef class wrapped_cstr {
        friend ostream &operator<<(ostream &os, const wrapped_cstr &a) {
            os << a.str;
            return os;
        }
    public:
        char str[trainID_SIZE];
        wrapped_cstr() = default;
        wrapped_cstr(const string &s) { strcpy(str, s.c_str()); }
        wrapped_cstr(const char *s) { strcpy(str, s); }
        bool operator<(const wrapped_cstr &o) const { return strcmp(str, o.str) < 0; }
        bool operator==(const wrapped_cstr &o) const { return strcmp(str, o.str) == 0; }
    }type_userName, type_trainID;

    class datentime {
    public:
        int date = 0;
        int minu = 0;

        // 时间 + 日期
        datentime(const string &t = "", const string &d = "") {
            if(d != "") {
                int month = (d[0] - '0') * 10 + d[1] - '0';
                switch(month) {
                    case 12: date += 30; // Dec.
                    case 11: date += 31; // Nov.
                    case 10: date += 30; // Oct.
                    case 9:  date += 31; // Sept.
                    case 8:  date += 31; // Aug.
                    case 7:  date += 30; // July
                    case 6:  date += 31; // June
                    case 5:  date += 30; // May
                    case 4:  date += 31; // Apr.
                    case 3:  date += 29; // March
                    case 2:  date += 31; // Feb.
                    case 1:  date += 0; break; // Jan.
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
        inline bool operator<=(const datentime &o) const {
            return !(*this > o);
        }
        inline string get_date() const {
            string ret;
            int data = date;
            if(date <= 31) ret =  "01-";
            else if(date <= 60)  ret = "02-", data -= 31;
            else if(date <= 91)  ret = "03-", data -= 60;
            else if(date <= 121) ret = "04-", data -= 91;
            else if(date <= 152) ret = "05-", data -= 121;
            else if(date <= 182) ret = "06-", data -= 152;
            else if(date <= 213) ret = "07-", data -= 182;
            else if(date <= 244) ret = "08-", data -= 213;
            else if(date <= 274) ret = "09-", data -= 244;
            else if(date <= 305) ret = "10-", data -= 274;
            else if(date <= 335) ret = "11-", data -= 305;
            else if(date <= 366) ret = "12-", data -= 335;
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
        char userName[username_SIZE];
        char password[pswd_SIZE];
        char name[name_SIZE]; // 2-5个汉字
        char mailAddr[mailAddr_SIZE];
        int privilege; // 0-10

        int orderNum; // 储存该用户订单的数量
        type_user() { privilege = 0; orderNum = 0; } // FIXME: memset 0?
    };
    typedef std::pair<bool, type_user> user_return;
    typedef std::pair<bool, int> cur_return;

    class type_train {
    private:
    public:
        char trainID[trainID_SIZE];
        int stationNum = 0; // 0-100
        char stations[MAXstationNum][station_SIZE]; // 10个汉字以内
        int pre_prices[MAXstationNum]; //票价前缀和
        int seatNum = 0;    // 0-100000
        datentime startTime;
        datentime arriving[MAXstationNum]; // 前缀和 但不包含startdate
        datentime leaving[MAXstationNum];  // 前缀和 但不包含startdate
        datentime saleDate[2];
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
    };
    typedef std::pair<bool, type_train> train_return;

    class type_train_release {
    public:
        char runtimeID[trainID_SIZE];
        datentime startdate; //始发日期，仅在列车release之后有
        int seats[MAXstationNum]; // 每站的座位数
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
        inline void buy(const int &l, const int &r, const int &k) { //左闭右开
            for(int i = l; i < r; ++i) seats[i] -= k;
        }
        inline void refund(const int &l, const int &r, const int &k) { //左闭右开
            for(int i = l; i < r; ++i) seats[i] += k;
        }
        inline int queryseats(const int &l, const int &r) const { //左闭右开
            int mn = __INT_MAX__;
            for(int i = l; i < r; ++i) mn = std::min(mn, seats[i]);
            return mn;
        }
    };
    typedef std::pair<bool, type_train_release> release_return;

    class type_stationName_startTime { // 包括列车runtimeID作为第三关键字（防止key值相同的情况发生）
    public:
        char stationName[station_SIZE];
        datentime startTime;
        char runtimeID[trainID_SIZE];
        type_stationName_startTime() = default;
        type_stationName_startTime(const char *_n, const datentime &_t, const char *_runtimeid) {
            strcpy(stationName, _n);
            startTime = _t;
            strcpy(runtimeID, _runtimeid);
        }
        inline bool operator<(const type_stationName_startTime &o) const {
            int _ = strcmp(stationName, o.stationName);
            return (_ == 0)? ((startTime == o.startTime)? strcmp(runtimeID, o.runtimeID) < 0 : startTime < o.startTime) : _ < 0;
        }
    };

    class type_query_value {
    public:
        char runtimeID[trainID_SIZE];
        int which_in_order; // 车次中的第几站
        int stationNum;
        datentime arriving_time; // 包含起始时间
        datentime leaving_time;
        int pre_price;
        type_query_value() = default;
        type_query_value(const char *_id, const int &_order, const int &_sta_num, const datentime &_arriving, const datentime &_leaving, const int &_pre_price):
        which_in_order(_order), stationNum(_sta_num), arriving_time(_arriving), leaving_time(_leaving), pre_price(_pre_price) {
            strcpy(runtimeID, _id);
        }
        bool operator==(const type_query_value &o) const {
            return strcmp(runtimeID, o.runtimeID) == 0 && which_in_order == o.which_in_order;
        }
    };

    class type_train_tnc { // 在query_ticket中用来排序，精简版type_train
    public:
        string out;
        string trainID; // 列车id作为第二排序关键字
        int time;
        int cost;
        type_train_tnc() { out.clear(); }
        type_train_tnc(const string &_out, const string &_trainid, const int &_t, const int &_c): time(_t), cost(_c) {
            trainID = _trainid; out = _out;
        }
    };
    class cmp_time {
    public:
        bool operator()(const type_train_tnc &a, const type_train_tnc &b) {
            return a.time == b.time? a.trainID < b.trainID : a.time < b.time;
        }
    };
    class cmp_cost {
    public:
        bool operator()(const type_train_tnc &a, const type_train_tnc &b) {
            return a.cost == b.cost? a.trainID < b.trainID : a.cost < b.cost;
        }
    };

    class type_train_transfer_tnc { // 在query_transfer中用来排序
    public:
        string out;
        int time_1; // 第一辆列车的运行时间，作为第二排序关键字
        int time;
        int cost;
        type_train_transfer_tnc() { out.clear(); }
        type_train_transfer_tnc(const string &_out, const int &_t_1, const int &_t, const int &_c): time_1(_t_1), time(_t), cost(_c) {
            out = _out;
        }
    };
    class cmp_transfer_time {
    public:
        bool operator()(const type_train_transfer_tnc &a, const type_train_transfer_tnc &b) {
            return a.time == b.time? a.time_1 < b.time_1 : a.time < b.time;
        }
    };
    class cmp_transfer_cost {
    public:
        bool operator()(const type_train_transfer_tnc &a, const type_train_transfer_tnc &b) {
            return a.cost == b.cost? a.time_1 < b.time_1 : a.cost < b.cost;
        }
    };

    class type_userName_orderID {
    public:
        char userName[username_SIZE];
        int ID = 0;
        type_userName_orderID() = default;
        type_userName_orderID(const char *_n, const int &_id) {
            strcpy(userName, _n);
            ID = _id;
        }
        inline bool operator<(const type_userName_orderID &o) const {
            int ans = strcmp(userName, o.userName);
            return (ans == 0)? ID > o.ID : ans < 0; // 倒序排列，所以ID越大越先
        }
        inline bool operator==(const type_userName_orderID &o) const {
            return strcmp(userName, o.userName) == 0 && ID == o.ID;
        }
    };
    enum Ordertype { success, pending, refunded };
    class type_order {
    public:
        int totalID;
        Ordertype _type;
        char userName[username_SIZE];
        char runtimeID[trainID_SIZE];
        char startS[station_SIZE], endS[station_SIZE];
        int startNum, endNum; // 在车次中的站号
        datentime leavingTime, arrivingTime;
        int price;
        int num;
        type_order() = default;
        inline bool operator==(const type_order &o) const {
            return _type == o._type && strcmp(userName, o.userName) == 0 && strcmp(runtimeID, o.runtimeID) == 0
            && strcmp(startS, o.startS) == 0 && strcmp(endS, o.endS) == 0 && num == o.num;
        }
        inline string get() const {
            string ans = "";
            switch(_type) {
            case success: ans = "[success] "; break;
            case pending: ans = "[pending] "; break;
            case refunded: ans = "[refunded] "; break;
            }
            ans += string(runtimeID).substr(0, strlen(runtimeID) - 6) + " " + startS + " " + leavingTime.get() + " -> " + endS + " " + arrivingTime.get() + " " + std::to_string(price) + " " + std::to_string(num);
            return ans;
        }
    };
    typedef std::pair<bool, type_order> order_return;

    class type_queue_key { // 候补队列数据库的key，含列车的runtimeID和order的totalID
    public:
        char runtimeID[trainID_SIZE];
        int totalID;
        type_queue_key() = default;
        type_queue_key(const char *train_id, const int &total_id): totalID(total_id) {
            strcpy(runtimeID, train_id);
        }
        inline bool operator<(const type_queue_key &o) const {
            int ans = strcmp(runtimeID, o.runtimeID);
            return (ans == 0)? totalID < o.totalID : ans < 0;
        }
    };

    class type_stationName_trainID {
    public:
        char stationName[station_SIZE], trainID[trainID_SIZE];
        type_stationName_trainID() = default;
        type_stationName_trainID(const char* sn, const char* id) {
            strcmp(stationName, sn), strcmp(trainID, id);
        }
        friend bool operator < (const type_stationName_trainID &a, const type_stationName_trainID &b) {
            auto c1 = strcmp(a.stationName, b.stationName);
            if(c1) return c1 < 0;
            return strcmp(a.trainID, b.trainID);
        }
    };

    class type_ticket_key {
    public:
        datentime saledate[2], arriving, leaving;
        int stationRank, sumPrice;
        type_ticket_key() = default;
        type_ticket_key(const datentime& startTime, const datentime& endTime, const datentime &_arriving, const datentime &_leaving,
                    const int &_stationRank, const int &_sumPrice): arriving(_arriving), leaving(_leaving), stationRank(_stationRank), sumPrice(_sumPrice) {
            saledate[0] = startTime, saledate[1] = endTime;
        }
    };

    database_cached<type_userName, type_user> Users;
    database_cached<type_userName, int> Cur_users; // 作为一个索引，仅判断是否在当前列表中，查询详细信息还要在Users中查询
    database_cached<type_trainID, type_train> Trains_base;
    database_cached<type_trainID, type_train_release> Trains_released;
    database_cached<type_stationName_startTime, type_query_value> Database_stations; // 按车站和出发时间记录车次和该站编号
    database_cached<type_userName_orderID, type_order> Database_orders;
    database_cached<type_queue_key, type_userName_orderID> Database_queue; // 候补队列，value为order_key和totalID
    int totalID_runtime; // 运行时的总orderID，在退出时写入文件file_totalID

    database_cached<type_stationName_trainID, type_ticket_key> Database_query;

private:
    char MAXID[trainID_SIZE], MINID[trainID_SIZE];

public:

    Ticket():
        Users("file_users"),
        Cur_users("file_cur_users"),
        Trains_base("file_trains_base"), Trains_released("file_trains_released"),
        Database_stations("file_stations"),
        Database_orders("file_orders"),
        Database_queue("file_queue"),
        Database_query("file_query") {
        // 读取总orderID
        ifstream totalID_in;
        totalID_in.open("file_totalID", ios::in);
        if(!totalID_in) {
            totalID_runtime = 0;
        } else totalID_in >> totalID_runtime;
        totalID_in.close();
        // 若在线列表非空则清空在线列表（针对强制退出情况）
        if(!Cur_users.empty()) Cur_users.clear();

        memset(MINID, 0, sizeof(MINID) - 1); MINID[sizeof(MINID) - 1] = '\0';
        memset(MAXID, 127, sizeof(MAXID) - 1); MAXID[sizeof(MAXID) - 1] = '\0';
    }

private:
    inline type_train_release get_release(const string &str) { // 获取train_release信息
        release_return this_t_release = Trains_released.query(str);
        if(!this_t_release.first) throw unknown_wrong();
        return this_t_release.second;
    }
    inline void update_totalID() const {
        ofstream totalID_out; // 写入总购买ID数
        totalID_out.open("file_totalID", ios::trunc);
        totalID_out << totalID_runtime;
        totalID_out.close();
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

        Cur_users.erase(username);
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
        newuser.orderNum = that_u.second.orderNum;
        if(!vis[2]) strcpy(newuser.password, that_u.second.password);
        if(!vis[3]) strcpy(newuser.name, that_u.second.name);
        if(!vis[4]) strcpy(newuser.mailAddr, that_u.second.mailAddr);
        if(!vis[5]) newuser.privilege = that_u.second.privilege;

        ans = (string)newuser.userName + " " + newuser.name + " " + newuser.mailAddr + " " + std::to_string(newuser.privilege);
        if(!(newuser == that_u.second)) Users.modify(newuser.userName, newuser);
        return ans;
    }


// *************************************** Train Operations **********************************************

    string add_train(const string* cmd, const int &siz) {
        int i = 1;
        bool vis[10] = {0};
        type_train newtrain;
        int travelTimes[MAXstationNum] = {0}, stopoverTimes[MAXstationNum] = {0};
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
                    string tmp[MAXstationNum];
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) {
                        strcpy(newtrain.stations[k], tmp[k].c_str());
                    }
                } else throw illegal_arg();
            } else if(cmd[i] == "-p") {
                if(!vis[4]) {
                    vis[4] = 1;
                    int _siz = 0;
                    string tmp[MAXstationNum];
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
                    string tmp[MAXstationNum];
                    split(cmd[++i], tmp, _siz, '|');
                    for(int k = 0; k < _siz; ++k) travelTimes[k] = atoi(tmp[k].c_str());
                } else throw illegal_arg();
            } else if(cmd[i] == "-o") {
                if(!vis[7]) {
                    vis[7] = 1;
                    int _siz = 0;
                    string tmp[MAXstationNum];
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
                    string tmp[MAXstationNum];
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

    // 添加Train_released，更改Train_base中该车次的is_released为true，并添加Database_stations
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
        type_train &t = this_t.second;
        t.is_released = 1;
        Trains_base.modify(trainID, t); // modify Train_base, is_released = 1

        string runtimeID; // 后置数字后列入Trains_released(例如：trainA#06-20)
        datentime everyday_starttime = t.saleDate[0];
        type_train_release t_release(t.stationNum, t.seatNum);
        for(int i = 0; i <= t.saleDate[1].date - t.saleDate[0].date; ++i) {
            t_release.startdate = everyday_starttime;
            runtimeID = (string)t.trainID + "#" + t_release.startdate.get_date();
            strcpy(t_release.runtimeID, runtimeID.c_str());
            Trains_released.insert(runtimeID, t_release);

            for(int j = 0; j < t.stationNum; ++j) { // 按车站和出发时间（含天）列入Database_stations
//                cout << t.stations[j] << " " << t.arriving[j].plusdate(everyday_starttime.date).get() << t.leaving[j].plusdate(everyday_starttime.date).get() << " " << t_release.runtimeID << " " << j << endl;
                Database_stations.insert(type_stationName_startTime(t.stations[j], t.leaving[j].plusdate(everyday_starttime.date), t_release.runtimeID),
                        type_query_value(t_release.runtimeID, j, t.stationNum, t.arriving[j].plusdate(everyday_starttime.date), t.leaving[j].plusdate(everyday_starttime.date), t.pre_prices[j]));
            }
            everyday_starttime.add_date(1);
//            cout << runtimeID << endl;
        }

        for(int i = 0; i < t.stationNum; i++) {
            Database_query.insert(type_stationName_trainID(t.stations[i], t.trainID),
                    type_ticket_key(t.saleDate[0], t.saleDate[1], t.arriving[i], t.leaving[i], i, t.pre_prices[i]));
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
        Trains_base.erase(trainID);
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
        if(startS == endS) return "0";

        auto st = Database_query.range2(type_stationName_trainID(startS.c_str(), MINID), type_stationName_trainID(startS.c_str(), MAXID));
        auto ed = Database_query.range2(type_stationName_trainID(endS.c_str(), MINID), type_stationName_trainID(endS.c_str(), MAXID));

        auto p1 = st.begin(), p2 = ed.begin();

        auto list = new type_train_tnc[min(st.size(), ed.size())];
        int cnt = 0;

        while(p1 != st.end() && p2 != ed.end()) {
            auto cmp = strcmp(p1->first.trainID, p2->first.trainID);
            if(cmp < 0) ++p1;
            else if(cmp > 0) ++p2;
            else {
                auto sd0 = p1->second.saledate[0], sd1 = p1->second.saledate[1];
                auto firstDay = date.minusdate(p1->second.leaving.date);
                if(sd0 <= firstDay && firstDay <= sd1) {
                    auto t_release = get_release(string(p1->first.trainID) + "#" + firstDay.get_date());
                    auto seats = t_release.queryseats(p1->second.stationRank, p2->second.stationRank);
                    auto price = p2->second.sumPrice - p1->second.sumPrice;
                    auto out = string(p1->first.trainID) + " " + startS + " " +
                               p1->second.leaving.plusdate(firstDay.date).get() + " -> " + endS + " " +
                               p2->second.arriving.plusdate(firstDay.date).get() + " " + std::to_string(price) + " " +
                               std::to_string(seats);

                    list[cnt++] = type_train_tnc(out, p1->first.trainID, p2->second.arriving - p1->second.leaving, price);
                }
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
        if(startS == endS) return "0";
//        mapA::map<string, int> trainID_set; // 优化处理，若之前查询过同一车辆且发车日期<当天发车日期，则直接跳过
        // st查询起始站对应有哪些车次，et查询从当天至世界末日终点站对应有哪些车次
        // 思路：枚举st车次经过的站点，枚举et逆向经过的站点，两者对应且符合时间先后则可行
        auto st = Database_stations.range(type_stationName_startTime(startS.c_str(), date, MINID), type_stationName_startTime(startS.c_str(), date + 1439, MAXID));
        auto et = Database_stations.range(type_stationName_startTime(endS.c_str(), date, MINID), type_stationName_startTime(endS.c_str(), datentime("23:59", "12-31"), MAXID));
        if(st.empty() || et.empty()) return "0";
        type_train_transfer_tnc choosed_transfer;
        for(auto & it_1 : st) {
//            trainID_set.clear();
            datentime startdate_1("", string(it_1.runtimeID).substr(strlen(it_1.runtimeID) - 5, 5));
            string trainID_1 = string(it_1.runtimeID).substr(0, strlen(it_1.runtimeID) - 6);

            auto first_t = Trains_base.query(trainID_1); // 取出车次名（不带日期）
            if(!first_t.first) throw unknown_wrong();
            const type_train &t_1 = first_t.second;

            for(auto & it_2 : et) {
                datentime startdate_2("", string(it_2.runtimeID).substr(strlen(it_2.runtimeID) - 5, 5));
                string trainID_2 = string(it_2.runtimeID).substr(0, strlen(it_2.runtimeID) - 6);
                if(trainID_2 == trainID_1) continue;

                auto second_t = Trains_base.query(trainID_2);
                if(!second_t.first) throw unknown_wrong();
                const type_train &t_2 = second_t.second;

//                if(trainID_set.count(trainID_2) && trainID_set[trainID_2] < startdate_2.date) continue;
                bool have_legal_ones = false, out_of_bound = false;

                for(int i = it_1.which_in_order + 1; i < t_1.stationNum; ++i) {
                    for(int j = it_2.which_in_order - 1; j >= 0; --j) {
                        if(!(t_2.leaving[j].plusdate(startdate_2.date) > t_1.arriving[i].plusdate(startdate_1.date))) { out_of_bound = true; break; }
                        if (strcmp(t_2.stations[j], t_1.stations[i]) == 0) {
                            have_legal_ones = true;

                            const type_train_release t_release_1 = get_release(it_1.runtimeID);
                            const type_train_release t_release_2 = get_release(it_2.runtimeID);
                            int price_1 = t_1.pre_prices[i] - t_1.pre_prices[it_1.which_in_order],
                                price_2 = t_2.pre_prices[it_2.which_in_order] - t_2.pre_prices[j];
                            int seats_1 = t_release_1.queryseats(it_1.which_in_order, i),
                                seats_2 = t_release_2.queryseats(j, it_2.which_in_order);
                            int time_1 = t_1.arriving[i] - t_1.leaving[it_1.which_in_order];
                            int total_time = t_2.arriving[it_2.which_in_order].plusdate(t_release_2.startdate.date) -
                                             t_1.leaving[it_1.which_in_order].plusdate(t_release_1.startdate.date);
                            // 先把输出信息写好存进string
                            string out = (string) t_1.trainID + " " + startS + " " +
                                         t_1.leaving[it_1.which_in_order].plusdate(t_release_1.startdate.date).get() + " -> " +
                                         t_1.stations[i] + " " +
                                         t_1.arriving[i].plusdate(t_release_1.startdate.date).get() + " " +
                                         std::to_string(price_1) + " " + std::to_string(seats_1) + "\n";
                            out += (string) t_2.trainID + " " + t_2.stations[j] + " " +
                                   t_2.leaving[j].plusdate(t_release_2.startdate.date).get() + " -> " + endS + " " +
                                   t_2.arriving[it_2.which_in_order].plusdate(t_release_2.startdate.date).get() + " " +
                                   std::to_string(price_2) + " " + std::to_string(seats_2);
                            type_train_transfer_tnc newtnc(out, time_1, total_time, price_1 + price_2);
                            if (choosed_transfer.out.empty()) choosed_transfer = newtnc;
                            else {
                                if (flag) {
                                    if (cmp_transfer_cost()(newtnc, choosed_transfer)) choosed_transfer = newtnc;
                                } else {
                                    if (cmp_transfer_time()(newtnc, choosed_transfer)) choosed_transfer = newtnc;
                                }
                            }
//                            cout << out << endl;
                            break;
                        }
                    }
                }
//                if(have_legal_ones) trainID_set[trainID_2] = startdate_2.date;
//                if(!out_of_bound && !have_legal_ones) trainID_set[trainID_2] = startdate_2.date;
            }
        }
        if(choosed_transfer.out.empty()) return "0";
        else return choosed_transfer.out;
    }

    string buy_ticket(const string* cmd, const int &siz) {
        string ans = "";
        int i = 1; bool vis[7] = {0}, flag = 0;
        type_order o;
        string trainID;
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
                    trainID = cmd[++i];
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
        if(strcmp(o.startS, o.endS) == 0) return "-1";
        // cout << o.userName << endl;
        this_u = Users.query(o.userName); // 查一下该用户的信息
        if(!this_u.first) throw unknown_wrong();
        train_return this_t = Trains_base.query(trainID);
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
        strcpy(o.runtimeID, ID_with_date.c_str());
        o.startNum = p; o.endNum = q;
        o.price = t.pre_prices[q] - t.pre_prices[p];
        o.leavingTime = t.leaving[p].plusdate(startdate.date);
        o.arrivingTime = t.arriving[q].plusdate(startdate.date);

        type_train_release t_release = get_release(ID_with_date);
        int seats_available = t_release.queryseats(p, q);
        if(seats_available < o.num) {
            if(flag && o.num <= t.seatNum) { // -q为true且票数不大于总座位数时加入候补队列
                ++totalID_runtime;
                update_totalID();

                o._type = pending;
                o.totalID = totalID_runtime;
                type_user &u = this_u.second;
                type_userName_orderID neworder_key(o.userName, ++u.orderNum);
                Database_orders.insert(neworder_key, o); // 添加Database_orders
                Users.modify(this_u.second.userName, u); // 更新该用户的orderNum

                Database_queue.insert(type_queue_key(ID_with_date.c_str(), totalID_runtime), neworder_key); // 进入queue
                ans = "queue";
            } else return "-1";
        } else { // 购票成功
            ++totalID_runtime; // 总orderID++
            update_totalID();

            o._type = success;
            o.totalID = totalID_runtime;
            long long total_cost = (long long)o.num * o.price;
            type_user &u = this_u.second;
            type_userName_orderID neworder_key(o.userName, ++u.orderNum);
            Database_orders.insert(neworder_key, o); // 添加Database_orders
            Users.modify(this_u.second.userName, u); // 更新该用户的orderNum
            t_release.buy(p, q, o.num); // 改变座位数
            Trains_released.modify(ID_with_date, t_release); // 更新该车次的信息

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
        ans = std::to_string(this_u.second.orderNum);
        if(this_u.second.orderNum) {
            List<type_order> orders = Database_orders.range(type_userName_orderID(this_u.second.userName, this_u.second.orderNum), type_userName_orderID(this_u.second.userName, 1));
            for(auto &order : orders) {
                ans += (string)"\n" + order.get();
            }
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

        type_order o_refund = this_o.second;
        o_refund._type = refunded;
        Database_orders.modify(type_userName_orderID(this_u.second.userName, which_order), o_refund);
        if(this_o.second._type == pending) { // 若为pending，则删去该候补
            Database_queue.erase(type_queue_key(o_refund.runtimeID, o_refund.totalID));
            return "0";
        }
        // 为success，退票
        type_train_release t_release = get_release(o_refund.runtimeID);
        t_release.refund(o_refund.startNum, o_refund.endNum, o_refund.num); // 退票，座位复原

//        cout << o_refund.runtimeID << " " << totalID_runtime << endl;
        auto queue = Database_queue.range(type_queue_key(o_refund.runtimeID, 1), type_queue_key(o_refund.runtimeID, totalID_runtime));
        for(auto &key: queue) {
            auto o_return = Database_orders.query(key);
            if(!o_return.first) throw unknown_wrong();
            auto o_todo = o_return.second;
            if(o_refund.startNum > o_todo.endNum || o_refund.endNum < o_todo.startNum) continue; // 未修改相应车站区间，则不必考虑
            int seats_remained = t_release.queryseats(o_todo.startNum, o_todo.endNum);
            if(seats_remained >= o_todo.num) { // 候补购票成功
                o_todo._type = success;
                Database_orders.modify(key, o_todo); // 更改相应用户的order
                Database_queue.erase(type_queue_key(o_refund.runtimeID, o_todo.totalID)); // 删去该候补
                t_release.buy(o_todo.startNum, o_todo.endNum, o_todo.num); // 购买车票
            }
        }
        Trains_released.modify(o_refund.runtimeID, t_release);
        return "0";
    }

    string clean(const string* cmd, const int &siz) {
        totalID_runtime = 0;
        update_totalID();
        Users.clear();
        Cur_users.clear();
        Trains_base.clear();
        Trains_released.clear();
        Database_stations.clear();
        Database_orders.clear();
        Database_queue.clear();
        return "0";
    }

    string exit(const string* cmd, const int &siz) {
        Cur_users.clear(); // 清空在线用户列表
        return "bye";
    }
};

}

#endif
