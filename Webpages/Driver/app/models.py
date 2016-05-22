from app import db

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True, nullable=False)
    username = db.Column(db.String(128), index=True, unique=True, nullable=False)
    password = db.Column(db.String(40), nullable=False)
    files = db.relationship('File', backref="uploader", lazy='dynamic')

    def __repr__(self):
        return "<User %r>" % (self.username)

    @property
    def is_authenticated(self):
        return True

    @property
    def is_active(self):
        return True

    @property
    def is_anonymous(self):
        return False

    def get_id(self):
        return str(self.id)

class File(db.Model):
    userid = db.Column(db.Integer, db.ForeignKey('user.id'), primary_key=True)
    virtualdir = db.Column(db.String(190), primary_key=True)
    md5 = db.Column(db.CHAR(32), nullable=False)

    def __repr__(self):
        return "<UserID: %r, Virtual Directory: %r>" %(self.userid, self.virtualDir)