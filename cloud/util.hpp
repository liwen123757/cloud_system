#ifndef __MY__UTIL__
#define __MY__UTIL__
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <experimental/filesystem>
#include <memory>
#include <stdio.h>
#include "bundle.h"
#include "jsoncpp/json/json.h"

namespace lw
{
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    public:
        FileUtil(const std::string filename) : _filename(filename) {}
        bool Remove()
        {
            if(Exist()==false) return true;
            remove(_filename.c_str());
            return true;
        }
        int64_t FileSize()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file size failed" << std::endl;
                return -1;
            }
            return st.st_size;
        }
        time_t LastMTime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file lastmtime failed" << std::endl;
                return -1;
            }
            return st.st_mtime;
        }
        time_t LastATime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file lastatime failed" << std::endl;
                return -1;
            }
            return st.st_atime;
        }
        std::string FileName()
        {
            int pos = _filename.find_last_of("/");
            if (pos == std::string::npos)
            {
                return _filename;
            }
            return _filename.substr(pos + 1);
        }
        bool GetPostLen(std::string *body, size_t pos, size_t len)
        {
            size_t fsize = this->FileSize();
            if (pos + len > fsize) // 判断要截取的大小是否大于文件
            {
                std::cout << "get file len error\n";
                return false;
            }

            std::ifstream ifs;
            ifs.open(_filename, std::ios::binary);
            if (ifs.is_open() == false) // 判断文件是否打开成功
            {
                std::cout << "read file open failed\n";
                return false;
            }
            ifs.seekg(pos,std::ios::beg);
            body->resize(len);
            ifs.read(&(*body)[0], len);
            if (ifs.good() == false)
            {
                std::cout << "get file conten failed\n";
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }
        bool GetContent(std::string *body)
        {
            size_t fsize = this->FileSize();
            GetPostLen(body, 0, fsize);
        }
        bool SetContent(const std::string &body)
        {
            std::ofstream ofs;
            
            std::cout << "_filename: "  << _filename << std::endl;
            ofs.open(_filename, std::ios::binary);
            if (ofs.is_open() == false)
            {
                std::cout << "write open file failed\n";
                return false;
            }
            ofs.write(&body[0], body.size());
            if (ofs.good() == false)
            {
                std::cout << "write file content failed\n";
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }
        bool Compress(const std::string &packname)
        {
            // 1.获取文件数据
            std::string body;
            if (GetContent(&body) == false)
            {
                std::cout << "compress get file content failed\n";
                return false;
            }
            // 2.将获取到的数据进行压缩
            std::string packed = bundle::pack(bundle::LZIP, body);
            // 3.将压缩的数据放入压缩包
            FileUtil fu(packname);
            if (fu.SetContent(packed) == false)
            {
                std::cout << "compress write packeed data failed!\n";
                return false;
            }
            return true;
        }
        bool UnCompress(const std::string &filename)
        {
            // 将当前压缩包的数据取出来
            std::string body;
            if (GetContent(&body) == false)
            {
                std::cout << "uncompress get file content failed\n";
                return false;
            }
            // 将数据进行解压缩
            std::string unpacked = bundle::unpack(body);
            // 将解压缩后的数据写入新文件
            FileUtil fu(filename);
            if (fu.SetContent(unpacked) == false)
            {
                std::cout << "uncompress write unpacked data failed!\n";
                return false;
            }
            return true;
        }
        bool Exist()
        {
            return fs::exists(_filename);
        }
        bool CreateDirectory()
        {
            if (Exist())
                return true;
            return fs::create_directories(_filename);
        }
        bool ScanDirector(std::vector<std::string> *array)
        {
            for (auto &p : fs::directory_iterator(_filename))
            {
                if (fs::is_directory(p) == true)
                {
                    continue;
                }
                // relative_path带有路径的文件名，相对路径
                array->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }

    private:
        std::string _filename;
    };
    class JsonUtil
    {
    public:
        static bool Serialize(const Json::Value &root, std::string *str)
        {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream ss;
            sw->write(root, &ss);
            *str = ss.str();
            return true;
        }
        static bool UnSerialize(Json::Value *root, std::string &str)
        {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            std::string err;
            bool ret=cr->parse(str.c_str(),str.c_str()+str.size(),root,&err);
            if(ret==false)
            {
                std::cout<<"parse error: "<<err<<std::endl;
                return false;
            }
            return true;
        }
    };
}

#endif