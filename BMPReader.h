#pragma once

#include "Image.h"
#include <string>

class BmpReader {
public:
    static Image Read(const std::string& path);

private:
    static std::size_t CalculateRowSize(std::size_t width);
};