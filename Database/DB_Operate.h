#ifndef __DBOperate_H__
#define __DBOperate_H__
#include "DB_Info.h"
#include "MyDB.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class DB_Operate
{
	public:
		//用于输出到日志或者debug
		void Debug_Out(string s);
		//链接数据库 
		DB_Operate();
		//插入一条用户登录记录
		void Insert_User_Log(string Userid, string Time, string Status);
		//输出数据库中的User表
		bool Show_User();
		//输出用户登录记录表Log
		bool Show_Log();
		//输出数据库中的File表
		bool Show_File();
		//筛选出当前目录下的文件列表
		vector<string> Find_File_List(vector<string> s, string aim_s);
		//输出数据库中的Status表
		bool Show_Status();
		//增加一个用户，如果username已经存在则返回false，否则添加用户并返回true
		bool Insert_User(string Username, string Password);
		//查询一个用户的账号密码是否正确
		//如果正确则返回该用户的相应id，否则返回-1
		int Check_User(string Username, string Password);
		//查询文件列表：根据用户uid，当前路径，获取当前路径下文件列表
		vector<string> Query_File_List(string User_Id, string Virtual_Path);
		//查询MD5的状态：根据MD5信息，查询所对应文件的状态（已有、正在上传、没有），没有该MD5则返回-1 
		int Query_Md5_Statu(string Md5); 
		//修改MD5的状态：根据MD5信息，和要更改的状态，修改数据库中相应MD5的状态
		//修改成功则返回true（已经存在该MD5），失败则返回false（不存在该MD5） 
		bool Update_Md5_Statu(string Md5, string statu); 
		//添加MD5的状态：根据MD5信息，和要添加的状态，增加一条MD5的状态信息
		//添加成功则返回true（不存在该MD5），失败则返回false（已经存在该MD5） 
		bool Insert_Md5_Statu(string Md5, string statu);
		//查询文件的MD5号：根据用户uid，文件虚拟路径，获得文件MD5号，如果错误则返回NULL 
		string Query_Md5(string User_Id, string Virtual_Path); 
		//插入文件：根据用户uid，文件虚拟路径，MD5, 在数据库中插入一条记录
		//成功返回true，失败返回false 
		bool Insert_File_Info(string User_Id, string Virtual_Path, string Md5);
		//删除文件/文件夹：根据用户uid，文件虚拟路径，在数据库中删除一条/一系列记录
		//文件/文件夹在数据库中不存在，则返回false,否则，删除记录，并返回true
		bool Delete_File_Info(string User_Id, string Virtual_Path); 
		//更改文件/文件夹名称：根据用户uid，文件虚拟路径和修改后得文件/文件名，在数据库中修改一条/一系列记录
		//如果修改后的文件名/文件夹名已经存在，则操作更改失败，返回false
		//如果修改后的文件名/文件夹名不存在。则操作更改成功，返回true
		bool Update_File_Info(string User_Id, string Virtual_Path, string Aim_Path); 
		//复制文件夹或文件，根据用户uid，源文件路径，目标文件路径，复制一系列记录		
		//如果文件存在，则复制为filename(i)
		//如果复制失败，则返回false
		//如果复制成功，则返回true
		bool Copy_File_Info(string User_Id, string Virtual_Path, string Aim_Path);
	private:
		MyDB db;
};
#endif
