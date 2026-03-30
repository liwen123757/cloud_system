#ifndef __M_SERVER_H__
#define __M_SERVER_H__
#include "config.hpp"
#include "util.hpp"
#include "httplib.h"
#include "data.hpp"

extern lwc::DataManager *_data;

namespace lwc
{
    //HTTP服务器
    //处理文件上传（POST/UPLOAD）
    //显示文件列表（GET/listenshow或GET/）
    //处理文件下载（GET/download/xxx）
    //支持断点续传（Range请求）
    class Server
    {
    public:
        Server(){
            //获取唯一的config实例
            Config* config=Config::GetInstance();
            //从配置文件里读取服务器IP地址
            _server_ip=config->GetServerIp();
            _server_port=config->GetServerPort();
            _download_prefix=config->GetDownLoadPrefix();
        }
        bool RunModule()
        {
            //注册Post /upload处理函数
            _server.Post("/upload",UpLoad);
            //注册GET /listshow处理函数
            _server.Get("/listshow", ListShow);
            //注册GET / 处理函数(根路径)
            _server.Get("/", ListShow);
            std::string download_url = _download_prefix + "(.*)";
            _server.Get(download_url, DownLoad);

            //监听所有网络接口
            _server.listen("0.0.0.0", _server_port);
            return true;
        }
    private:
        int _server_port;
        std::string _server_ip;
        std::string _download_prefix;

        //httplib::Server 对象-这是HTTP服务器本身
        httplib::Server _server;
        static void UpLoad(const httplib::Request &req, httplib::Response &rsp)
        {
            //检查请求中是否有文件
            auto ret=req.form.has_file("file");
            if(ret==false)
            {
                std::cout<<"上传请求中没有文件";
                //没有文件返回400错误,Bad Request请求格式错误
                rsp.status=400;
                return ;
            }
            //从请求中获取上传的文件
            const auto &file=req.form.get_file("file");
            //获取文件备份目录
            const std::string back_dir=Config::GetInstance()->GetBackDir();
            //构造真实的文件路径
            std::string realpath=back_dir+FileUtil(file.filename).Filename();

            //将上传的文件内容写入磁盘
            FileUtil fu(realpath);
            fu.SentContent(file.content);

            //构造备份信息
            BackupInfo info;
            // NewBackupInfo()会：
            // - 获取文件大小、修改时间等
            // - 根据配置生成压缩包路径
            // - 根据配置生成下载URL
            info.NewBackupInfo(realpath);
            _data->Insert(info);
            return ;

        }

        static std::string TimeToStr(time_t t)
        {
            std::string time=std::ctime(&t);
            return time;
        }

        //显示文件列表页面
        static void ListShow(const httplib::Request &req, httplib::Response &rsp)
        {
            std::vector<BackupInfo> array;
            _data->GetAll(&array);

            //创建HTML页面内容
            std::stringstream ss;
            ss<<"<html><head><title>文件下载</title></head>";
            ss << "<body><h1>文件下载</h1><table>";
            //为每个备份数据生成一个HTML表格
            for(auto &ch:array)
            {
                //每一行包括文件名（可点击下载），文件修改时间，文件大小
                ss<<"<tr>";
                //获取不带路径的文件名
                std::string filename=FileUtil(ch.real_path).Filename();
                ss<<"<td><a href='" << ch.url << "'>" << filename << "</a></td>";
                ss<<"<td align='right'>" <<TimeToStr(ch.mtime)<<"</td>";
                ss<<"<td align='right'>"<<ch.fsize/1024<<"k</td>";
                ss<<"</tr>";
            }
            ss << "</table></body></html>";
            //赋给响应体
            rsp.body=ss.str();
            //设置响应头
            rsp.set_header("Content-Type","text/html");

            rsp.status=200;
            return ;


        }

        //用于支持断点续传
        static std::string GetETag(const BackupInfo &info)
        {
            // etg : filename-fsize-mtime
            FileUtil fu(info.real_path);
            std::string str = fu.Filename();
            str += "-";
            str += std::to_string(info.fsize);
            str += "-";
            str += std::to_string(info.mtime);
            return str;
        }

        static void DownLoad(const httplib::Request &req, httplib::Response &rsp)
        {
            //获取客户端的请求路径,从数据管理模块查询该文件的信息
            BackupInfo info;
            _data->GetOneByUrl(req.path,&info);

            if(info.pack_flag==true)
            {
                //如果压缩标志为真则解压缩数据
                FileUtil fu(info.pack_path);
                fu.uncompress(info.real_path);
                fu.Remove();

                //更新备份信息
                info.pack_flag=false;
                _data->Update(info);
            }

            //读取文件内容到响应体
            FileUtil fu(info.real_path);
            fu.GetContent(rsp.body);

            std::string older_etag;
            bool retrans=false;//判断是否允许断点续传
            if(req.has_header("if-range")){
                older_etag=req.get_header_value("If-Range");

                std::string current_etag=GetETag(info);

                if(older_etag==current_etag){
                    retrans=true;
                }
            }

            if (retrans == false) {
                // 正常下载（不是断点续传）
                
                // 告诉客户端这个服务器支持Range请求（断点续传）
                rsp.set_header("Accept-Ranges", "bytes");
                
                // 设置ETag，让客户端下次可以验证文件是否修改
                rsp.set_header("ETag", GetETag(info));
                
                // 设置Content-Type为二进制流
                // 这样浏览器会直接下载，而不是在页面中显示
                rsp.set_header("Content-Type", "application/octet-stream");
                
                // HTTP状态码200表示成功
                rsp.status = 200;
            }
            else{
                //httplib内部实现了对于区间请求也就是断点续传请求的处理
                //只需要我们用户将该文件按所有数据读取到rsp.body中，它内部会自动根据请求
                //区间，从body中取出指定区间数据进程响应。
                fu.GetContent(rsp.body);
                rsp.set_header("Accept-Ranges","bytes");
                rsp.set_header("ETag",GetETag(info));
                rsp.status=206;//区间请求响应的是206
            }
            
        }

    };
}

#endif