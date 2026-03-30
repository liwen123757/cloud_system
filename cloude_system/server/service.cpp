#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
void FileUtilTest(const std::string &filename)
{
    /* lwc::FileUtil fu(filename);
    std::cout << fu.FileSize() << std::endl;
    std::cout << fu.LastMTime() << std::endl;
    std::cout << fu.LastATime() << std::endl;
    std::cout << fu.Filename() << std::endl; */
    lwc::FileUtil fu(filename);
    std::string body;
    fu.GetContent(body);
    lwc::FileUtil nfu("./hello.txt");
    nfu.SentContent(body);
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

int main()
{
    FileUtilTest("./test.txt");
    return 0;
}