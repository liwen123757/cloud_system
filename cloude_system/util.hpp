#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <ostream>
#include "jsoncpp/json/json.h"
#include "bundle.h"


namespace lwc{
   namespace fs=std::experimental::filesystem; 
    class FileUtil{
    public:
        FileUtil(std::string filename):_filename(filename){}
        //获取文件上一次修改时间
        time_t LastMTime()
        {
            struct  stat st;
            if(stat(_filename.c_str(),&st)<0)
            {
                std::cout<<"get file lastmtime failed!";
                return -1;
            }
            return st.st_mtime;
            
        }
        //获取文件最后上传时间，用于以此判断是不是长期未访问文件
        time_t LastATime()
        {
            struct stat st;
            if(stat(_filename.c_str(),&st)<0)
            {
                std::cout<<"get file lastatime failed!"<<std::endl;
                return -1;
            }
            return st.st_atime;
        }
        bool Exist()
        {
            return fs::exists(_filename);
        }
        bool CreateDirctory()
        {
            if(Exist())
                return true;
            return fs::create_directories(_filename);
        }
        int64_t FileSize()
        {
            struct stat st;
            if(stat(_filename.c_str(),&st)<0)
            {
                std::cout<<"get file failed"<<std::endl;
            }
            return st.st_size;
        }
        std::string Filename()
        {
            int pos=_filename.find_last_of("/");
            if(pos==std::string::npos)
            {
                return _filename;
            }
            return _filename.substr(pos+1);
        }

        bool SentContent(const std::string &body)
        {
            std::ofstream ofs;
            std::cout<<"_filename: "<<_filename<<std::endl;
            ofs.open(_filename,std::ios::binary);
            if(ofs.is_open()==false)
            {

                std::cout<<"write open file failed!\n";
                return false;
            }
            ofs.write(&body[0],body.size());
            return true;

        }
        bool GetContent(std::string &body)
        {
            GetPostLen(body,0,FileSize());
        }
        bool GetPostLen(std::string &body,size_t pos,size_t len)
        {
            size_t fsize=FileSize();
            if(pos+len>fsize)
            {
                std::cout<<"getpostlen failed\n";
                return false;
            }
            std::ifstream ifs;
            ifs.open(_filename,std::ios::binary);
            if(ifs.is_open()==false)
            {
                std::cout<<"read file failed\n";
                return false;
            }
            ifs.seekg(pos,std::ios::beg);
            body.resize(len);
            ifs.read(&body[0],len);
            if(ifs.good()==false)
            {
                std::cout<<"get fail content failed\n";
                return false;
            }
            ifs.close();
            return true;
        }
        bool ScanDirector(std::vector<std::string> *array)
        {
            for(auto &p:fs::directory_iterator(_filename))
            {
                if(fs::is_directory(p)==true)
                {
                    continue;
                }
                array->push_back(fs::path().relative_path().string());
                std::cout<<"get relative_path success\n";
                return true;
            }
        }

    private:
        std::string _filename;
    };

    class JsonUtil
    {
    public:
        //将Json::Value对象转换为JSON字符串
        static bool Serialize(const Json::Value &root,std::string &str)
        {
            //创建JSON写入器的工厂
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

            std::stringstream ss;
            sw->write(root,&ss);
            str=ss.str();
            return true;
        }

        static bool UnSerialize(Json::Value *root,std::string &str)
        {
            //创建JSON读取器工厂
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            std::string err;
            bool ret=cr->parse(str.c_str(),str.c_str()+str.size(),root,&err);
            if(ret==false)
            {
                std::cout<<"unserialize failed\n";
                return false;
            }
            return true;

        }
    };
}

#endif