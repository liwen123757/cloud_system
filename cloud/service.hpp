#ifndef __MY__SERVICE__
#define __MY__SERVICE__
#include "data.hpp"
#include "httplib.h"

extern lw::DataManager *_data;

namespace lw
{
    class Service
    {
    public:
        Service()
        {
            Config *config = Config::GetInstance();
            _server_ip = config->GetServerIp();
            _server_port = config->GetServerPort();
            _download_prefix = config->GetDownLoadPrefix();
        }
        bool RunModule()
        {
            _server.Post("/upload", UpLoad);
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _download_prefix + "(.*)";
            _server.Get(download_url, DownLoad), //.*匹配任意一个字符任意次
                _server.listen("0.0.0.0", _server_port);
            return true;
        }

    private:
        int _server_port;
        std::string _server_ip;
        std::string _download_prefix;
        httplib::Server _server;

    private:
        static void UpLoad(const httplib::Request &req, httplib::Response &rsp) // 文件上传请求
        {
            auto ret = req.has_file("file");
            if (ret == false)
            {
                std::cout << "not file upload\n";
                rsp.status = 400; // 请求的资源不存在
                return;
            }
            const auto &file = req.get_file_value("file");
            // file.filename; // 文件名称
            // file.content; // 文件内容
            std::string back_dir = Config::GetInstance()->GetBackDir();           // 文件上传目录
            std::string realpath = back_dir + FileUtil(file.filename).FileName(); // 文件真实路径
            FileUtil fu(realpath);
            fu.SetContent(file.content); // 将数据写入文件中
            BackupInfo info;
            info.NewBackupInfo(realpath); // 组织备份的文件信息
            _data->Insert(info);          // 向数据管理了模块添加备份的文件信息
            return;
        }
        static std::string TimeToStr(time_t t)
        {
            std::string tmp = std::ctime(&t);
            return tmp;
        }
        static void ListShow(const httplib::Request &req, httplib::Response &rsp) // 展示页面获取请求
        {
            // 1.获取所有文件信息
            std::vector<BackupInfo> arry;
            _data->GetAll(&arry);
            // 2.根据所有备份的文件信息，组织HTML文件数据
            std::stringstream ss;
            ss << "<html><head><title>Download</title></head>";
            ss << "<body><h1>Download</h1><table>";
            for (auto &ch : arry)
            {
                ss << "<tr>";
                std::string filename = FileUtil(ch.real_path).FileName();
                ss << "<td><a href ='" << ch.url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << TimeToStr(ch.mtime) << "</td>";
                ss << "<td align='right'>" << ch.fsize / 1024 << "k</td>";
            }
            ss << "</table></body></html>";
            rsp.body = ss.str();
            rsp.set_header("Content-Type", "text/html");
            rsp.status = 200;
            return;
        }
        static std::string GetETag(const BackupInfo &info)
        {
            // etg : filename-fsize-mtime
            FileUtil fu(info.real_path);
            std::string str = fu.FileName();
            str += "-";
            str += std::to_string(info.fsize);
            str += "-";
            str += std::to_string(info.mtime);
            return str;
        }
        static void DownLoad(const httplib::Request &req, httplib::Response &rsp) // 文件下载处理请求
        {
            // 1.获取客户端请求的资源路径path  req.path
            // 2.根据资源路径，获取文件对应的信息
            BackupInfo info;
            _data->GetOneByUrl(req.path, &info);
            // 3.查看该文件是否被压缩，如果压缩了，就先解压缩
            if (info.pack_flag == true)
            {
                FileUtil fu(info.pack_path);
                fu.UnCompress(info.real_path); // 将文件解压到备份目录下
                // 4.删除压缩包，修改备份信息
                fu.Remove();
                info.pack_flag = false;
                _data->Update(info);
            }
            // //5.读取文件数据放入rsp.body中 
            // FileUtil fu(info.real_path);
            // fu.GetContent(&rsp.body);
            // 5.读取文件数据放入rsp.body中
            FileUtil fu(info.real_path);
            fu.GetContent(&rsp.body);
            std::string old_etag;
            bool retrans = false;
            if (req.has_header("if-Range")) // 如果有证明是一个断点续传
            {
                old_etag = req.get_header_value("If-Range");
                // 有IF-Range字段且，这个字段的值与请求文件最新的etag一致则符合断点续传
                if (old_etag == GetETag(info))
                {
                    retrans = true;
                }
            }
            // 如果没有IF-Range字段则是正常下载，或者如果有这个字段，但是他的值与当前文件的Etag不一致，则也必须重新返回全部数据（被修改过了）

            if (retrans == false)
            {
                // 6.设置响应头部字段,ETag .Accept-Ranges:bytes
                rsp.set_header("Accept-Ranges", "bytes");
                rsp.set_header("ETag", GetETag(info));
                rsp.set_header("Content-Type", "application/octet-stream"); // 响应的数据是一个二进制数据流
                rsp.status = 200;
            }
            else{
                //httplib内部实现了对于区间请求也就是断点续传请求的处理
                //只需要我们用户将该文件按所有数据读取到rsp.body中，它内部会自动根据请求
                //区间，从body中取出指定区间数据进程响应。
                fu.GetContent(&rsp.body);
                rsp.set_header("Accept-Ranges","bytes");
                rsp.set_header("ETag",GetETag(info));
                rsp.status=206;//区间请求响应的是206
            }
            // // 6.设置响应头部字段,ETag .Accept-Ranges:bytes
            // rsp.set_header("Accept-Ranges", "bytes");
            // rsp.set_header("ETag", GetETag(info));
            // rsp.set_header("Content-Type", "application/octet-stream"); // 响应的数据是一个二进制数据流
            // rsp.status = 200;
        }
    };

}

#endif
