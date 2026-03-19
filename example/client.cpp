#include "httplib.h"
#include <memory>
#define SERVER_IP "127.37.67.170"
#define SERVER_PORT 9090
int main()
{
    httplib::Client client(SERVER_IP,SERVER_PORT);
    httplib::MultipartFormData item;
    item.name="file";
    item.filename="hello.txt";
    item.content="Hello World";
    item.content_type="text/plain";

    httplib::MultipartFormDataItems items;
    items.push_back(item);

    auto res=client.Post("/multipart",items);//提交请求，获取响应返回值
    std::cout<<res->status<<std::endl;
    std::cout<<res->body<<std::endl;
    return 0;
}