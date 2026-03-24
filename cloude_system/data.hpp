#ifndef __M_DATA_H__
#define __M_DATA_H__
#include <iostream>
#include "util.hpp"
#include "config.hpp"
#include <pthread.h>
#include <unordered_map>
#include <vector>
namespace lwc
{
    //BackupInfo 结构体--备份信息 存储一个备份文件的全部信息

    typedef struct BackupInfo_t
    {
        bool pack_flag;//判断文件是否被压缩
        size_t fsize;
        time_t mtime;
        time_t atime;
        std::string real_path;//原始备份文件的真实路径，当pack_flag=true时，该文件删除
        std::string pack_path;//只有在pack_flag=true时才有意义
        //HTTP下载URL，"/download/document.doc" 服务器靠这个URL查询文件，客户端用这个URL下载文件
        std::string url;

        bool NewBackupInfo(const std::string &realpath)
        {
            //第一步：创建FileUtil对象，检查文件是否存在
            FileUtil fu(realpath);
            if(fu.Exist()==false)
            {
                std::cout<<"new backupinfo:file not exist\n ";
                return false;
            }
            //第二步：从Config中读取配置参数
            Config* config=Config::GetInstance();
            std::string packsuffix=config->GetPackFileSuffix();//获取压缩包文件后缀
            std::string packdir=config->GetPackDir();
            std::string download_prefix=config->GetDownLoadPrefix();

            //第三步初始化BackupInfo的各个字段

            pack_flag=false;
            fsize=fu.FileSize();
            mtime=fu.LastMTime();
            atime=fu.LastATime();

            this->real_path=realpath;

            //第四步构造压缩包文件路径
            pack_path=packdir+fu.Filename()+packsuffix;

            //构造URL
            url=download_prefix+fu.Filename();
            return true;
        }
    }BackupInfo;

    class DataManager
    {
    public:
        DataManager(){
            _manager_file=Config::GetInstance()->GetManagerFile();
            pthread_rwlock_init(&_rwlock,nullptr);
            InitLoad();
        }
        ~DataManager(){
            pthread_rwlock_destroy(&_rwlock);
        }

        bool Insert(const BackupInfo info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url]=info;
            //解锁
            pthread_rwlock_unlock(&_rwlock);
            //存储到硬盘上
            storage();
        }
        //获取所有备份信息
        bool GetAll(std::vector<BackupInfo> *array)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it=_table.begin();
            for(;it!=_table.end();it++)
            {
                array->push_back(it->second);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool GetOneByUrl(std::string &url,BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it=_table.find(url);
            if(it==_table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            *info=it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool GetOneByRealPath(const std::string &realpath, BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it=_table.begin();
            for(;it!=_table.end();it++)
            {
                if(realpath==it->second.real_path)
                {
                    *info=it->second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
                pthread_rwlock_unlock(&_rwlock);
                return false;
            
            }
        }
        bool Update(BackupInfo &info)
        {
            //加锁，操作_table
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url]=info;
            //释放锁
            pthread_rwlock_unlock(&_rwlock);
            //同步到硬盘
            storage();
        }
        bool storage()
        {
            std::vector<BackupInfo> array;
            GetAll(&array);

            //转换为json：value对象
            Json::Value root;
            for(auto ch:array)
            {
                Json::Value item;
                //将每个BackupInfo的每个字段转化为JSON对象
                item["pack_flag"]=ch.pack_flag;
                item["fsize"]=(Json::Int64)ch.fsize;
                item["mtime"]=(Json::Int64)ch.mtime;
                item["atime"]=(Json::Int64)ch.atime;

                item["real_path"]=ch.real_path;
                item["pack_path"]=ch.pack_path;
                item["url"]=ch.url;

                root.append(item);
            }
            //序列化json::value为JSON字符串
            std::string body;
            JsonUtil::Serialize(root,body);
            FileUtil fu(_manager_file);
            fu.SentContent(body);
        }
        bool InitLoad()
        {
            //读取文件
            FileUtil fu(_manager_file);
            if(fu.Exist())
            {
                return false;
            }
            std::string body;
            fu.GetContent(body);
            Json::Value root;
            JsonUtil::UnSerialize(&root,body);
            for(auto ch:root)
            {
                BackupInfo info;
                //从JSON中提取字段，转换为BackupInfo
                info.pack_flag=ch["pack_flag"].asBool();
                info.fsize=ch["fsize"].asInt();
                info.mtime=ch["mtime"].asInt();
                info.atime=ch["atime"].asInt();
                info.real_path=ch["real_path"].asString();
                info.pack_path=ch["pack_path"].asString();
                info.url=ch["url"].asString();
                _table[info.url]=info;
            }
            return true;
           
        }
    private:
        pthread_rwlock_t _rwlock;
        std::string _manager_file;
        std::unordered_map<std::string,BackupInfo> _table;
    };
}
#endif