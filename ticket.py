
class Ticket:
    def __init__(self, para):
        self.para = para
    def add_user(self, cur_username, username, password, name, mailAddr, privilege):
        """
        :return: bool # 1 when success
        """
        return
    def login(self, username, password):
        """
        :return: bool
        """
        return 1
    def logout(self, username):
        """
        :return: bool
        """
        return
    def query_profile(self, cur_username, tar_username):
        """
        :return: (username，name, mailAddr, privilege)
        """
        return
    def modify_profile(self, cur_username, username, password, name, mailAddr, privilege):
        """
        :param cur_username: str
        :param username: str
        :param password: str # not empty means to modify, otherwise ignore.
        :param name: str
        :param mailAddr: str
        :return: bool
        """
        return
    def add_train(self, trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type):
        """
        :return: None
        """
        return
    def release_train(self, trainID):
        """
        :return: None
        """
        return
    def query_train(self, trainID, date):
        """
        :return: ((trainID, type), (stations, ARRIVING_TIME, LEAVING_TIME, PRICE, SEAT)*)
        # 07-02 05:19 -> 07-02 05:24 114 1000
        """
        return
    def delete_train(self, trainID):
        """
        :return: bool
        """
        return
    def query_ticket(self, time, start, end, sort_param):
        """
        :return: ((trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, SEAT)*)
        HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
        """
        return
    def query_transfer(self, time, start, end, sort_param):
        """
        :return: ((trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, SEAT)*2)
        """
        return
    def buy_ticket(self, username, trainID, day, ffrom, to, number, que):
        """
        :param que: bool
        :return: int or 'queue', -1 when fail
        """
    def query_order(self, username):
        """
        :return: ((status, trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, NUM)*)
        # [pending] HAPPY_TRAIN 上院 08-17 05:24 -> 下院 08-17 15:24 628 500
        """
        return
    def refund_ticket(self, username, num):
        """
        :param num: int, refund kth order from last order.
        :return: bool, fail when refund twice
        """
        return
    def clean(self):
        """
        :return: None
        """
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


