#ifndef __MY__HOT__
#define __MY__HOT__
#include "data.hpp"
#include <unistd.h>

extern lw::DataManager *_data;

namespace lw
{
    class HotManager
    {
    public:
        HotManager()
        {
            Config *config = Config::GetInstance();
            _back_dir = config->GetBackDir();
            _pack_dir = config->GetPackDir();
            _pack_suffix = config->GetPackFileSuffix();
            _hot_time = config->GetHotTime();
            FileUtil tmp1(_back_dir);
            FileUtil tmp2(_pack_dir);
            tmp1.CreateDirectory();
            tmp2.CreateDirectory();

        }
        bool RunModule()
        {
            while (1)
            {
                // 1.遍历备份目录，获取所有文件名称
                FileUtil fu(_back_dir);
                std::vector<std::string> arry;
                fu.ScanDirector(&arry);
                // 2.判断该文件是否是非热点文件
                for (auto &ch : arry)
                {
                    if (HotJudge(ch) == false)
                    {
                        continue; // 热点文件不需要特别处理
                    }
                    // 3.获取文件备份信息
                    BackupInfo bi;
                    if (_data->GetOneByRealPath(ch, &bi) == false)
                    {
                        // 现在有一个文件存在但是没有备份信息
                        bi.NewBackupInfo(ch); // 设置一个新的备份信息出来
                    }
                    // 3.将非热点文件压缩处理
                    FileUtil tmp(ch);
                    tmp.Compress(bi.pack_path);
                    // 4.删除源文件，修改文件备份信息
                    tmp.Remove();
                    bi.pack_flag = true;
                    _data->Update(bi);
                }
                usleep(1000);
            }
            return true;
        }

    private:
        std::string _back_dir;
        std::string _pack_dir;
        std::string _pack_suffix;
        int _hot_time;
        // 非热点文件返回真，热点文件返回假
        bool HotJudge(const std::string &filename)
        {
            FileUtil fu(filename);
            time_t lastatime = fu.LastATime();
            time_t curtime = time(nullptr);
            if ((curtime - lastatime) > _hot_time)
                return true;
            return false;
        }
    };
}

#endif
