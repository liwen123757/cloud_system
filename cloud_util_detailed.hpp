// cloud_util_detailed.hpp

#ifndef CLOUD_UTIL_DETAILED_HPP
#define CLOUD_UTIL_DETAILED_HPP

#include <iostream> // 包含输入输出流库
#include <fstream> // 包含文件流库
#include <string> // 包含字符串库
#include <json/json.h> // 包含JSON处理库
#include <zlib.h> // 包含压缩库

// FileUtil 类：用于文件操作、压缩、JSON处理和系统调用的工具类
class FileUtil {
public:
    // 构造函数
    FileUtil() {}

    // 用于读取文件内容的函数
    std::string readFile(const std::string &filePath) {
        std::ifstream file(filePath); // 创建输入文件流
        std::string content; // 存储文件内容的字符串

        if (file.is_open()) { // 检查文件是否打开成功
            std::string line;
            while (std::getline(file, line)) { // 逐行读取文件
                content += line + "\n"; // 将每行内容添加到字符串中，并在行尾添加换行符
            }
            file.close(); // 关闭文件流
        } else {
            std::cerr << "无法打开文件: " << filePath << std::endl; // 输出错误信息
        }
        return content; // 返回文件内容
    }

    // 用于写入文件内容的函数
    void writeFile(const std::string &filePath, const std::string &content) {
        std::ofstream file(filePath); // 创建输出文件流

        if (file.is_open()) { // 检查文件是否打开成功
            file << content; // 将内容写入文件
            file.close(); // 关闭文件流
        } else {
            std::cerr << "无法打开文件: " << filePath << std::endl; // 输出错误信息
        }
    }

    // 用于压缩数据的函数
    std::string compressData(const std::string &data) {
        uLongf compressedDataSize = compressBound(data.size()); // 压缩数据的大小
        std::string compressedData(compressedDataSize, ' '); // 创建压缩数据的字符串

        if (compress((Bytef *)compressedData.data(), &compressedDataSize, (const Bytef *)data.data(), data.size()) == Z_OK) {
            compressedData.resize(compressedDataSize); // 确保压缩后的字符串大小正确
        } else {
            std::cerr << "数据压缩失败" << std::endl; // 输出错误信息
        }
        return compressedData; // 返回压缩后的数据
    }

    // 用于处理JSON的函数
    Json::Value parseJson(const std::string &jsonString) {
        Json::Value jsonData;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;

        std::istringstream stream(jsonString); // 创建字符串流
        if (Json::parseFromStream(readerBuilder, stream, &jsonData, &errs)) {
            return jsonData; // 返回解析后的JSON数据
        } else {
            std::cerr << "JSON解析失败: " << errs << std::endl; // 输出错误信息
        }
        return Json::Value(); // 返回空JSON值
    }

    // 用于执行系统命令的函数
    void execCommand(const std::string &command) {
        system(command.c_str()); // 执行系统命令
    }
};

#endif // CLOUD_UTIL_DETAILED_HPP
