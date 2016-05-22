from app import app, db
db.create_all()
app.run(debug=True, host="192.168.1.101", port=8000)
