#pragma once

#include <string>
#include <vector>

class Controller {
public:
    int Run(int argc, const char* argv[]);

private:
    struct ParsedFilter {
        std::string name;
        std::vector<std::string> params;
    };

    void PrintHelp() const;
    std::vector<ParsedFilter> ParseFilters(int argc, const char* argv[]) const;

    std::string input_file_;
    std::string output_file_;
};

// задаем наш контролер, который инитит фильтровые штуки, дальше будем просто передавать изображение в фильтры