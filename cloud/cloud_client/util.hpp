#ifndef __MY__UTIL__
#define __MY__UTIL__
#include <iostream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <experimental/filesystem>
#include <memory>
#include <stdio.h>
#include <filesystem>


namespace lw
{
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    public:
        FileUtil(const std::string filename) : _filename(filename) {}
        bool Remove()
        {
            if (Exist() == false) return true;
            remove(_filename.c_str());
            return true;
        }
        size_t FileSize()
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
            int64_t pos = _filename.find_last_of("\\");
            if (pos == std::string::npos)
            {
                return _filename;
            }
            return _filename.substr(pos + 1);
        }
        bool GetPostLen(std::string* body, size_t pos, size_t len)
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
            ifs.seekg(pos, std::ios::beg);
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
        bool GetContent(std::string* body)
        {
            size_t fsize = this->FileSize();
            GetPostLen(body, 0, fsize);
            return true;
        }
        bool SetContent(const std::string& body)
        {
            std::ofstream ofs;
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
        bool ScanDirector(std::vector<std::string>* array)
        {
            CreateDirectory();
            for (auto& p : fs::directory_iterator(_filename))
            {
                if (fs::is_directory(p) == true)
                {
                    continue;
                }
                
                array->push_back(fs::path(p).relative_path().string()); //relative_path带有路径的文件名
            }
            return true;
        }

    private:
        std::string _filename;
    };
}

#endif