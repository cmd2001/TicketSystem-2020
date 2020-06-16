import sys
from subprocess import *

class Ticket:
    def __init__(self, exe):
        self.exe = exe
        self.proc = Popen(exe, bufsize=1024, stdin=PIPE, stdout=PIPE)
        (self.fin, self.fout) = (self.proc.stdin, self.proc.stdout)

    def __del__(self):
        self.pipePrint('exit')
        self.pipeRead()

    def pipePrint(self, s):
        s += '\n' # '\n' matters!
        self.fin.write(s.encode('UTF-8'))
        self.fin.flush()

    def pipeRead(self):
        ret = ''
        while 1:
            x = self.fout.readline().decode('UTF-8')
            print('x = ', x)
            if x == ' ' or x == '':
                break
            if x == '$Final\n' or x == '$Failed\n': # ret contains $Final, '\n' matters!
                ret += x[0:-1]
                break
            else:
                ret += x
        return ret.split('\n')


    def add_user(self, cur_username, username, password, name, mailAddr, privilege):
        """
        :return: bool # 1 when success
        """
        com = 'add_user -c cur_username -u username -p password -n name -m mailAddr -g privilege'
        com = com.replace('cur_username', cur_username).replace('username', username).replace('password', password)\
                 .replace('name', name).replace('mailAddr', mailAddr).replace('privilege', privilege)
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'

    def login(self, username, password):
        """
        :return: bool
        """
        com = 'login -u username -p password'
        com = com.replace('username', username).replace('password', password)
        print('com = ', com)
        self.pipePrint(com)
        print('pipe Printed')
        ret = self.pipeRead()
        print('ret = ', ret)
        return ret[0] != '$Failed' and ret[0] != '-1'

    def logout(self, username):
        """
        :return: bool
        """
        com = 'logout -u username'
        com = com.replace('username', username)
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'


    def query_profile(self, cur_username, tar_username):
        """
        :return: (username，name, mailAddr, privilege)
        """
        com = 'query_profile -c cur_username -u tar_username'
        com = com.replace('cur_username', cur_username).replace('tar_username', tar_username)
        self.pipePrint(com)
        ret = self.pipeRead()
        if ret[0] == '$Failed' or ret[0] == '-1':
            return -1
        ret = ret[0].split(' ')
        return ret

    def modify_profile(self, cur_username, username, password, name, mailAddr, privilege):
        """
        :param cur_username: str
        :param username: str
        :param password: str # not empty means to modify, otherwise ignore.
        :param name: str
        :param mailAddr: str
        :return: bool
        """
        com = 'modify_profile -c cur_username -u username'
        com = com.replace('cur_username', cur_username).replace('username', username)
        if password:
            com += ' -p ' + password
        if name:
            com += ' -n ' + name
        if mailAddr:
            com += ' -m ' + mailAddr
        if privilege:
            com += ' -g ' + privilege
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'


    def add_train(self, trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type):
        """
        :return: None
        """
        com = 'add_train -i trainID -n stationNum -m seatNum -s stations -p prices -x startTime -t travelTimes -o stopoverTimes -d saleDate -y type'
        com = com.replace('trainID', trainID).replace('stationNum', str(stationNum)).replace('seatNum', seatNum)\
                 .replace('stations', stations).replace('prices', prices).replace('startTime', startTime)\
                 .replace('travelTimes', travelTimes).replace('stopoverTimes', stopoverTimes).replace('saleDate', saleDate)\
                 .replace('type', type)
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'

    def release_train(self, trainID):
        """
        :return: None
        """
        com = 'release_train -i trainID'
        com = com.replace('trainID', trainID)
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'

    def query_train(self, trainID, date):
        """
        :return: ((trainID, type), (stations, ARRIVING_TIME, LEAVING_TIME, PRICE, SEAT)*)
        # 07-02 05:19 -> 07-02 05:24 114 1000
        """
        com = 'query_train -i trainID -d date'
        com = com.replace('trainID', trainID).replace('date', date)
        self.pipePrint(com)
        ret0 = self.pipeRead()
        if ret0[0] == '$Failed' or ret0[0] == '-1':
            return -1
        ret = []
        for i in range(0, len(ret0) - 1):
            ret.append(ret0[i].split(' '))
        print('ret = ', ret)
        return ret

    def delete_train(self, trainID):
        """
        :return: bool
        """
        com = 'delete_train -i trainID'
        com = com.replace('trainID', trainID)
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'


    def query_ticket(self, time, start, end, sort_param):
        """
        :return: [(trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, SEAT)*]
        HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
        """
        com = 'query_ticket -s start -t end -d time'
        com = com.replace('time', time).replace('start', start).replace('end', end)
        if sort_param:
            com += ' -p ' + sort_param
        self.pipePrint(com)
        ret0 = self.pipeRead()
        if ret0[0] == '$Failed' or ret0[0] == '-1':
            return -1
        print('ret0 = ', ret0)
        ret = [[start, end, time]]
        for i in range(1, len(ret0) - 1):
            ret.append(ret0[i].split(' '))
        return ret

    def query_transfer(self, time, start, end, sort_param):
        """
        :return: [(trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, SEAT)*2]
        """
        com = 'query_transfer -s start -t end -d time'
        com = com.replace('time', time).replace('start', start).replace('end', end)
        if sort_param:
            com += ' -p ' + sort_param
        self.pipePrint(com)
        ret0 = self.pipeRead()
        if ret0[0] == '$Failed' or ret0[0] == '-1':
            return -1
        ret = [[start, end, time]]
        for i in range(1, len(ret0) - 1):
            ret.append(ret0[i].split(' '))
        return ret

    def buy_ticket(self, username, trainID, day, ffrom, to, number, que):
        """
        :param que: bool
        :return: int or 'queue', -1 when fail
        """
        com = 'buy_ticket -u username -i trainID -d day -n number -f ffrom -t to'
        com = com.replace('username', username).replace('trainID', trainID).replace('day', day)\
                 .replace('ffrom', ffrom).replace('to', to).replace('number', number)
        if que:
            com += ' -q true'
        self.pipePrint(com)
        ret = self.pipeRead()
        if ret[0] == '$Failed':
            return -1
        return ret[0]

    def query_order(self, username):
        """
        :return: [(status, trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, NUM)*]
        # [pending] HAPPY_TRAIN 上院 08-17 05:24 -> 下院 08-17 15:24 628 500
        """
        com = 'query_order -u username'
        com = com.replace('username', username)
        self.pipePrint(com)
        ret0 = self.pipeRead()
        if ret0[0] == '$Failed' or ret0[0] == '-1':
            return -1
        ret = []
        for i in range(1, len(ret0) - 1):
            ret.append(ret0[i].split(' '))
        return ret

    def refund_ticket(self, username, num):
        """
        :param num: int, refund kth order from last order.
        :return: bool, fail when refund twice
        """
        com = 'refund_ticket -u username'
        com = com.replace('username', username)
        if num:
            com += ' -i ' + num
        self.pipePrint(com)
        ret = self.pipeRead()
        return ret[0] != '$Failed' and ret[0] != '-1'

    def clean(self):
        """
        :return: None
        """
        com = 'clean'
        self.pipePrint(com)
        self.pipeRead()
        return
    def exit(self):
        """
        :return: None
        """
        com = 'exit'
        self.pipePrint(com)
        self.pipeRead()
        return

class Constant:
    add_user_form = [('Username', 'username', 1), ('Password', 'password', 1), ('Name', 'name', 1), ('Mail Address', 'mailAddr', 1), ('Privilege', 'privilege', 1)]
    query_profile_form = [('Username', 'username', 1)]
    modify_profile_form = [('Username', 'username', 0), ('Password', 'password', 0), ('Name', 'name', 0), ('Mail Address', 'mailAddr', 0), ('Privilege', 'privilege', 0)]
    add_train_form = [('Train ID', 'trainID', 1), ('Seat Number', 'seatNum', 1), ("Stations (splited by single '|')" ,'stations', 1),
                      ("Prices (splited by single '|')", 'prices', 1), ('Start Time', 'startTime', 1), ("Travel Times (splited by single '|')", 'travelTimes', 1),
                      ("Stopover Times (splited by single '|')", 'stopoverTimes', 1), ('Sale Date', 'saleDate', 1), ('Train Type', 'type', 1)]
    release_train_form = [('Train ID', 'trainID', 1)]
    query_train_form = [('Train ID', 'trainID', 1), ('Date', 'date', 1)]
    delete_train_form = [('Train ID', 'trainID', 1)]
    query_ticket_form = [('Time', 'time', 1), ('Start Station', 'start', 1), ('Destination Station', 'end', 1), ('Sort parameter', 'sort_param', 0)]
    query_transfer_form = [('Time', 'time', 1), ('Start Station', 'start', 1), ('Destination Station', 'end', 1), ('Sort parameter', 'sort_param', 0)]
    buy_ticket_form = [('Train ID', 'trainID', 1), ('Date', 'day', 1), ('From Station', 'ffrom', 1), ('To Station', 'to', 1),
                       ('Number', 'number', 1), ('Queue? (0 or 1)', 'que', 1)]
    refund_ticket_form = [('Order Number (From Last)', 'num', 1)]
    login_form = [('Username', 'username', 1), ('Password', 'password', 1)]
    clean_form = [('Enter Your Username Here to Authenticate', 'username', 1)]
    shutdown_form = [('Enter Your Username Here to Authenticate', 'username', 1)]
    init_form = [('Username', 'username', 1), ('Password', 'password', 1), ('Name', 'name', 1), ('Mail Address', 'mailAddr', 1)]

    query_train_list = (0, 1, 2, 4, 5, 6, 7)
    query_train_table_head = ('Station', 'Arrival Date', 'Arrival Time', 'Departure Date', 'Departure Time', 'Cumulative Price', 'Remaining Seat')

    query_ticket_list = (0, 1, 2, 3, 5, 6, 7, 8, 9)
    query_ticket_table_head = ('Train ID', 'Start Station', 'Start Date', 'Start Time',
                               'Arrival Station', 'Arrival Date', 'Arrival Time', 'Ticket Price', 'Remaining Seat')
    query_transfer_list = (0, 1, 2, 3, 5, 6, 7, 8, 9)
    query_transfer_table_head = ('Train ID', 'Start Station', 'Start Date', 'Start Time',
                               'Arrival Station', 'Arrival Date', 'Arrival Time', 'Ticket Price', 'Remaining Seat')
    query_order_list = (0, 1, 2, 3, 4, 6, 7, 8, 9, 10)
    query_order_table_head = ('Status', 'Train ID', 'Start Station', 'Start Date', 'Start Time',
                              'Arrival Station', 'Arrival Date', 'Arrival Time', 'Ticket Price', 'Numbers')

from uuid import uuid4

class cookiePool:
    def __init__(self):
        self.pool = {}
    def clean(self):
        self.pool.clear()

    def push(self, userName):
        id = str(uuid4())
        self.pool[id] = userName
        return id
    def erase(self, id):
        self.pool.pop(id)

    def check(self, id):
        return id in self.pool
    def query(self, id):
        return self.pool[id]

class initChecker():
    def __init__(self, arg):
        self.arg = arg

        try:
            file = open(arg, 'r')
            file.close()
        except IOError:
            file = open(arg, 'w')
            file.write('0')

        file = open(arg, 'r')
        self.sta = bool(int(file.read()))

    def check(self):
        return self.sta

    def fil(self):
        file = open(self.arg, 'w')
        file.write('1')
        file.close()
        self.sta = 1

    def reset(self):
        file = open(self.arg, 'w')
        file.write('0')
        file.close()

