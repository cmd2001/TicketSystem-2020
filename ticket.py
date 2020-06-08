
class Ticket:
    def __init__(self, para):
        self.para = para
    def add_user(self, cur_username, username, password, name, mailAddr):
        """
        :return: bool # 1 when success
        """
        return 1
    def login(self, username, password):
        """
        :return: bool
        """
        return
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
    def modify_profile(self, cur_username, username, password, name, mailAddr):
        """
        :param cur_username: str
        :param username: str
        :param password: (bool, str) # 1 means to modify, otherwise ignore [1].
        :param name: (bool, str)
        :param mailAddr: (bool, str)
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
    def query_train(self, trainID):
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
    def query_ticket(self, time, station, sort_param):
        """
        :return: ((trainID, FROM, LEAVING_TIME, TO, ARRIVING_TIME, PRICE, SEAT)*)
        HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
        """
        return
    def query_transfer(self, time, station, sort_param):
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
    add_user_form = '''    <form role = "form" action = "/add_user" method = "post">
        Current Username<input type = "text" name = "cur_username" required = "required"> <br>
        Username<input type = "text" name = "username" required = "required"> <br>
        Password<input type = "text" name = "password" required = "required"> <br>
        Name<input  type = "text" name = "name" required = "required"> <br>
        Mail Address<input  type = "text" name = "mailAddr" required = "required"> <br>
        <button type = "submit">submit</button>
    </form>'''

