// util.hpp

#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <json/json.h> // 需要使用 JSON 库

// 文件工具类，用于处理文件相关的操作
class FileUtil {
public:
    // 读取文件内容，并返回字符串
    static std::string readFile(const std::string &filePath) {
        // 打开文件
        std::ifstream inFile(filePath);
        std::string content;
        if (inFile) {
            std::string line;
            // 按行读取文件直到结束
            while (std::getline(inFile, line)) {
                content += line + "\n"; // 将每行内容加到总内容中
            }
            inFile.close(); // 关闭文件
        }
        return content; // 返回文件内容
    }

    // 写入字符串到文件
    static void writeFile(const std::string &filePath, const std::string &content) {
        // 打开文件以写入
        std::ofstream outFile(filePath);
        if (outFile) {
            outFile << content; // 写入内容
            outFile.close(); // 关闭文件
        }
    }
};

// JSON 工具类，用于处理 JSON 数据
class JsonUtil {
public:
    // 从字符串解析 JSON 数据
    static Json::Value parseJson(const std::string &jsonStr) {
        Json::Value jsonData;
        Json::CharReaderBuilder builder;
        std::string errors;
        std::istringstream s(jsonStr);
        // 解析字符串至 JSON 对象
        Json::parseFromStream(builder, s, &jsonData, &errors);
        return jsonData; // 返回解析后的 JSON 对象
    }

    // 将 JSON 对象转换为字符串
    static std::string jsonToString(const Json::Value &jsonData) {
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, jsonData); // 返回 JSON 字符串
    }
};

#endif // UTIL_HPP
