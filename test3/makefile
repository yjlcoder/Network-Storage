all:	test3-client test3-server

test3-client:	test3-client.cpp
	g++ -o test3-client test3-client.cpp -L/usr/local/bin -lcrypto

test3-server:	test3-server.cpp sql_mani.o
	g++ -o test3-server test3-server.cpp sql_mani.o \
		-L/usr/lib64/mysql -lmysqlclient

sql_mani.o:	sql_mani.cpp sql_mani.h
	g++ -o sql_mani.o -c sql_mani.cpp

clean:
	rm -rf test3-client test3-server
	rm -rf *.o

