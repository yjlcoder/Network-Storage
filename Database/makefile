main:DB_Test
DB_Test:DB_Test.o DB_Operate.o MyDB.o DB_Info.o
	g++ DB_Test.o DB_Operate.o MyDB.o DB_Info.o -o DB_Test -L/usr/lib64/mysql -lmysqlclient
DB_Test.o:DB_Test.cpp
	g++ DB_Test.cpp -c -L/usr/lib64/mysql -lmysqlclient
DB_Operate.o:DB_Operate.cpp
	g++ DB_Operate.cpp -c -L/usr/lib64/mysql -lmysqlclient
MyDB.o:MyDB.cpp
	g++ MyDB.cpp -c -L/usr/lib64/mysql -lmysqlclient
DB_Info.o:DB_Info.cpp
	g++ DB_Info.cpp -c -L/usr/lib64/mysql -lmysqlclient

clean:
	rm DB_Test DB_Test.o DB_Operate.o MyDB.o DB_Info.o 
