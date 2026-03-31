#include "util.hpp"
#include "data.hpp"
#include "client.hpp"
#include "httplib.h"
#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR	"./backup/"
int main()
{
	// using namespace lwc;
	/*lwc::FileUtil fu("./");
	std::vector<std::string> arry;
	fu.ScanDirector(&arry);
	lwc::DataManager data(BACKUP_FILE);
	for (auto& a : arry)
	{
		data.Insert(a,"sfakshfkhf");
	}*/
	/*lw::DataManager data(BACKUP_FILE);
	std::string str;
	data.GetOneByKey(".\\cloud.cpp", &str);
	std::cout << str << std::endl;*/
    
    lwc::Backup backup(BACKUP_DIR,BACKUP_FILE);
    backup.RunModule();
	return 0;
}