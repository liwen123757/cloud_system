#ifndef __M_DATA_H__
#define __M_DATA_H__
#include <iostream>
#include "util.hpp"
#include <pthread.h>
#include <unordered_map>
#include <vector>
#include <memory>
namespace lwc
{
    class DataManager
    {
    public:
        DataManager(std::string manager_file):_manager_file(manager_file) {
            InitLoad();
        }
        ~DataManager(){}

        bool Insert(const std::string& key,const std::string& value)
        {
            _table[key]=value;
            //存储到硬盘上
            storage();
            return true;
        }
        
        bool GetOneByKey(const std::string &key, std::string *val)
        {
            auto it=_table.find(key);
            if(it==_table.end())
            {
                return false;
            }
            *val=it->second;
            return true;
        }
        
        bool Update(const std::string& key,const std::string& value)
        {
            _table[key]=value;
            //同步到硬盘
            storage();
        }
        bool storage()
        {
            std::stringstream ss;
            for(auto ch:_table)
            {
                ss<<ch.first<<" "<< ch.second<<"\n";
            }
            FileUtil fu(_manager_file);
            fu.SentContent(ss.str());
            return true;
        }
        
        int Split(const std::string &str,const std::string &seq,std::vector<std::string> *arry)
        {
            int pos=0,idx=0;
            int count=0;
            while(1)
            {
                pos=str.find(seq,idx);
                if(pos==std::string::npos)
                {
                    break;
                }
                //开头就为要查找的字符或者要查找的字符连续了
                if(pos==idx)
                {
                    idx=pos+seq.size();
                    continue;
                }
                //找到了要查找的字符
                std::string ss=str.substr(idx,pos-idx);
                arry->push_back(ss);
                count++;
                //继续向下查找
                idx=pos+seq.size();
            }
            //截取最后一段
            std::string ss=str.substr(idx);
            arry->push_back(ss);
            count++;
            return count;
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
            std::vector<std::string> arry;
            Split(body,"\n",&arry);
            for(auto &ch:arry)//k  v -- k空格v形式
            {
                std::vector<std::string> tmp;
                Split(ch," ",&tmp);
                if(tmp.size()!=2)
                {
                    continue;
                }
                _table[tmp[0]]=tmp[1];
            }
            return true;
        }
    private:
        std::string _manager_file;
        std::unordered_map<std::string,std::string> _table;
    };
}
#endif