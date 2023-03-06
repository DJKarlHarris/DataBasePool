main : MysqlConn.cpp main.cpp ConnectionPool.cpp
	g++ -g MysqlConn.cpp main.cpp ConnectionPool.cpp -L/usr/local/lib/ -lmysqlclient -l:libjsoncpp.a -o main




clean:
	rm *.o main