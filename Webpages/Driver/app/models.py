from app import db

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True, nullable=False)
    username = db.Column(db.String(128), index=True, unique=True, nullable=False)
    password = db.Column(db.String(40), index=True, nullable=False)

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
