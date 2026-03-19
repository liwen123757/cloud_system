#include <iostream>
#include <string>
#include <vector>

int Split(const std::string &str,const std::string &sep,std::vector<std::string> *arry)
{
    //abc  def
    size_t pos=0,idx=0;
    int count=0;
    while(1)
    {
        //find（要查找的字符，从那里开始查找的偏移量）
        pos=str.find(sep,idx);
        if(pos==std::string::npos)
        {
            break;
        }
        if(pos==idx)
        {
            idx=pos+sep.size();
            continue;
        }
        //找到了，截取字符串 substr(截取的起始位置，截取的偏移量)
        std::string tmp=str.substr(idx,pos-idx);
        arry->push_back(tmp);
        count++;
        idx=pos+sep.size();
    }
    if(idx<str.size())
    {
        //截取最后一段
        std::string tmp=str.substr(idx);
        arry->push_back(tmp);
    }
    return count;
}

int main()
{
    std::string str="abc cde vfr ";
    std::vector<std::string> array;
    Split(str," ",&array);
    for(auto &ch:array)
    {
        std::cout<<ch<<std::endl;
    }
    return 0;
}