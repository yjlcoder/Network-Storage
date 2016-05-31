#include "DB_Operate.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include<mysql/mysql.h> 
using namespace std;

DB_Operate::DB_Operate()
{
	db.initDB("localhost", "root", "root123", "driver"); 
}

bool DB_Operate::Show_User()
{
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from user", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		for (j = i; j < i+3; j ++)
			cout << SQL_ans[j] << " ";
		cout << endl;
	}
}
bool DB_Operate::Show_File()
{
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from file", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		for (j = i; j < i+3; j ++)
			cout << SQL_ans[j] << " ";
		cout << endl;
	}
}

//筛选出在当前目录下的文件列表
vector<string> DB_Operate::Find_File_List(vector<string> s, string aim_s)
{
	int i, aim_len = aim_s.size(), pos, s_len;
//	cout << "aim_s_len = " << aim_len << endl;
	vector<string> ans;
	string ans_s;
	ans.clear();
	if (aim_s[aim_len-1] != '/')
	{
		cout << "your directory is illegal" << endl;
		return ans;
	}
	for (i = 0; i < s.size(); i ++)
		if (s[i].size() > aim_len)
		{
			s_len = s[i].size();
			pos = s[i].find("/", aim_len);
//			cout << s[i] << endl;
//			cout << pos << endl;
			if (pos == s[i].npos)
			{
				ans_s = s[i].substr(aim_len, s_len-aim_len);
//				cout << ans_s << endl;
				ans.push_back(ans_s);
			}
			else if (pos == s[i].size()-1)
			{
				ans_s = s[i].substr(aim_len, s_len-aim_len);
//				cout << ans_s << endl;
				ans.push_back(ans_s);
			}
		}
//	cout << ans.size() << endl;
	return ans;
};

bool DB_Operate::Show_Status()
{
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from status", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 2)
	{
		for (j = i; j < i+2; j ++)
			cout << SQL_ans[j] << " ";
		cout << endl;
	}
}

//增加一个用户，如果username已经存在则返回false，否则添加用户并返回true
bool DB_Operate::Insert_User(string Username, string Password)
{
	vector<string> SQL_ans;
	string DB_Command;
	cout << "检查Username是否存在" << endl;
	DB_Command = "SELECT * FROM user WHERE username = \"" + Username + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	if (SQL_ans.size() > 0)
	{	
		cout << "该username已经存在" << endl;
		return false;
	}
	cout << "增加个用户到数据库当中" << endl;
	DB_Command = "INSERT INTO user(username, password) VALUES(\"" + Username + "\", \"" + Password + "\")";
	cout << DB_Command << endl;
	SQL_ans.clear(); 
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		cout << "添加用户信息成功" << endl;
		return true;
	}
	else
	{
		cout << "添加用户信息失败" << endl;
		return false;
	}
}

//查询一个用户的账号密码是否正确
//如果正确则返回该用户的相应id，否则返回-1
int DB_Operate::Check_User(string Username, string Password)
{
	int ans;
	stringstream ss;
	vector<string> SQL_ans;
	string DB_Command;
	cout << "检查数据库中有没有该Username和Password" << endl;
	DB_Command = "SELECT id FROM user WHERE username = \"" + Username + "\" AND password = \"" + Password + "\""; 
	cout << DB_Command << endl;
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	cout << "得到该用户的id" << endl;
	if (SQL_ans.size() == 0)
	{
//		cout << -1 << endl;
		return -1;
	}
	else
	{
//		cout << SQL_ans[0] << endl;
		ss << SQL_ans[0];
		ss >> ans;
		return ans;
	}
}

//查询文件列表：根据用户uid，当前路径，获取当前路径下文件列表
vector<string> DB_Operate::Query_File_List(string User_Id, string Virtual_Path)
{
	vector<string> ans;
	vector<string> SQL_ans;
	string len;
	string DB_Command;
	stringstream ss;
	cout << "查询出文件列表" << endl;
	ss << Virtual_Path.size();
	ss >> len;
//	cout << len << endl;
//	cout << Virtual_Path << endl;
	if (Virtual_Path[Virtual_Path.size()-1] != '/')
		return ans;
	DB_Command = "SELECT virtualpath FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	ans = Find_File_List(SQL_ans, Virtual_Path);
	cout << "得到文件目录" << endl;
	for (int i = 0; i < ans.size(); i ++)
		cout << ans[i] << endl;
	return ans;
}

//查询MD5的状态：根据MD5信息，查询所对应文件的状态（已有、正在上传、没有），没有该MD5则返回-1 
int DB_Operate::Query_Md5_Statu(string Md5)
{
	vector<string> SQL_ans;
	int ans;
	stringstream ss;
	cout << "查询MD5的状态" << endl;
	string DB_Command;
	DB_Command = "SELECT status FROM status WHERE md5 = \"" + Md5 + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	cout << "该MD5的状态为：" << endl;
	if (SQL_ans.size() == 0)
	{
		cout << -1 << endl;
		return -1;
	}
	else 
	{
		cout << SQL_ans[0] << endl;
		ss << SQL_ans[0];
		ss >> ans;
		return ans;
	}
}

//修改MD5的状态：根据MD5信息，和要更改的状态，修改数据库中相应MD5的状态
//修改成功则返回true（已经存在该MD5），失败则返回false（不存在该MD5） 
bool DB_Operate::Update_Md5_Statu(string Md5, string statu)
{
	vector<string> SQL_ans;
	cout << "修改MD5的状态" << endl;
	string DB_Command;
	DB_Command = "UPDATE status SET status = " + statu + " WHERE md5 = \"" + Md5 + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		cout << "修改状态成功" << endl;
		return true;
	}
	else
	{
		cout << "修改状态失败" << endl;
		return false; 
	}
} 

//添加MD5的状态：根据MD5信息，和要添加的状态，增加一条MD5的状态信息
//添加成功则返回true（不存在该MD5），失败则返回false（已经存在该MD5） 
bool DB_Operate::Insert_Md5_Statu(string Md5, string statu = NULL)
{
	cout << "添加MD5的状态" << endl;
	string DB_Command;
	vector<string> SQL_ans;
	DB_Command = "INSERT INTO status(md5, status) VALUES(\"" + Md5 + "\", " + statu + ")"; 
	cout << DB_Command << endl;
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		cout << "添加状态成功" << endl;
		return true;
	}
	else 
	{
		cout << "添加状态失败" << endl;
		return false;
	}
}

//查询文件的MD5号：根据用户uid，文件虚拟路径，获得文件MD5号，如果错误则返回NULL 
string DB_Operate::Query_Md5(string User_Id, string Virtual_Path)
{
	vector<string> SQL_ans;
	cout << "查询文件的MD5号" << endl;
	string DB_Command;
	DB_Command = "SELECT md5 FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	if (SQL_ans.size() > 0)
	{
		cout << "该文件的MD号为：" << endl;
		cout << SQL_ans[0] << endl;
		return SQL_ans[0];
	}
	else
	{
		cout << "查询文件MD号失败，返回NULL" << endl;
		cout << "NULL" << endl;
		return "NULL";
	}
}

//插入文件：根据用户uid，文件虚拟路径，MD5, 在数据库中插入一条记录
//成功返回true，失败返回false 
bool DB_Operate::Insert_File_Info(string User_Id, string Virtual_Path, string Md5 = "NULL")
{
	cout << "插入一条文件记录" << endl;
	string DB_Command;
	vector<string> ans;
	if (Md5 == "NULL")	
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) VALUES(" + User_Id + ", \"" + Virtual_Path + "\", " + Md5 + ")";
	else DB_Command = "INSERT INTO file(userid, virtualpath, md5) VALUES(" + User_Id + ", \"" + Virtual_Path + "\", \"" + Md5 + "\")";
	cout << DB_Command << endl;
	ans.clear();
	if (db.exeSQL(DB_Command, ans) == false)
	{
		cout << "插入文件记录失败" << endl;
		return false;
	}
	else 
	{
		cout << "插入文件记录成功" << endl;
		return true;
	}
}

//删除文件/文件夹：根据用户uid，文件虚拟路径，在数据库中删除一条/一系列记录
//文件/文件夹在数据库中不存在，则返回false,否则，删除记录，并返回true
bool DB_Operate::Delete_File_Info(string User_Id, string Virtual_Path)
{
	cout << "删除文件/文件夹" << endl;
	string DB_Command, len;
	stringstream ss;
	vector<string> SQL_ans;
	ss << Virtual_Path.size();
	ss >> len;
	if (Virtual_Path[Virtual_Path.size()-1] == '/')
		DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	else DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	cout << DB_Command << endl;
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		cout << "删除文件或文件夹成功" << endl;
		return true;
	}
	else
	{
		cout << "删除文件或文件夹失败" << endl;
		return false;
	}
} 
