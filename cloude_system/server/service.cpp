#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
#include <thread>

lwc::DataManager *_data = nullptr;
std::mutex lwc::Config::_mutex;

int main()
{
    lwc::Config *config = lwc::Config::GetInstance();
    (void)config;
    _data = new lwc::DataManager();

    lwc::Server srv;
    lwc::HotManager hot;
    std::thread thread_srv(&lwc::Server::RunModule, &srv);
    std::thread thread_hot(&lwc::HotManager::RunModule, &hot);

    thread_srv.join();
    thread_hot.join();
    return 0;
}
