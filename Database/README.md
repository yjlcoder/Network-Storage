## The Database of cloud driver

### Database platform - MariaDB 

### Linux Distribution: RHEL 7.0

DB_Info.cpp和DB_Info.h里面存储的是文件类型信息（并没有用到。。。。）

MyDB.cpp和MyDB.cpp里面存储的是数据库操作的接口

DB_Operate.cpp和DB_Operate.h里面存储的是此次网盘需要用到的数据库操作的接口

DB_Test.cpp和DB_Test.h是一个测试文件，用于测试此次要用到的数据库接口操作


输入make，编译测试文件DB_Test
输入./DB_Test，运行测试文件进行测试
