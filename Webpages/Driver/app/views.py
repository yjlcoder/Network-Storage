from flask.ext.login import login_user, login_required, logout_user, current_user
from app import app, forms, db, models, login_manager
from flask import render_template, request, redirect, flash, url_for
from app.utils import buildTree
import hashlib


@app.route("/")
@app.route("/index")
@login_required
def index():
    path = request.args.get('path')
    if path is None or path == "":
        path = '/'
        return redirect(url_for('index', path=path))
    files = models.File.query.filter_by(userid=current_user.id).all()
    files = buildTree(files=files, visit=path)
    return render_template("index.html", path=path, files=files)


@app.route("/login", methods=['GET', 'POST'])
def login():
    if current_user is not None and current_user.is_authenticated:
        flash("您已经登录，请注销后重试")
        return redirect('index')
    form = forms.LoginForm(request.form)
    if request.method == "POST" and form.validate():
        user = models.User.query.filter_by(username=form.username.data).first()
        if user is None:
            flash("不存在该用户名")
            return redirect('login')
        if user.password != hashlib.sha1(form.password.data.encode('utf-8')).hexdigest():
            flash("登录失败, 密码错误")
            return redirect('login')
        login_user(user, remember=form.remember_me.data)
        flash("登录成功")
        next = request.args.get('next')
        if next is None or next == "":
            return redirect('index')
        else:
            return redirect(next)
    return render_template('login.html', form = form)


@app.route("/register", methods=['GET', 'POST'])
def register():
    form = forms.RegisterForm(request.form)
    if request.method == "POST" and form.validate():
        db.create_all()
        find = models.User.query.filter_by(username=form.username.data).first()
        if(find):
            flash("该用户名已经被注册")
            return redirect('register')
        user = models.User(username=form.username.data, password=hashlib.sha1(form.password.data.encode('utf-8')).hexdigest())
        file = models.File(virtualpath='/', uploader=user)
        db.session.add(user)
        db.session.commit()
        db.session.add(file)
        db.session.commit()
        flash("注册成功")
        return redirect('login')
    return render_template('register.html', form = form)


@login_manager.user_loader
def load_user(user_id):
    return models.User.query.get(int(user_id))

@login_manager.unauthorized_handler
def unauthorized():
    return redirect('login')


@app.route('/logout')
@login_required
def logout():
    flash("注销成功")
    logout_user()
    return redirect('login')