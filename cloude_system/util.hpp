#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <ostream>


namespace lwc{
   namespace fs=std::experimental::filesystem; 
    class FileUtil{
    public:
        FileUtil(std::string filename):_filename(filename){}
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

        bool SetContent(const std::string &body)
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
        bool GetPostLen(std::string *body,size_t pos,size_t len)
        {
            size_t fsize=FileSize();
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
}

#endif