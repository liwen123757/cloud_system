#include <iostream>
#include <string>
#include <memory>
#include <jsoncpp/json/json.h>

int main()
{
    std::string str=R"({"姓名":"小黑","年龄":19,"成绩":[58.5,56,59]})";
    Json::Value root;
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb. newCharReader());
    std::string err;
    bool ret=cr->parse(str.c_str(),str.c_str()+str.size(),&root,&err);
    if(ret==false)
    {
        std::cout<<"parse error: "<<err<<std::endl;
    }
    std::cout<<root["姓名"].asString()<<std::endl;
    std::cout<<root["年龄"].asInt()<<std::endl;
    int size=root["成绩"].size();
    for(int i=0;i<size;i++)
    {
        std::cout<<root["成绩"][i].asFloat()<<std::endl;
    }

    return 0;
}
