#include "DB_Operate.cpp"
#include <iostream>
#include <string>
using namespace std;

int main()
{
	int Type_num;
	string username, password;
	string userid, virtualpath, md5;
	string statu;
//	freopen("output.txt","w",stdout);
	DB_Operate DB_OPT;
	while (1)
	{
		cout << "0.显示所有的用户信息" << endl;
		cout << "1.加入一条用户信息" << endl;
		cout << "2.查询一条用户信息" << endl;
		cout << "3.显示所有的文件信息" << endl;
		cout << "4.插入一条文件信息" << endl;
		cout << "5.显示所有的文件状态信息" << endl;
		cout << "6.查询一个文件的状态" << endl;
		cout << "7.修改一个文件的状态" << endl;
		cout << "8.添加一个文件的状态" << endl;
		cout << "9.查询一个文件的md5" << endl;
		cout << "10.查询一个目录下的所有文件" << endl;
		cout << "11.删除一个文件或是一个文件夹" << endl;
		cin >> Type_num;
		if (Type_num == 0)
		{
			DB_OPT.Show_User();
		}
		else if (Type_num == 1)
		{
			cout << "请输入用户名和密码" << endl;
//			cin >> username >> password;
//			cout << username << endl;
			cout << password << endl;
			DB_OPT.Insert_User(username,password); 
		}
		else if (Type_num == 2)
		{
			cout << "请输入用户名和密码" << endl;
			cin >> username >> password;
			DB_OPT.Check_User(username, password);
		}
		else if (Type_num == 3)
		{
			DB_OPT.Show_File();
		}
		else if (Type_num == 4)
		{
			cout << "请输入用户id、用户虚拟路径和md5" << endl;
			cin >> userid >> virtualpath >> md5;
			DB_OPT.Insert_File_Info(userid, virtualpath, md5);
		}
		else if (Type_num == 5)
		{
			DB_OPT.Show_Status();
		}
		else if (Type_num == 6)
		{
			cout << "请输入md5号" << endl;
			cin >> md5;
			DB_OPT.Query_Md5_Statu(md5);
		}
		else if (Type_num == 7)
		{
			cout << "请输入md5号和文件状态" << endl;
			cin >> md5 >> statu;
			DB_OPT.Update_Md5_Statu(md5, statu);
		}
		else if (Type_num == 8)
		{
			cout << "请输入md5号和文件状态" << endl;
			cin >> md5 >> statu;
			DB_OPT.Insert_Md5_Statu(md5, statu);
		}
		else if (Type_num == 9)
		{
			cout << "请输入用户id和用户虚拟路径" << endl;
			cin >> userid >> virtualpath;
			DB_OPT.Query_Md5(userid, virtualpath);
		}
		else if (Type_num == 10)
		{
			cout << "请输入用户id和用户虚拟路径" << endl;
//			cout << "Query_File_List" << endl;
			cin >> userid >> virtualpath;
			DB_OPT.Query_File_List(userid, virtualpath);
		}
		else if (Type_num == 11)
		{
			cout << "请输入用户id和用户虚拟路径" << endl;
			cin >> userid >> virtualpath;
			DB_OPT.Delete_File_Info(userid, virtualpath);
		}
	}
//	DB_OPT.Check_User("123","456"); 
//	DB_OPT.Query_File_List("1", "\\a\\");
//	DB_OPT.Query_Md5_Statu("MD555555");
//	DB_OPT.Update_Md5_Statu("MD555555", "1");
//	DB_OPT.Insert_Md5_Statu("MD444444", "2");
//	DB_OPT.Query_Md5("1", "\\a");
//	DB_OPT.Insert_File_Info("1", "\\b", "MD333333");
//	DB_OPT.Delete_File_Info("1", "\\a");  
	return 0;
}
