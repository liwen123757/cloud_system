#ifndef __M_CLIENT_H__
#define __M_CLIENT_H__
#include "data.hpp"
#include <vector>
#include "httplib.h"
namespace lwc 
{
#define SERVER_ADDR "43.139.140.92"
#define SERVER_PORT 9090
	class Backup 
	{
	public:
		Backup(const std::string backdir,const std::string back_file)
			:_back_dir(backdir) {
			_data = new DataManager(back_file);
		}
		std::string GetFileIdentifier(const std::string& filename)//创建文件的唯一标识
		{
			FileUtil fu(filename);
			std::stringstream ss;
			ss << fu.FileName() << "-" << fu.FileSize() << "-" << fu.LastMTime();
			return ss.str();
		}
		/* bool Upload(const std::string& filename)
		{
			//1. 获取文件数据
			FileUtil fu(filename);
			std::string body;
			fu.GetContent(body);

			// 准备上传数据
			httplib::UploadFormDataItems items = {
				{"file", body, fu.FileName(), "application/octet-stream"}
			};

			httplib::Client client(SERVER_ADDR, SERVER_PORT);
			auto res = client.Post("/upload", items);
			if (!res || res->status != 200)
			{
				return false;
			}
			return true;
			
		} */
		 bool Upload(const std::string& filename)
		{
			//1.获取文件数据
			FileUtil fu(filename);
			std::string body;
			fu.GetContent(body);
			//2.搭建http客户端上传文件数据
			httplib::Client client(SERVER_ADDR, SERVER_PORT);
			httplib::UploadFormData item;
			item.content = body;
			item.filename = fu.FileName();
			item.name = "file";
			item.content_type = "application/octet-stream";//octet-stream二进制流数据
			httplib::UploadFormDataItems items;
			items.push_back(item);
			auto res = client.Post("/upload", items);
			if (!res || res->status != 200)
			{
				return false;
			}
			return true;
		}
		bool IsNeedUpload(const std::string &filename)
		{
			//需要上传的文件的判断条件，文件是新增的，不是新增但是修改过的
			//文件是新增的：看一下有没有历史信息
			//不是新增但是修改过的：有历史信息但是历史的唯一标识与当前最新的唯一标识不一致
			std::string id;
			if (_data->GetOneByKey(filename, &id) != false)
			{
				//有历史信息
				std::string new_id = GetFileIdentifier(filename);
				if (new_id == id)
				{
					return false;//不需要被上传-上次上传后没有被修改过
				}
			}
			//有一个文件比较大，正在不断的拷贝到这个目录下，拷贝需要过程
			//据上次上传后有一段时间没有被修改才上传
			FileUtil fu(filename);
			if (time(NULL) - fu.LastMTime() < 3) {//三秒钟之内被修改过
				return false;
			}
			std::cout << filename <<" "<< "need upload" << std::endl;
			return true;
			
		}
		bool RunModule() 
		{
			while (1)
			{
				//1.遍历获取指定文件夹中所有文件
				FileUtil fu(_back_dir);
				std::vector<std::string> arry;
				fu.ScanDirector(&arry);//目录浏览完毕
				//2.逐个判断是否需要上传
				for (auto& ch : arry)
				{
					if (IsNeedUpload(ch)==false)
					{
						continue;
					}
					//3.如果需要上传则上传文件
					if (Upload(ch) == true)
					{
						std::string id = GetFileIdentifier(ch);//创建唯一标识
						_data->Insert(ch, id);//新增文件备份信息
						std::cout << ch <<" "<< "upload success" << std::endl;
					}
				}
				usleep(5);
			}
			return true;
		}
	private:
		std::string _back_dir;
		DataManager* _data;

	};
}
#endif