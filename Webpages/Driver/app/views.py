from app import app, forms, db, models
from flask import render_template, request, redirect
import hashlib


@app.route("/")
@app.route("/index")
def index():
    return render_template("index.html")


@app.route("/login", methods=['GET', 'POST'])
def login():
    form = forms.LoginForm(request.form)
    if request.method == "POST" and form.validate():
        #TODO: change status
        return redirect('index')
    return render_template('login.html', form = form)


@app.route("/register", methods=['GET', 'POST'])
def register():
    form = forms.RegisterForm(request.form)
    if request.method == "POST" and form.validate():
        db.create_all()
        user = models.User(username=form.username.data, password=hashlib.sha1(form.password.data.encode('utf-8')).hexdigest())
        db.session.add(user)
        db.session.commit()
        return redirect('login')
    return render_template('register.html', form = form)
