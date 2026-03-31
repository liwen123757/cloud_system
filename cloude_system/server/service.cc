#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
#include <mutex>
#include <thread>
// lwc::DataManager* _data;
std::mutex lwc::Config::_mutex;
void FileUtilTest(const std::string &filename)
{
    using namespace lwc;
    /* lwc::FileUtil fu(filename);
    std::cout << fu.FileSize() << std::endl;
    std::cout << fu.LastMTime() << std::endl;
    std::cout << fu.LastATime() << std::endl;
    std::cout << fu.Filename() << std::endl; */
    // lwc::FileUtil fu(filename);
    // std::string body;
    // fu.GetContent(body);
    // lwc::FileUtil nfu("./hello.txt");
    // nfu.SentContent(body);
    Config* config=config->GetInstance();
    _data=new DataManager();
    Server srv;
    srv.RunModule();//阻塞监听
    
    return ;
    //std::string packname = filename + ".lz";
    // lw::FileUtil fu(filename);
    // fu.Compress(packname);
    // lw::FileUtil pfu(packname);
    // pfu.UnCompress("hello.txt");
    // lw::FileUtil fu(filename);
    // fu.CreateDirectory();
    // std::vector<std::string> arry;
    // fu.ScanDirector(&arry);
    // for (auto &arr : arry)
    // {
    //     std::cout << arr << std::endl;
    // }
    //return;
}

lwc::DataManager *_data=nullptr;
void HotTest()
{
    lwc::HotManager hot;
    hot.RunModule();
}
void ServiceTest()
{
    lwc::Server srv;
    srv.RunModule();
}

int main()
{
    lwc::Config* config=lwc::Config::GetInstance();
    _data = new lwc::DataManager();
    std::thread hot_thread(HotTest);
    std::thread service_thread(ServiceTest);
    hot_thread.join();
    service_thread.join();
    return 0;
}