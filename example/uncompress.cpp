#include <iostream>
#include <string>
#include <fstream>
#include "bundle.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "argv[1]是压缩包名称\n";
        std::cout << "argv[2]是解压后的文件名称\n";
        return -1;
    }
    std::string ifilename=argv[1];//这是压缩包名称
    std::string ofilename=argv[2];//这是解压后的文件名称
    std::ifstream ifs;
    ifs.open(ifilename,std::ios::binary);
    ifs.seekg(0,std::ios::end);
    size_t fsize=ifs.tellg();//读取文件大小
    ifs.seekg(0,std::ios::beg);
    std::string body;
    body.resize(fsize);
    ifs.read(&body[0],fsize);//将压缩包里的数据读取到body里
    ifs.close();

    std::string unpacked=bundle::unpack(body);//将读去出来的内容进行解压缩

    std::ofstream ofs;
    ofs.open(ofilename,std::ios::binary);
    ofs.write(&unpacked[0],unpacked.size());//将压缩包里解压出来的内容写入文件中
    ofs.close();
    return 0;
}