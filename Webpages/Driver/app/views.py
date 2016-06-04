from flask.ext.login import login_user, login_required, logout_user, current_user
from app import app, forms, db, models, login_manager
from flask import render_template, request, redirect, flash, url_for, jsonify
from sqlalchemy import and_
from app.utils import buildTree, getFiles
from werkzeug.utils import secure_filename
import hashlib, os, datetime
from flask import current_app, send_from_directory
from copy import deepcopy


@app.route("/")
@app.route("/index")
@login_required
def index():
    path = request.args.get('path')
    if path is None or path == "":
        path = '/'
        return redirect(url_for('index', path=path))
    files = getFiles(path)
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


@app.route('/upload', methods=['POST'])
@login_required
def upload():
    md5 = request.form['md5']
    saveto = request.form['saveto']
    name = request.form['name']
    path = os.path.join(app.config['UPLOADED_FOLDER'], md5)
    files = getFiles(saveto)
    for file in files:
        if file[0] == name and file[1] is not None:
            flash("文件已存在")
            return redirect("index")
    if os.path.isfile(path):
        flash("秒传成功")
        file = models.File(
            userid = current_user.id,
            virtualpath = saveto + name,
            md5 = md5
        )
        db.session.add(file)
        db.session.commit()
    else:
        form = forms.UploadForm(request.form)
        file = request.files[form.file.name]
        if file:
            file.save(path)
            status = models.Status(
                md5=md5,
                status=1
            )
            db.session.add(status)
            db.session.commit()
            file = models.File(
                userid = current_user.id,
                virtualpath = saveto + name,
                status=status
            )
            db.session.add(file)
            db.session.commit()
            flash("文件上传成功")
        else:
            flash("文件上传失败")
    return redirect(url_for('index', path=request.args.get('path')))


@app.route('/download/<filename>')
def download(filename):
    hash = request.args.get('hash')
    path = os.path.join(current_app.root_path, app.config['UPLOADED_FOLDER'])
    return send_from_directory(directory=path, filename=hash)


@app.route('/newFolder')
def newFolder():
    path = request.args.get('path')
    name = request.args.get('name')
    next = request.args.get('next')
    file = models.File(
        userid=current_user.id,
        virtualpath=path+name+'/',
        md5=None
    )
    db.session.add(file)
    db.session.commit()
    if next is not None:
        return redirect(next)
    else:
        return url_for(index, path=path+name)


@app.route('/delete')
def delete():
    path = request.args.get('path')
    name = request.args.get('name')
    if path is None and name is None:
        wholepath = request.args.get('wholepath')
        pathsplit = wholepath.split('/')
        if pathsplit[len(pathsplit)-1] == '':
            name = pathsplit[len(pathsplit)-2] + "/"
            path = ""
            for pathfragile in pathsplit[0:len(pathsplit)-2]:
                if pathfragile == '':
                    path = path + '/'
                else:
                    path = path + pathfragile + '/'
        else:
            name = pathsplit[len(pathsplit)-1]
            path = ""
            for pathfragile in pathsplit[0:len(pathsplit)-1]:
                if pathfragile == '':
                    path = path + '/'
                else:
                    path = path + pathfragile + '/'
    if name[len(name)-1] == '/':
        # Folder
        files = models.File.query.filter(and_(models.File.userid==current_user.id, models.File.virtualpath.like(path+name+'%'))).all()
        for file in files:
            db.session.delete(file)
        db.session.commit()
    else:
        file = models.File.query.filter_by(userid=current_user.id, virtualpath=path+name).first()
        db.session.delete(file)
        db.session.commit()
    return redirect(url_for('index', path=path))

@app.route('/copy')
def copy():
    src = request.args.get('src')
    dest = request.args.get('dest')
    path = request.args.get('path')
    srcpath = request.args.get('srcpath')
    next = request.args.get('next')
    if path is None:
        path = '/'
    if srcpath is None:
        srcpath = '/'
    if path[len(path)-1] != '/':
        path = path + '/'
    if src is None:
        flash("错误: 不合法的参数")
        return render_template('jump.html', path=url_for('index', path=path))
    elif dest is None:
        files = getFiles(path)
        return render_template('select.html', path=path, src=src, files=files, srcpath=srcpath, next=next)
    else:
        try:
            include = dest.index(src) == 0
        except:
            include = False
        if include:
            flash("复制失败：目录有包含关系")
            return render_template('jump.html', path=url_for('index', path=path))
        files = getFiles(dest)
        name = src.split('/')
        if name[len(name)-1] == '':
            # Folder
            name = (name[len(name)-2], 'Folder')
        else:
            # File
            name = (name[len(name)-1], 'File')
        for file in files:
            if file[0] == name[0] and ((name[1] == 'Folder' and file[1] is None) or (name[1] == 'File' and file[2] is not None)):
                flash("目的文件夹内有相同文件或目录")
                return render_template('jump.html', path=url_for('index', path=path))
        if name[1] == 'File':
            srcFile = models.File.query.filter_by(userid=current_user.id, virtualpath=src).first()
            destFile = models.File(userid=current_user.id, virtualpath=dest+name[0], md5=srcFile.md5 )
            db.session.add(destFile)
            db.session.commit()
        elif name[1] == 'Folder':
            srcFiles = models.File.query.filter(and_(models.File.userid==current_user.id, models.File.virtualpath.like(src+'%'))).all()
            for srcFile in srcFiles:
                destFile = models.File(userid=current_user.id, virtualpath=srcFile.virtualpath.replace(srcpath, dest, 1), md5=srcFile.md5)
                db.session.add(destFile)
            db.session.commit()
        if next is None:
            return render_template('jump.html', path=url_for('index', path=path))
        else:
            return render_template('jump.html', path=next)

@app.route('/move')
def move():
    src=request.args.get('src')
    srcpath = request.args.get('srcpath')
    return redirect(url_for('copy', src=src, srcpath=srcpath, next=url_for('delete', wholepath=src)))

@app.route('/ajax')
def ajax():
    type = request.args.get('type')
    if type == 'md5':
        md5 = request.args.get('md5')
        status = models.Status.query.filter_by(md5=md5).first()
        ret = False
        if status is not None and status.status == 1:
            ret = True
        return jsonify(status=ret)
