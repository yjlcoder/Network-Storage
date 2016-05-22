from app import app
from wtforms import Form, BooleanField, StringField, PasswordField
from wtforms import validators, ValidationError

def usernameCheck(form, field):
    valid = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    for i in field.data:
        if i not in valid:
            raise ValidationError("用户名只能是数字或字母")

class RegisterForm(Form):
    username = StringField('Username', [validators.DataRequired(message="请输入用户名"), validators.Length(min=5, max=15, message="用户名长度不符合要求，请输入5-15位用户名"), usernameCheck])
    password = PasswordField('Password', [validators.DataRequired(message="请输入密码"), validators.Length(min=5, max=15, message="密码长度不符合要求, 请输入5-15位密码")])
    confirm = PasswordField('Confirm', [validators.DataRequired(message="请再次输入密码"), validators.equal_to('password', message='两次密码不匹配')])

    def __repr__(self):
        return "Username:" + self.username.data + ", Password: " + self.password.data + ", Confirm: " + self.confirm.data


class LoginForm(Form):
    username = StringField('Username', [validators.DataRequired(message="请输入用户名"), validators.Length(min=5, max=15, message="用户名长度不符合要求，请输入5-15位用户名"), usernameCheck])
    password = PasswordField('Password', [validators.DataRequired(message="请输入密码"), validators.Length(min=5, max=15, message="密码长度不符合要求, 请输入5-15位密码")])
    remember_me = BooleanField("Remember Me")

    def __repr__(self):
        return "Username:" + self.username.data + ", Password: " + self.password.data + ", Remember Me:" + str(self.remember_me.data)
