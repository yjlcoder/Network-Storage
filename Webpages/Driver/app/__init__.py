from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
import pymysql
from flask.ext.login import LoginManager


#Initial
pymysql.install_as_MySQLdb()
login_manager = LoginManager()


app = Flask(__name__)
app.config.from_object('config')
login_manager.init_app(app)
db = SQLAlchemy(app)

from app import views, models
