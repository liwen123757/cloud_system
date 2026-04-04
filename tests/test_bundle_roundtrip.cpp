#include <cassert>
#include <iostream>
#include <string>

#include "cloude_system/bundle.h"

int main() {
    const std::string original =
        "cloud_system round-trip test\n"
        "1234567890\n"
        "中文内容也要能正常压缩解压\n";

    const std::string packed = bundle::pack(bundle::LZIP, original);
    assert(!packed.empty());

    const std::string unpacked = bundle::unpack(packed);
    assert(unpacked == original);

    std::cout << "bundle round-trip test passed" << std::endl;
    return 0;
}
