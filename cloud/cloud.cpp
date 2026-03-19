#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
void FileUtilTest(const std::string &filename)
{
    // lw::FileUtil fu(filename);
    // std::cout << fu.FileSize() << std::endl;
    // std::cout << fu.LastMTime() << std::endl;
    // std::cout << fu.LastATime() << std::endl;
    // std::cout << fu.FileName() << std::endl;
    // lw::FileUtil fu(filename);
    // std::string body;
    // fu.GetContent(&body);
    // lw::FileUtil nfu("./hello.txt");
    // nfu.SetContent(body);
    return;
    //std::string packname = filename + ".lz";
    // lw::FileUtil fu(filename);
    // fu.Compress(packname);
    // lw::FileUtil pfu(packname);
    // pfu.UnCompress("hello.txt");
    // lw::FileUtil fu(filename);
    // fu.CreateDirectory();
    // std::vector<std::string> arry;
    // fu.ScanDirector(&arry);
    // for (auto &arr : arry)
    // {
    //     std::cout << arr << std::endl;
    // }
    //return;
}
void JsonUtilTest()
{
    const char *name = "小明";
    int age = 19;
    float score[] = {85, 88.5, 99};
    Json::Value root;
    root["姓名"] = name;
    root["年龄"] = age;
    root["成绩"].append(score[0]);
    root["成绩"].append(score[1]);
    root["成绩"].append(score[2]);
    std::string json_str;
    lw::JsonUtil::Serialize(root, &json_str);
    std::cout << json_str << std::endl;

    Json::Value val;
    lw::JsonUtil::UnSerialize(&val, json_str);
    std::cout << val["姓名"].asString() << std::endl;
    std::cout << val["年龄"].asInt() << std::endl;
    for (int i = 0; i < val["成绩"].size(); i++)
    {
        std::cout << val["成绩"][i].asFloat() << std::endl;
    }
}
void ConfigTest()
{
    lw::Config *config = lw::Config::GetInstance();
    std::cout << config->GetHotTime() << std::endl;
    std::cout << config->GetServerPort() << std::endl;
    std::cout << config->GetServerIp() << std::endl;
    std::cout << config->GetDownLoadPrefix() << std::endl;
    std::cout << config->GetPackFileSuffix() << std::endl;
    std::cout << config->GetBackDir() << std::endl;
    std::cout << config->GetPackDir() << std::endl;
    std::cout << config->GetBackupFile() << std::endl;
}
void DataTest() // const std::string &filename
{
    lw::DataManager data;
    std::vector<lw::BackupInfo> arry;
    data.GetAll(&arry);
    for (auto ch : arry)
    {
        std::cout << ch.pack_flag << std::endl;
        std::cout << ch.fsize << std::endl;
        std::cout << ch.mtime << std::endl;
        std::cout << ch.atime << std::endl;
        std::cout << ch.real_path << std::endl;
        std::cout << ch.pack_path << std::endl;
        std::cout << ch.url << std::endl;
    }
    // lw::BackupInfo info;
    // info.NewBackupInfo(filename);
    // lw::DataManager data;
    // data.Insert(info);

    // lw::BackupInfo tmp;
    // data.GetOneByUrl("/download/bundle.h", &tmp);
    // std::cout<<"------------insert and getonbyurl--------\n";
    // std::cout << tmp.pack_flag << std::endl;
    // std::cout << tmp.fsize << std::endl;
    // std::cout << tmp.mtime << std::endl;
    // std::cout << tmp.atime << std::endl;
    // std::cout << tmp.real_path << std::endl;
    // std::cout << tmp.pack_path << std::endl;
    // std::cout << tmp.url << std::endl;

    // info.pack_flag = true;
    // data.Update(info);
    // std::vector<lw::BackupInfo> arry;
    // data.GetAll(&arry);
    // std::cout<<"--------------upfate and getall----------\n";
    // for (auto ch : arry)
    // {
    //     std::cout << ch.pack_flag << std::endl;
    //     std::cout << ch.fsize << std::endl;
    //     std::cout << ch.mtime << std::endl;
    //     std::cout << ch.atime << std::endl;
    //     std::cout << ch.real_path << std::endl;
    //     std::cout << ch.pack_path << std::endl;
    //     std::cout << ch.url << std::endl;
    // }
    // std::cout<<"-------------------realpath--------------\n";
    // data.GetOneByRealPath(filename,&tmp);
    // std::cout << tmp.pack_flag << std::endl;
    // std::cout << tmp.fsize << std::endl;
    // std::cout << tmp.mtime << std::endl;
    // std::cout << tmp.atime << std::endl;
    // std::cout << tmp.real_path << std::endl;
    // std::cout << tmp.pack_path << std::endl;
    // std::cout << tmp.url << std::endl;
    // data.Storage();
}
lw::DataManager *_data;
void HotTest()
{
    lw::HotManager hot;
    hot.RunModule();
}
void ServiceTest()
{
    lw::Service srv;
    srv.RunModule();
}
int main(int argc, char *argv[])
{
    //_data=new lw::DataManager();
    // DataTest();
    FileUtilTest(argv[1]);
    // JsonUtilTest();
    // ConfigTest();
    // DataTest(argv[1]);
    // ServiceTest();
    // HotTest();
    // std::thread thread_hot_manager(HotTest);
    // std::thread thread_service(ServiceTest);
    // thread_hot_manager.join();
    // thread_service.join();
    return 0;
}
