#ifndef __MY__DATA__
#define __MY__DATA__
#include "util.hpp"
#include <unordered_map>
#include <sstream>

namespace lw
{
	class DataManager
	{
	public:
		DataManager(const std::string backup_file) :_backup_file(backup_file) {
			InitLoad();
		}
		
		bool Storage()
		{
			//1.获取备份信息
			std::stringstream ss;
			for (auto& it : _tables)
			{
				//2.将所有信息进行指定持久化格式的组织
				
				ss << it.first << " " << it.second << "\n";
			}
			//3.持久化存储
			FileUtil fu(_backup_file);
			fu.SetContent(ss.str());
			return true;
		}

		int Split(const std::string& str, const std::string& sep, std::vector<std::string>* arry)
		{
			//abc  def
			size_t pos = 0, idx = 0;
			int count = 0;
			while (1)
			{
				//find（要查找的字符，从那里开始查找的偏移量）
				pos = str.find(sep, idx);
				if (pos == std::string::npos)
				{
					break;
				}
				if (pos == idx)
				{
					idx = pos + sep.size();
					continue;
				}
				//找到了，截取字符串 substr(截取的起始位置，截取的偏移量)
				std::string tmp = str.substr(idx, pos - idx);
				arry->push_back(tmp);
				count++;
				idx = pos + sep.size();
			}
			if (idx < str.size())
			{
				//截取最后一段
				std::string tmp = str.substr(idx);
				arry->push_back(tmp);
			}
			return count;
		}

		bool InitLoad()
		{
			//1.从文件中读取所有数据
			FileUtil fu(_backup_file);
			std::string body;
			fu.GetContent(&body);
			//2.反格式化（进行数据解析），添加到表中
			std::vector<std::string> arry;
			Split(body, "\n", &arry);
			for (auto& a : arry)
			{
				//b.txt b.txt-34567-345634
				std::vector<std::string> tmp;
				Split(a, " ", &tmp);
				if (tmp.size() != 2)
				{
					continue;
				}
				_tables[tmp[0]] = tmp[1];
			}
			return true;

		}

		bool Insert(const std::string& key, const std::string& val)
		{
			_tables[key] = val;
			Storage();
			return true;
		}
		bool Update(const std::string& key, const std::string& val)
		{
			_tables[key] = val;
			Storage();
			return true;
		}
		bool GetOneByKey(const std::string& key,std::string* val)
		{
			auto it = _tables.find(key);
			if (it == _tables.end())
			{
				return false;
			}
			*val = it->second;
			return true;
		}

	private:
		std::string _backup_file;//备份信息的持久化存储文件
		std::unordered_map<std::string, std::string> _tables;
	};
}


#endif