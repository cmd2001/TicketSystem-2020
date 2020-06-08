from flask import Flask, render_template, request, redirect, make_response, send_from_directory
from ticket import Ticket
from ticket import Constant

app = Flask(__name__)
ticket = Ticket(None)

@app.route('/add_user', methods=("GET", "POST"))
def add_user():
    if request.method == "GET":
        return render_template('form.html', form = Constant.add_user_form)
    form = request.form.to_dict()
    ret = ticket.add_user(form['cur_username'], form['username'], form['password'], form['name'], form['mailAddr'])
    if ret:
        return render_template('success.html', form = Constant.add_user_form)
    else:
        return render_template('failure.html', form = Constant.add_user_form)


@app.route('/')
def root():
    return 'Hello World!'

if __name__ == '__main__':
    app.run(debug = True)