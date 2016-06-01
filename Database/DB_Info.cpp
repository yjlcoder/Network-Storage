#include <iostream>
#include <string>
#include "DB_Info.h"
using namespace std;

File_Info::File_Info(string Init_User_Id = "", string Init_Virtual_Path = "", string Init_Md5 = "")
{
	User_Id = Init_User_Id;
	Virtual_Path = Init_Virtual_Path;
	Md5 = Init_Md5;
}

File_Info::~File_Info()
{
}

User_Info::User_Info(string Init_User_Id = "", string Init_Username = "", string Init_Password = "")
{
	User_Id = Init_User_Id;
	Username = Init_Username;
	Password = Init_Password;
}

Status_Info::Status_Info(string Init_Md5 = "", string Init_Status = "")
{
	Md5 = Init_Md5;
	Status = Init_Status;
}

Status_Info::~Status_Info()
{
}
