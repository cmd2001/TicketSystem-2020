from flask import Flask, render_template, request, redirect, make_response
from ticket import Ticket, Constant, cookiePool

app = Flask(__name__)
ticket = Ticket(None)

userPool = cookiePool()

def checkCookie():
    userID = request.cookies.get('id')
    return userPool.check(userID)

@app.route('/login', methods=("GET", "POST"))
def login():
    if checkCookie():
        return redirect('/')

    if request.method == "GET":
        return render_template('form.html', form = Constant.login_form, form_path = '/login', title = 'User Login')

    form = request.form.to_dict()
    check = ticket.login(form['username'], form['password'])
    if not check:
        return render_template('failure.html', form = Constant.login_form, form_path = '/login', title = 'User Login')
    else:
        ret = make_response(redirect('/'))
        ret.set_cookie('id', userPool.push(form['username']))
        return ret

@app.route('/logout')
def logout():
    userID = request.cookies.get('id')
    if not checkCookie():
        return redirect('/')

    name = userPool.query(userID)
    ticket.logout(name)
    userPool.erase(userID)
    return redirect('/')


def getUsername():
    userID = request.cookies.get('id')
    return userPool.query(userID)

# todo: check whether system has initialized

@app.route('/')
def root():
    userID = request.cookies.get('id')
    if not checkCookie():
        return render_template('root_login.html')
    return render_template('root.html', username = userPool.query(userID))



@app.route('/add_user', methods=("GET", "POST"))
def add_user():
    if request.method == "GET":
        return render_template('form.html', form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')
    form = request.form.to_dict()
    ret = ticket.add_user(getUsername(), form['username'], form['password'], form['name'], form['mailAddr'], form['privilege'])
    if ret:
        return render_template('success.html', form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')
    else:
        return render_template('failure.html', form = Constant.add_user_form, form_path = '/add_user', title = 'Add User')

@app.route('/query_profile', methods=("GET", "POST"))
def query_profile():
    if request.method == "GET":
        return render_template('form.html', form = Constant.query_profile_form, form_path = '/query_profile', title = 'Query Profile')
    form = request.form.to_dict()
    ret = ticket.query_profile(getUsername(), form['username'])
    return 'Query Profile' # todo

@app.route('/modify_profile', methods=("GET", "POST"))
def modify_profile():
    if request.method == "GET":
        return render_template('form.html', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')
    form = request.form.to_dict()
    ret = ticket.modify_profile(getUsername(), form['username'], form['password'], form['name'], form['mailAddr'], form['privilege'])
    if ret:
        return render_template('success.html', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')
    else:
        return render_template('failure.html', form = Constant.modify_profile_form, form_path = '/modify_profile', title = 'Modify Profile')

@app.route('/add_train', methods=("GET", "POST"))
def add_train():
    if request.method == "GET":
        return render_template('form.html', form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')
    form = request.form.to_dict()
    ret = ticket.add_train(form['trainID'], len(form['stations'].split('|')), form['seatNum'], form['stations'],
                           form['prices'], form['startTime'], form['travelTimes'], form['stopoverTimes'], form['saleDate'], form['type'])
    if ret:
        return render_template('success.html', form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')
    else:
        return render_template('failure.html', form = Constant.add_train_form, form_path = '/add_train', title = 'Add Train')

@app.route('/release_train', methods=("GET", "POST"))
def release_train():
    if request.method == "GET":
        return render_template('form.html', form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')
    form = request.form.to_dict()
    ret = ticket.release_train(form['trainID'])
    if ret:
        return render_template('success.html', form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')
    else:
        return render_template('failure.html', form = Constant.release_train_form, form_path = '/release_train', title = 'Release Train')

@app.route('/query_train', methods=("GET", "POST"))
def query_train():
    if request.method == "GET":
        return render_template('form.html', form = Constant.query_train_form, form_path = '/query_train', title = 'Query Train')
    form = request.form.to_dict()
    ret = ticket.query_train(form['trainID'], form['date'])
    return 'Query Train' # todo

@app.route('/delete_train', methods=("GET", "POST"))
def delete_train():
    if request.method == "GET":
        return render_template('form.html', form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')
    form = request.form.to_dict()
    ret = ticket.delete_train(form['trainID'])
    if ret:
        return render_template('success.html', form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')
    else:
        return render_template('failure.html', form = Constant.delete_train_form, form_path = '/delete_train', title = 'Delete Train')

@app.route('/query_ticket', methods=("GET", "POST"))
def query_ticket():
    if request.method == "GET":
        return render_template('form.html', form = Constant.query_ticket_form, form_path = '/query_train', title = 'Query Ticket')
    form = request.form.to_dict()
    ret = ticket.query_ticket(form['time'], form['start'], form['end'], form['sort_param'])
    return 'Query Ticket' # todo

@app.route('/query_transfer', methods=("GET", "POST"))
def query_transfer():
    if request.method == "GET":
        return render_template('form.html', form = Constant.query_transfer_form, form_path = '/query_transfer', title = 'Query Transfer')
    form = request.form.to_dict()
    ret = ticket.query_transfer(form['time'], form['start'], form['end'], form['sort_param'])
    return 'Query Transfer' # todo

@app.route('/buy_ticket', methods=("GET", "POST"))
def buy_ticket():
    if request.method == "GET":
        return render_template('form.html', form = Constant.buy_ticket_form, form_path = '/buy_ticket', title = 'Buy Ticket')
    form = request.form.to_dict()
    ret = ticket.buy_ticket(getUsername(), form['trainID'], form['day'], form['ffrom'], form['to'], form['number'], form['que'])
    return 'Buy Ticket' # todo

@app.route('/query_order')
def query_order():
    ret = ticket.query_order(getUsername())
    return 'Query Order' # todo

@app.route('/refund_ticket', methods=("GET", "POST"))
def refund_ticket():
    if request.method == "GET":
        return render_template('form.html', form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')
    form = request.form.to_dict()
    ret = ticket.refund_ticket(getUsername(), form['trainID'])
    if ret:
        return render_template('success.html', form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')
    else:
        return render_template('failure.html', form = Constant.refund_ticket_form, form_path = '/refund_ticket', title = 'Refund Ticket')

@app.route('/clean')
def clean():
    #todo Warning
    ticket.clean()
    userPool.clean()
    return redirect('/')


if __name__ == '__main__':
    app.run(debug = True)