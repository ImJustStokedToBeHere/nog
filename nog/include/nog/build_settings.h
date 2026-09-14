#pragma once

#include <string>
#include <vector>

namespace nog {
    class BuildSettings {
    public:
        std::vector<std::string> search_dirs;
        std::vector<std::string> lib_dirs;
        std::vector<std::string> sys_dirs;
    };
} // namespace nog
