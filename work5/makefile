all:	test5-client test5-server

test5-client:	test5-client.cpp RSA_AES.o
	g++ -o test5-client test5-client.cpp RSA_AES.o -L/usr/local/bin -lcrypto

test5-server:	test5-server.cpp RSA_AES.o sql_mani.o
	g++ -o test5-server test5-server.cpp RSA_AES.o sql_mani.o \
		-L/usr/local/bin -lcrypto \
		-L/usr/lib64/mysql -lmysqlclient

RSA_AES.o:	RSA_AES.cpp RSA_AES.h
	g++ -o RSA_AES.o -c RSA_AES.cpp

sql_mani.o:	sql_mani.cpp sql_mani.h
	g++ -o sql_mani.o -c sql_mani.cpp

clean:
	rm -rf test5-client test5-server
	rm -rf *.o

