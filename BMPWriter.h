#pragma once

#include "Image.h"

#include <string>

class BmpWriter {
public:
    static void Write(const std::string& path, const Image& image);

private:
    static std::size_t CalculateRowSize(std::size_t width);
};