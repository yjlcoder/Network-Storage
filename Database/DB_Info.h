#include <iostream>
#include <string>
using namespace std;

class File_Info
{
	public:
		string User_Id;
		string Virtual_Path;
		string Md5;
		File_Info(string Init_User_Id, string Init_Virtual_Path, string Init_Md5);
		~File_Info();
};

class User_Info
{
	public:
		string User_Id;
		string Username;
		string Password;
		User_Info(string Init_User_Id, string Init_Username, string Init_Password);
		~User_Info();
};

class Status_Info
{
	public:
		string Md5;
		string Status;
		Status_Info(string Init_Md5, string Init_Status);
		~Status_Info();
};
