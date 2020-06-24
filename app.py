from flask import Flask, render_template, request, redirect, make_response
from ticket import Ticket, Constant, cookiePool, initChecker

rootUsername = 'root'
rootPassword = 'root'
rootMailAddr = 'root@ticket.system'

app = Flask(__name__)
ticket = Ticket('./backend')
ini = initChecker('inited.txt')

userPool = cookiePool()

def initRoot(): # root cannot login in web frontend.
    tmp = ticket.login(rootUsername, rootPassword)
    if not tmp:
        tmp = ticket.add_user('-1', rootUsername, rootPassword, rootUsername, rootMailAddr, '10')
        if not tmp:
            print('Internal Sever Error!')
            exit(0)
        ticket.login(rootUsername, rootPassword)

def logoutRoot():
    ticket.logout(rootUsername)

def checkCookie():
    userID = request.cookies.get('id')
    return userPool.check(userID)

def preCheck():
    return ini.check() and checkCookie()

@app.route('/login', methods=("GET", "POST"))
def login():
    if not ini.check():
        return redirect('/')
    if checkCookie():
        return redirect('/')

    if request.method == "GET":
        return render_template('form.html', flag = True, form = Constant.login_form, form_path = '/login', title = 'User Login')

    form = request.form.to_dict()
    check = ticket.login(form['username'], form['password'])
    if not check:
        return render_template('form.html', flag = True, message = 'Failed', form = Constant.login_form, form_path = '/login', title = 'User Login')
    else:
        ret = make_response(redirect('/'))
        tmp = ticket.query_profile(form['username'], form['username'])
        ret.set_cookie('id', userPool.push(form['username'], (tmp[1], tmp[3])))
        return ret

@app.route('/logout')
def logout():
    if not preCheck():
        return redirect('/')
    userID = request.cookies.get('id')
    name = userPool.query(userID)
    ticket.logout(name[0])
    userPool.erase(userID)
    return redirect('/')


def getUsername():
    userID = request.cookies.get('id')
    return userPool.query(userID)[0]

def getFriendlyname():
    userID = request.cookies.get('id')
    return userPool.query(userID)[1]

def checkPrivilege(): # users with privilege level >= 1 are administrators.
    userID = request.cookies.get('id')
    return int(userPool.query(userID)[2]) > 1

@app.route('/init', methods=("GET", "POST"))
def init():
    if request.method == "GET":
        return render_template('form.html', flag = True, form = Constant.init_form, form_path = '/init', title = 'System Initialization')
    form = request.form.to_dict()
    if form['password'] != form['password2']:
        return render_template('form.html', message = 'Failed', flag = True, form = Constant.init_form, form_path = '/init', title = 'System Initialization')

    ret = ticket.add_user(rootUsername, form['username'], form['password'], form['name'], form['mailAddr'], '9') # privilege of 9
    if ret:
        ini.fil()
    else:
        return render_template('form.html', message = 'Failed', flag = True, form = Constant.init_form, form_path = '/init', title = 'System Initialization')
    return redirect('/')


@app.route('/')
def root():
    if not ini.check():
        return redirect('/init')
    userID = request.cookies.get('id')
    if not checkCookie():
        return render_template('root.html', flag = 1)
    return render_template('root.html', privilege = checkPrivilege(), username = getFriendlyname())



@app.route('/add_user', methods=("GET", "POST"))
def add_user():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')
    form = request.form.to_dict()
    ret = ticket.add_user(getUsername(), form['username'], form['password'], form['name'], form['mailAddr'], form['privilege'])
    if ret:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Succeed!', form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')

@app.route('/query_profile', methods=("GET", "POST"))
def query_profile():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.query_profile_form, form_path = '/query_profile', title = 'Query Profile')
    form = request.form.to_dict()
    ret = ticket.query_profile(getUsername(), form['username'])
    if ret == -1:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.query_profile_form, form_path = '/query_profile', title = 'Query Profile')
    return render_template('query_profile.html', privilege = checkPrivilege(), username = getFriendlyname(), ret = ret)

@app.route('/modify_profile', methods=("GET", "POST"))
def modify_profile():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')
    form = request.form.to_dict()

    if form['password'] != form['password2']:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')

    ret = ticket.modify_profile(getUsername(), form['username'], form['password'], form['name'], form['mailAddr'], form['privilege'])
    if ret:
        username = getUsername() # flush friendly name
        tmp = ticket.query_profile(username, username)[1]
        ret2 = make_response(render_template('form.html', privilege = checkPrivilege(), username = tmp, message = 'Succeed!', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile'))
        ret2.set_cookie('id', userPool.push(username, tmp))
        return ret2
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')

@app.route('/add_train', methods=("GET", "POST"))
def add_train():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')
    form = request.form.to_dict()
    ret = ticket.add_train(form['trainID'], len(form['stations'].split('|')), form['seatNum'], form['stations'],
                           form['prices'], form['startTime'], form['travelTimes'], form['stopoverTimes'], form['saleDate'], form['type'])
    if ret:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Succeed!', form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')

@app.route('/release_train', methods=("GET", "POST"))
def release_train():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')
    form = request.form.to_dict()
    ret = ticket.release_train(form['trainID'])
    if ret:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Succeed!', form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')

@app.route('/query_train', methods=("GET", "POST"))
def query_train():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.query_train_form, form_path = '/query_train', title = 'Query Train')
    form = request.form.to_dict()
    ret = ticket.query_train(form['trainID'], form['date'])
    if ret == -1:
        render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.query_train_form, form_path = '/query_train', title = 'Query Train')
    return render_template('table.html', privilege = checkPrivilege(), username = getFriendlyname(), title = 'Train', ret = ret, ret_len = len(ret), table_head = Constant.query_train_table_head, col_list = Constant.query_train_list)

@app.route('/delete_train', methods=("GET", "POST"))
def delete_train():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')
    form = request.form.to_dict()
    ret = ticket.delete_train(form['trainID'])
    if ret:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Succeed!', form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')

@app.route('/query_ticket', methods=("GET", "POST"))
def query_ticket():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.query_ticket_form, form_path = '/query_ticket', title = 'Query Ticket')
    form = request.form.to_dict()
    ret = ticket.query_ticket(form['time'], form['start'], form['end'], form['sort_param'])
    if ret == -1:
        render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.query_ticket_form, form_path = '/query_ticket', title = 'Query Ticket')
    return render_template('table.html', privilege = checkPrivilege(), username = getFriendlyname(), title = 'Ticket', ret = ret, ret_len = len(ret), table_head = Constant.query_ticket_table_head, col_list = Constant.query_ticket_list)

@app.route('/query_transfer', methods=("GET", "POST"))
def query_transfer():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.query_transfer_form, form_path = '/query_transfer', title = 'Query Transfer')
    form = request.form.to_dict()
    ret = ticket.query_transfer(form['time'], form['start'], form['end'], form['sort_param'])
    if ret == -1:
        render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.query_transfer_form, form_path = '/query_ticket', title = 'Query Transfer')
    return render_template('table.html', privilege = checkPrivilege(), username = getFriendlyname(), title = 'Transfer', ret = ret, ret_len = len(ret), table_head = Constant.query_transfer_table_head, col_list = Constant.query_transfer_list)


@app.route('/buy_ticket', methods=("GET", "POST"))
def buy_ticket():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.buy_ticket_form, form_path = '/buy_ticket', title = 'Buy Ticket')
    form = request.form.to_dict()
    que = '1' if 'que' in form else '0'
    ret = ticket.buy_ticket(getUsername(), form['trainID'], form['day'], form['ffrom'], form['to'], form['number'], que)
    if ret == -1:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.buy_ticket_form, form_path = '/buy_ticket', title = 'Buy Ticket')
    elif ret == 'queue':
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Queuing', form = Constant.buy_ticket_form, form_path = '/buy_ticket', title = 'Buy Ticket')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = ('Total Cost: ' + str(ret)), form = Constant.buy_ticket_form, form_path = '/buy_ticket', title = 'Buy Ticket')

@app.route('/query_order')
def query_order():
    if not preCheck():
        return redirect('/')
    ret = ticket.query_order(getUsername()) # it can never be -1
    return render_template('table.html', privilege = checkPrivilege(), username = getFriendlyname(), title = 'Order', ret = ret, ret_len = len(ret), table_head = Constant.query_order_table_head, col_list = Constant.query_order_list)


@app.route('/refund_ticket', methods=("GET", "POST"))
def refund_ticket():
    if not preCheck():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')
    form = request.form.to_dict()
    ret = ticket.refund_ticket(getUsername(), form['num'])
    if ret:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Succeed!', form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')

@app.route('/clean', methods=("GET", "POST"))
def clean():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), warning = 'Are You Sure to Clear All Data?', form = Constant.clean_form, form_path = '/clean', title = 'Clear Data')
    form = request.form.to_dict()
    if form['username'] == getUsername():
        logoutRoot()
        ticket.clean()
        userPool.clean()
        ini.reset()
        initRoot()
        return redirect('/')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', warning = 'Are You Sure to Clear All Data?', form = Constant.clean_form, form_path = '/clean', title = 'Clear Data')

@app.route('/shutdown', methods=("GET", "POST"))
def shutdown():
    if not preCheck() or not checkPrivilege():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), warning = 'Are You Sure to Shutdown the System?', form = Constant.shutdown_form, form_path = '/shutdown', title = 'Shutdown System')
    form = request.form.to_dict()
    if form['username'] == getUsername():
        logoutRoot()
        ticket.exit()
        return redirect('/')
    else:
        return render_template('form.html', privilege = checkPrivilege(), username = getFriendlyname(), message = 'Failed!', warning = 'Are You Sure to Shutdown the System?', form = Constant.shutdown_form, form_path = '/shutdown', title = 'Shutdown System')

@app.route('/buy_ticket2', methods=("GET", "POST"))
def buy_ticket2():
    if not preCheck():
        return redirect('/')
    if request.method == "GET": # unable to be accessed by get
        return redirect('/')
    form = request.form.to_dict()

    if not ('number' in form):
        return render_template('buy_ticket2.html', privilege = checkPrivilege(), username = getFriendlyname(), trainID = form['trainID'], day = form['day'], ffrom = form['ffrom'], to = form['to'])
    else:
        que = '1' if 'que' in form else '0'
        ret = ticket.buy_ticket(getUsername(), form['trainID'], form['day'], form['ffrom'], form['to'], form['number'], que)
        if ret == -1:
            return render_template('buy_ticket2.html', message = 'Failed', privilege = checkPrivilege(), username = getFriendlyname(), trainID = form['trainID'], day = form['day'], ffrom = form['ffrom'], to = form['to'])
        elif ret == 'queue':
            return render_template('buy_ticket2.html', message = 'Queue', privilege = checkPrivilege(), username = getFriendlyname(), trainID = form['trainID'], day = form['day'], ffrom = form['ffrom'], to = form['to'])
        else:
            return render_template('buy_ticket2.html', message = ('Total Cost: ' + str(ret)), privilege = checkPrivilege(), username = getFriendlyname(), trainID = form['trainID'], day = form['day'], ffrom = form['ffrom'], to = form['to'])

@app.route('/register', methods=("GET", "POST"))
def register():
    if not ini.check():
        return redirect('/')
    if request.method == "GET":
        return render_template('form.html', flag = True, form = Constant.register_form, form_path = '/register', title = 'User Registration')
    form = request.form.to_dict()
    if form['password'] != form['password2']:
        return render_template('form.html', flag = True, message = 'Failed', form = Constant.register_form, form_path = '/register', title = 'User Registration')

    ret = ticket.add_user(rootUsername, form['username'], form['password'], form['name'], form['mailAddr'], '1')

    if ret:
        return redirect('/login')
    else:
        return render_template('form.html', flag = True, message = 'Failed', form = Constant.register_form, form_path = '/register', title = 'User Registration')

initRoot()

if __name__ == '__main__':
    app.run(debug = True)