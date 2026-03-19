#include "util.hpp"
#include "data.hpp"
#include "cloud.hpp"
#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR	"./backup/"
int main()
{
	/*lw::FileUtil fu("./");
	std::vector<std::string> arry;
	fu.ScanDirector(&arry);
	lw::DataManager data(BACKUP_FILE);
	for (auto& a : arry)
	{
		data.Insert(a,"sfakshfkhf");
	}*/
	/*lw::DataManager data(BACKUP_FILE);
	std::string str;
	data.GetOneByKey(".\\cloud.cpp", &str);
	std::cout << str << std::endl;*/
	lw::Backup backup(BACKUP_DIR, BACKUP_FILE);
	backup.RunModule();
	return 0;
}