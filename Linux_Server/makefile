main:server.exe
server.exe:server.o database/DB_Operate.o database/MyDB.o database/DB_Info.o
	g++ server.o \
		database/DB_Operate.o \
		database/MyDB.o \
		database/DB_Info.o \
		-o server.exe \
		-L/usr/lib64/mysql -lmysqlclient \
		-L/usr/local/bin -lcrypto

server.o:server.cpp
	g++ server.cpp -c -o server.o

DB_Operate.o:database/DB_Operate.cpp
	g++ database/DB_Operate.cpp -c -o database/DB_Operate.o

MyDB.o:database/MyDB.cpp
	g++ database/MyDB.cpp -c -o database/MyDB.o

DB_Info.o:Ddatabase/B_Info.cpp
	g++ database/DB_Info.cpp -c -o database/DB_Info.o

clean:
	rm -rf server.o database/DB_Operate.o database/MyDB.o database/DB_Info.o

