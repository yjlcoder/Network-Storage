#include "DB_Operate.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include<mysql/mysql.h> 
using namespace std;

void DB_Operate::Debug_Out(string s)
{
	cout << s << endl;
}
DB_Operate::DB_Operate()
{
	db.initDB("localhost", "root", "root123", "driver"); 
}

void DB_Operate::Insert_User_Log(string Userid, string Time, string Status)
{
	vector<string> SQL_ans;
	string DB_Command;
	Debug_Out("增加了一条用户登录记录");
	DB_Command = "INSERT INTO log(userid, time, status) VALUES(" + Userid + ", \"" + Time + "\", " + Status + ")";
	Debug_Out(DB_Command);
	SQL_ans.clear(); 
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		Debug_Out("增加一条用户登录记录成功");
	}
	return;
}

bool DB_Operate::Show_User()
{
	Debug_Out("Show_User");
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from user order by id", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		Debug_Out(SQL_ans[i]+" "+SQL_ans[i+1]+" "+SQL_ans[i+2]);
	}
}
bool DB_Operate::Show_Log()
{
	Debug_Out("Show_Log");
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from log order by logid", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 4)
	{
		Debug_Out(SQL_ans[i]+" "+SQL_ans[i+1]+" "+SQL_ans[i+2]+" "+SQL_ans[i+3]);
	}
}
bool DB_Operate::Show_File()
{
	int i, j;
	vector<string> SQL_ans;
	SQL_ans.clear();
	db.exeSQL_SELECT("select * from file order by userid, virtualpath", SQL_ans);
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		Debug_Out(SQL_ans[i]+" "+SQL_ans[i+1]+" "+SQL_ans[i+2]);
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
//		cout << "your directory is illegal" << endl;
		Debug_Out("your directory is illegal");
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
//	cout << "检查Username是否存在" << endl;
	Debug_Out("检查Username是否存在");
	
	DB_Command = "SELECT * FROM user WHERE username = \"" + Username + "\"";
//	cout << DB_Command << endl;
	Debug_Out(DB_Command);
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	if (SQL_ans.size() > 0)
	{	
//		cout << "该username已经存在" << endl;
		Debug_Out("该username已经存在");
		return false;
	}
//	cout << "增加个用户到数据库当中" << endl;
	Debug_Out("增加个用户到数据库当中");
	DB_Command = "INSERT INTO user(username, password) VALUES(\"" + Username + "\", \"" + Password + "\")";
//	cout << DB_Command << endl;
	Debug_Out(DB_Command);
	SQL_ans.clear(); 
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
//		cout << "添加用户信息成功" << endl;
		Debug_Out("添加用户信息成功");
		return true;
	}
	else
	{
//		cout << "添加用户信息失败" << endl;
		Debug_Out("添加用户信息失败");
		return false;
	}
}

//查询一个用户的账号密码是否正确
//如果正确则返回该用户的相应id，否则返回-1
int DB_Operate::Check_User(string Username, string Password)
{
	time_t now_time;
	now_time = time(NULL);
    	char tmp[64]; 
    	strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&now_time) ); 
    	string time;
    	time = tmp;
	cout << time << endl;
	int ans;
	stringstream ss;
	vector<string> SQL_ans;
	string DB_Command;
//	cout << "检查数据库中有没有该Username和Password" << endl;
	Debug_Out("检查数据库中有没有该Username和Password");
	DB_Command = "SELECT id FROM user WHERE username = \"" + Username + "\" AND password = \"" + Password + "\""; 
//	cout << DB_Command << endl;
	Debug_Out(DB_Command);
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
//	cout << "得到该用户的id" << endl;
	Debug_Out("得到该用户的id");
	if (SQL_ans.size() == 0)
	{
//		cout << -1 << endl;
		DB_Command = "SELECT id FROM user WHERE username = \"" + Username + "\""; 
//		cout << DB_Command << endl;
		Debug_Out(DB_Command);
		SQL_ans.clear();
		db.exeSQL_SELECT(DB_Command, SQL_ans);
		if (SQL_ans.size() != 0)
		{
			Insert_User_Log(SQL_ans[0], time, "0");
		}
		return -1;
	}
	else
	{
//		cout << SQL_ans[0] << endl;
		Insert_User_Log(SQL_ans[0], time, "1");
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
//	cout << "查询出文件列表" << endl;
	Debug_Out("查询出文件列表");
	ss << Virtual_Path.size();
	ss >> len;
//	cout << len << endl;
//	cout << Virtual_Path << endl;
	if (Virtual_Path[Virtual_Path.size()-1] != '/')
		return ans;
	DB_Command = "SELECT virtualpath FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	cout << DB_Command << endl;
	Debug_Out(DB_Command);
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	ans = Find_File_List(SQL_ans, Virtual_Path);
//	cout << "得到文件目录" << endl;
	Debug_Out("得到文件目录");
	for (int i = 0; i < ans.size(); i ++)
//		cout << ans[i] << endl;
		Debug_Out(ans[i]);
	return ans;
}

//查询MD5的状态：根据MD5信息，查询所对应文件的状态（已有、正在上传、没有），没有该MD5则返回-1 
int DB_Operate::Query_Md5_Statu(string Md5)
{
	vector<string> SQL_ans;
	int ans;
	stringstream ss;
	Debug_Out("查询MD5的状态");
//	cout << "查询MD5的状态" << endl;
	string DB_Command;
	DB_Command = "SELECT status FROM status WHERE md5 = \"" + Md5 + "\"";
//	cout << DB_Command << endl;
	Debug_Out(DB_Command);
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
//	cout << "该MD5的状态为：" << endl;
	Debug_Out("该MD5的状态为：");
	if (SQL_ans.size() == 0)
	{
//		cout << -1 << endl;
		Debug_Out("-1");
		return -1;
	}
	else 
	{
//		cout << SQL_ans[0] << endl;
		Debug_Out(SQL_ans[0]);
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
	Debug_Out("修改MD5的状态");
	string DB_Command;
	DB_Command = "UPDATE status SET status = " + statu + " WHERE md5 = \"" + Md5 + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		Debug_Out("修改状态成功");
		return true;
	}
	else
	{
		Debug_Out("修改状态失败");
		return false; 
	}
} 

//添加MD5的状态：根据MD5信息，和要添加的状态，增加一条MD5的状态信息
//添加成功则返回true（不存在该MD5），失败则返回false（已经存在该MD5） 
bool DB_Operate::Insert_Md5_Statu(string Md5, string statu = NULL)
{
	Debug_Out("添加MD5的状态");
	string DB_Command;
	vector<string> SQL_ans;
	DB_Command = "INSERT INTO status(md5, status) VALUES(\"" + Md5 + "\", " + statu + ")"; 
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		Debug_Out("添加状态成功");
		return true;
	}
	else 
	{
		Debug_Out("添加状态失败");
		return false;
	}
}

//查询文件的MD5号：根据用户uid，文件虚拟路径，获得文件MD5号，如果错误则返回NULL 
string DB_Operate::Query_Md5(string User_Id, string Virtual_Path)
{
	vector<string> SQL_ans;
	Debug_Out("查询文件的MD5号");
	string DB_Command;
	DB_Command = "SELECT md5 FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	db.exeSQL_SELECT(DB_Command, SQL_ans);
	if (SQL_ans.size() > 0)
	{
		Debug_Out("该文件的MD号为：");
		Debug_Out(SQL_ans[0]);
		return SQL_ans[0];
	}
	else
	{
		Debug_Out("查询文件MD号失败，返回NULL");
		Debug_Out("NULL");
		return "NULL";
	}
}

//插入文件：根据用户uid，文件虚拟路径，MD5, 在数据库中插入一条记录
//成功返回true，失败返回false 
bool DB_Operate::Insert_File_Info(string User_Id, string Virtual_Path, string Md5 = "NULL")
{
	Debug_Out("插入一条文件记录");
	string DB_Command;
	vector<string> ans;
	if (Md5 == "NULL")	
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) VALUES(" + User_Id + ", \"" + Virtual_Path + "\", " + Md5 + ")";
	else DB_Command = "INSERT INTO file(userid, virtualpath, md5) VALUES(" + User_Id + ", \"" + Virtual_Path + "\", \"" + Md5 + "\")";
	Debug_Out(DB_Command);
	ans.clear();
	if (db.exeSQL(DB_Command, ans) == false)
	{
		Debug_Out("插入文件记录失败");
		return false;
	}
	else 
	{
		Debug_Out("插入文件记录成功");
		return true;
	}
}

//删除文件/文件夹：根据用户uid，文件虚拟路径，在数据库中删除一条/一系列记录
//文件/文件夹在数据库中不存在，则返回false,否则，删除记录，并返回true
bool DB_Operate::Delete_File_Info(string User_Id, string Virtual_Path)
{
	Debug_Out("删除文件/文件夹");
	string DB_Command, len;
	stringstream ss;
	vector<string> SQL_ans;
	ss << Virtual_Path.size();
	ss >> len;
	if (Virtual_Path[Virtual_Path.size()-1] == '/')
		DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	else DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == true)
	{
		Debug_Out("删除文件或文件夹成功");
		return true;
	}
	else
	{
		Debug_Out("删除文件或文件夹失败");
		return false;
	}
} 

//复制文件夹或文件，根据用户uid，源文件路径，目标文件路径，复制一系列记录		
//如果文件存在，则复制为filename(i)
//如果复制失败，则返回false
//如果复制成功，则返回true
//一个文件夹下不能同时出现filename和filename/
bool DB_Operate::Copy_File_Info(string User_Id, string Virtual_Path, string Aim_Path)
{
	Debug_Out("复制文件/文件夹");
	stringstream ss;
	string DB_Command, len;
	int i, j;
	int virtual_path_len = Virtual_Path.size();
	int aim_path_len = Aim_Path.size();
	vector<string> SQL_ans;
	vector<File_Info> Copy_File_Info;
	ss.clear();
	ss.str("");
	ss << Virtual_Path.size();
	ss >> len;
	if (Virtual_Path[Virtual_Path.size()-1] == '/')
		DB_Command = "SELECT * FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	else DB_Command = "SELECT * FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL_SELECT(DB_Command, SQL_ans) == false)
		return false;
	if (SQL_ans.size() == 0)
	{
		Debug_Out("该文件/文件夹不存在");
		return false;
	}
	Copy_File_Info.clear();
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		Debug_Out(SQL_ans[i] + " " + SQL_ans[i+1] + " " + SQL_ans[i+2]);
		Copy_File_Info.push_back(File_Info(SQL_ans[i], SQL_ans[i+1], SQL_ans[i+2]));
	}
	for (i = 0; i < Copy_File_Info.size(); i ++)
	{
		Copy_File_Info[i].Virtual_Path.replace(0, virtual_path_len, Aim_Path);	
	}
	if (Copy_File_Info[0].Md5 != "NULL")
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) Values(" + Copy_File_Info[0].User_Id + ", \"" + Copy_File_Info[0].Virtual_Path + "\", \"" + Copy_File_Info[0].Md5 + "\")";
	else
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) Values(" + Copy_File_Info[0].User_Id + ", \"" + Copy_File_Info[0].Virtual_Path + "\", " + Copy_File_Info[0].Md5 + ")";
	for (i = 1; i < Copy_File_Info.size(); i ++)
	{
		if (Copy_File_Info[i].Md5 != "NULL")
			DB_Command += ",(" + Copy_File_Info[i].User_Id + ", \"" + Copy_File_Info[i].Virtual_Path + "\", \"" + Copy_File_Info[i].Md5 + "\")";
		else
			DB_Command += ",(" + Copy_File_Info[i].User_Id + ", \"" + Copy_File_Info[i].Virtual_Path + "\", " + Copy_File_Info[i].Md5 + ")";
	}
	DB_Command += ";";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == false)
		return false;
	return true;
}

//更改文件/文件夹名称：根据用户uid，文件虚拟路径和修改后得文件/文件名路径，在数据库中修改一条/一系列记录
//需要保证修改后的文件名/文件夹名不存在
bool DB_Operate::Update_File_Info(string User_Id, string Virtual_Path, string Aim_Path)
{
	if (Virtual_Path.size() <= 1)
		return false;
	Debug_Out("重命名文件/文件夹");
	stringstream ss;
	string DB_Command, len;
	int i, j;
	int virtual_path_len = Virtual_Path.size();
	int aim_path_len = Aim_Path.size();
	vector<string> SQL_ans;
	vector<File_Info> Copy_File_Info;
	ss.clear();
	ss.str("");
	ss << Virtual_Path.size();
	ss >> len;
	if (Virtual_Path[Virtual_Path.size()-1] == '/')
		DB_Command = "SELECT * FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	else DB_Command = "SELECT * FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL_SELECT(DB_Command, SQL_ans) == false)
		return false;
	if (SQL_ans.size() == 0)
	{
		Debug_Out("该文件/文件夹不存在");
		return false;
	}
	Copy_File_Info.clear();
	for (i = 0; i < SQL_ans.size(); i += 3)
	{
		Debug_Out(SQL_ans[i] + " " + SQL_ans[i+1] + " " + SQL_ans[i+2]);
		Copy_File_Info.push_back(File_Info(SQL_ans[i], SQL_ans[i+1], SQL_ans[i+2]));
	}
	for (i = 0; i < Copy_File_Info.size(); i ++)
	{
		Copy_File_Info[i].Virtual_Path.replace(0, virtual_path_len, Aim_Path);	
	}
	if (Copy_File_Info[0].Md5 != "NULL")
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) Values(" + Copy_File_Info[0].User_Id + ", \"" + Copy_File_Info[0].Virtual_Path + "\", \"" + Copy_File_Info[0].Md5 + "\")";
	else
		DB_Command = "INSERT INTO file(userid, virtualpath, md5) Values(" + Copy_File_Info[0].User_Id + ", \"" + Copy_File_Info[0].Virtual_Path + "\", " + Copy_File_Info[0].Md5 + ")";
	for (i = 1; i < Copy_File_Info.size(); i ++)
	{
		if (Copy_File_Info[i].Md5 != "NULL")
			DB_Command += ",(" + Copy_File_Info[i].User_Id + ", \"" + Copy_File_Info[i].Virtual_Path + "\", \"" + Copy_File_Info[i].Md5 + "\")";
		else
			DB_Command += ",(" + Copy_File_Info[i].User_Id + ", \"" + Copy_File_Info[i].Virtual_Path + "\", " + Copy_File_Info[i].Md5 + ")";
	}
	DB_Command += ";";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == false)
		return false;
//=================删除数据库中原文件/文件夹的信息==========================
	if (Virtual_Path[Virtual_Path.size()-1] == '/')
		DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND left(virtualpath, " + len + ") = \"" + Virtual_Path + "\"";
	else DB_Command = "DELETE FROM file WHERE userid = " + User_Id + " AND virtualpath = \"" + Virtual_Path + "\"";
	Debug_Out(DB_Command);
	SQL_ans.clear();
	if (db.exeSQL(DB_Command, SQL_ans) == false)
		return false;
//=================删除数据库中原文件/文件夹的信息==========================
	return true;
}



