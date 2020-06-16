#include "src/core.hpp"

MLJ::Ticket ticket;
const char *CMDs[] = {"add_user",       "login",         "logout",         "query_profile",
                      "modify_profile", "add_train",     "release_train",  "query_train",
                      "delete_train",   "query_ticket",  "query_transfer", "buy_ticket",
                      "query_order",    "refund_ticket", "clean",          "exit" };

string (MLJ::Ticket::*FUNCs[])(const string *, const int &) = {
    &MLJ::Ticket::add_user,       &MLJ::Ticket::login,         &MLJ::Ticket::logout,         &MLJ::Ticket::query_profile,
    &MLJ::Ticket::modify_profile, &MLJ::Ticket::add_train,     &MLJ::Ticket::release_train,  &MLJ::Ticket::query_train,
    &MLJ::Ticket::delete_train,   &MLJ::Ticket::query_ticket,  &MLJ::Ticket::query_transfer, &MLJ::Ticket::buy_ticket,
    &MLJ::Ticket::query_order,    &MLJ::Ticket::refund_ticket, &MLJ::Ticket::clean,          &MLJ::Ticket::exit };

#define debug cerr

int main() {
    static bool is_legal_cmd;
    cerr << "C++ INITED" << endl;
    while(true) {
        string cmd_list[30], src;
        int siz = 0;
        getline(cin, src); //
        debug << "src = " << src << endl;
        is_legal_cmd = 0;
        MLJ::split(src, cmd_list, siz, ' ');
        for(int i = 0; i < siz; i++) debug << cmd_list[i] << " "; debug << endl;
        for(int i = 0; i < sizeof(CMDs) / sizeof(CMDs[0]); ++i)
            if(cmd_list[0] == CMDs[i]) {
                debug << "i = " << i << cmd_list[0] << endl;
                try {
                    debug << "C++ in try" << endl;
                    string tmp = (ticket.*FUNCs[i])(cmd_list, siz);
                    debug << "got tmp" << endl;
                    debug << "in C++ out = " << tmp << "\n$Final" << endl;
                    cout << tmp << "\n$Final" << endl; // magic code
                    if(tmp == "bye") exit(0);
                } catch(...) {
                    debug << "failed1" << endl;
                    cout << "$Failed" << endl; // magic code
                }
                is_legal_cmd = 1;
                break;
            }
        if(!is_legal_cmd) cout << "$Failed" << endl; // magic code
    }
}
