#include "core.hpp"

MLJ::Ticket ticket;
const char *CMDs[] = {"add_user",       "login",         "logout",         "query_profile",
                      "modify_profile", "add_train",     "release_train",  "query_train",
                      "delete_train",   "query_ticket",  "query_transfer", "buy_ticket",
                      "query_order",    "refund_ticket", "clean",          "exit" };
string (MLJ::Ticket::*FUNCs[])(const string *, const int &) = {
    MLJ::Ticket::add_user,       MLJ::Ticket::login,         MLJ::Ticket::logout,         MLJ::Ticket::query_profile,
    MLJ::Ticket::modify_profile, MLJ::Ticket::add_train,     MLJ::Ticket::release_train,  MLJ::Ticket::query_train,
    MLJ::Ticket::delete_train,   MLJ::Ticket::query_ticket,  MLJ::Ticket::query_transfer, MLJ::Ticket::buy_ticket,
    MLJ::Ticket::query_order,    MLJ::Ticket::refund_ticket, MLJ::Ticket::clean,          MLJ::Ticket::exit };

/** Split a string by ch
 */

int main() {
    string src;
    getline(cin, src);
    while(true) {
        int siz;
        bool is_legal_cmd = 0;
        string cmd_list[30];
        MLJ::split(src, cmd_list, siz, ' ');
        // cout << siz << endl;
        // for(int i = 0; i < siz; ++i) cout << cmd_list[i] << endl;
        for(int i = 0; i < sizeof(CMDs) / sizeof(CMDs[0]); ++i)
            if(cmd_list[0] == CMDs[i]) {
                try {
                    string tmp = (ticket.*FUNCs[i])(cmd_list, siz);
                    cout << tmp << endl;
                    if(tmp == "bye") goto OUT;
                } catch(...) {
                    cout << "-1" << endl;
                }
                is_legal_cmd = 1;
                break;
            }
        if(!is_legal_cmd) cout << "illegal!" << endl;
        getline(cin, src);
    }
    OUT:
    return 0;
}

/*
add_user -c asdasd -u mlj -p 19679145 -n 闵乐钧 -m 897744820@qq.com -g 10
login -u mlj -p 19679145
modify_profile -c mlj -u mlj -g 10
*/