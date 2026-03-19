#ifndef __MY__DATA__
#define __MY__DATA__
#include "util.hpp"
#include <unordered_map>
#include <pthread.h>
#include "config.hpp"

namespace lw
{
    typedef struct BackupInfo_t
    {
        bool pack_flag;
        size_t fsize;
        time_t mtime;
        time_t atime;
        std::string real_path;
        std::string pack_path;
        std::string url;
        bool NewBackupInfo(const std::string &realpath)
        {
            FileUtil fu(realpath);
            if (fu.Exist() == false)
            {
                std::cout << "new backupinfo:file not exist\n";
                return false;
            }
            Config *config = Config::GetInstance();
            std::string packsuffix = config->GetPackFileSuffix();
            std::string packdir = config->GetPackDir();
            std::string download_prefix = config->GetDownLoadPrefix();
            //./backdir/a.txt
            this->pack_flag = false;
            this->fsize = fu.FileSize();
            this->mtime = fu.LastMTime();
            this->atime = fu.LastATime();
            this->real_path = realpath;
            //./backeddir/a.txt ->./packdir/a.txt.lz
            this->pack_path = packdir + fu.FileName() + packsuffix; // 压缩包的创建涉及到压缩包的路径和压缩包后缀，所以这里截取真是路径的文件名就可以了
            //./backeddir/a.txt ->/download/a.txt
            this->url = download_prefix + fu.FileName();
            return true;
        }
    } BackupInfo;

    class DataManager
    {
    public:
        DataManager()
        {
            _backup_file = Config::GetInstance()->GetBackupFile();
            pthread_rwlock_init(&_rwlock, nullptr);
            InitLoad();
        }
        ~DataManager()
        {
            pthread_rwlock_destroy(&_rwlock);
        }
        bool Insert(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        bool Update(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        bool GetOneByUrl(const std::string &url, BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            // 因为url是key值，所以直接使用find
            auto it = _table.find(url);
            if (it == _table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool GetOneByRealPath(const std::string &realpath, BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                if (it->second.real_path == realpath)
                {
                    *info = it->second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }
        bool GetAll(std::vector<BackupInfo> *array)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                array->push_back(it->second);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool Storage()
        {
            // 获取所有数据
            std::vector<BackupInfo> arry;
            this->GetAll(&arry);
            // 添加到json::value中
            Json::Value root;
            for (auto ch : arry)
            {
                Json::Value item;
                item["pack_flag"] = ch.pack_flag;
                item["fsize"] = (Json::Int64)ch.fsize;
                item["mtime"] = (Json::Int64)ch.mtime;
                item["atime"] = (Json::Int64)ch.atime;
                item["real_path"] = ch.real_path;
                item["pack_path"] = ch.pack_path;
                item["url"] = ch.url;
                root.append(item); // 添加数组元素
            }
            // 对数据进行序列化
            std::string body;
            JsonUtil::Serialize(root, &body);
            // 4.写文件
            FileUtil fu(_backup_file);
            fu.SetContent(body);
            return true;
        }
        bool InitLoad()
        {
            // 1.获取文件的所有数据
            FileUtil fu(_backup_file);
            if (fu.Exist() == false)
            {
                return false;
            }
            std::string body;
            fu.GetContent(&body);
            // 2.将数据反序列化
            Json::Value root;
            JsonUtil::UnSerialize(&root, body);
            // 3.将反序列化的Json::value中的数据添加到table中
            for (auto ch : root)
            {
                BackupInfo info;
                info.pack_flag == ch["pack_flag"].asBool();
                info.fsize = ch["fsize"].asInt64();
                info.mtime = ch["mtime"].asInt64();
                info.atime = ch["atime"].asInt64();
                info.real_path = ch["real_path"].asString();
                info.pack_path = ch["pack_path"].asString();
                info.url = ch["url"].asString();
                Insert(info);
            }
            return true;
        }

    private:
        pthread_rwlock_t _rwlock; // 读写锁
        std::string _backup_file;
        std::unordered_map<std::string, BackupInfo> _table;
        
    };
}
#endif