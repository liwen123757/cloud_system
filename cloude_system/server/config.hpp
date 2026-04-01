#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__
#include <memory>
#include <mutex>
#include <iostream>
#include "util.hpp"
namespace lwc{
    //配置文件的路径
    #define CONFIG_FILE "./cloud.conf"
    class Config{
    public:
        //获得Config对象的唯一途径
        static Config* GetInstance()
        {
            if(_instance==nullptr)
            {
                std::unique_lock<std::mutex> _lock(_mutex);
                if(_instance==nullptr)
                {
                    _instance=new Config();//创建_instance
                }
            }
            return _instance;
        }
        int GetHotTime()
        {
            return _hot_time;
        }
        int GetServerPort()
        {
            return _server_port;
        }
        std::string GetServerIp()
        {
            return _server_ip;
        }
        std::string GetDownLoadPrefix()
        {
            return _download_prefix;
        }
        std::string GetPackFileSuffix()
        {
            return _arc_suffix;
        }
        std::string GetPackDir()
        {
            return _pack_dir;
        }
        std::string GetBackDir()
        {
            return _back_dir;
        }
        std::string GetManagerFile()
        {
            return _manager_file;
        }
        
    private:
        Config()
        {
            //调用readconfigfile读取cloud.conf文件
            ReadConfigFile();
        }
        static Config *_instance;
        static std::mutex _mutex;

    private:
        int _hot_time;
        std::string _server_ip;
        int _server_port;
        std::string _download_prefix;
        std::string _arc_suffix;
        std::string _back_dir;
        std::string _pack_dir;
        std::string _manager_file;//数据信息存放文件
        
        bool ReadConfigFile()
        {
            //读取我们CONFIG_FILE里面的配置
            FileUtil fu(CONFIG_FILE);
            std::string body;
            if(fu.GetContent(body)==false)
            {
                std::cout<<"get content failed\n";
                return false;
            }
            Json::Value root;
            if (JsonUtil::UnSerialize(&root,body) == false)
            {
                return false;
            }
            _hot_time=root["hot_time"].asInt();
            _server_ip=root["server_ip"].asString();
            _server_port=root["server_port"].asInt();
            _download_prefix=root["download_prefix"].asString();
            _arc_suffix=root["arc_suffix"].asString();
            _back_dir=root["back_dir"].asString();
            _pack_dir=root["pack_dir"].asString();
            _manager_file=root["manager_file"].asString();
            return true;
        }
    };
    //静态成员变量类外初始化
    Config* Config::_instance=nullptr;
}

#endif
