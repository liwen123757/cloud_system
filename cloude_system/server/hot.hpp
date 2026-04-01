#ifndef __M__HOT_H__
#define __M__HOT_H__
#include "data.hpp"
#include "config.hpp"
#include <unistd.h>

extern lwc::DataManager *_data;
namespace lwc{
    class HotManager
    {
    public:
        HotManager(){
            Config* config=Config::GetInstance();
            _back_dir=config->GetBackDir();
            _pack_dir=config->GetPackDir();
            _hot_time=config->GetHotTime();
            //如果没有就创建目录
            FileUtil tmp1(_back_dir);
            tmp1.CreateDirctory();

            FileUtil tmp2(_pack_dir);
            tmp2.CreateDirctory();
        }

        bool RunModule()
        {
            while(1)
            {
                FileUtil fu(_back_dir);
                std::vector<std::string> array;
                fu.ScanDirector(&array);
                for(auto &it:array)
                {
                    if(HotJudge(it)==false)
                    {
                        continue;
                    }
                    //获取文件备份信息
                    BackupInfo info;
                    if(_data->GetOneByRealPath(it,&info) == false)
                    {
                        info.NewBackupInfo(it);//设置一个新的备份信息出来
                    }
                    //将非热点文件压缩
                    FileUtil tmp(it);
                    tmp.compress(info.pack_path);
                    //删除源文件，创建文件备份信息
                    tmp.Remove();
                    info.pack_flag=true;
                    _data->Update(info);
                }
                sleep(1);
            }
            return true;
        }

    private:
        std::string _back_dir;
        std::string _pack_dir;
        std::string _arc_suffix;
        int _hot_time;
        bool HotJudge(const std::string &filename)
        {
            FileUtil fu(filename);
            time_t atime=fu.LastATime();
            time_t ctime=time(nullptr);
            if((ctime-atime)>_hot_time)
            {
                return true;
            }
            return false;
        }

    };
}
#endif
