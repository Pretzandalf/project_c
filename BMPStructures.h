#pragma once

#include <cstdint>

#pragma pack(push, 1) // кланг очень не хотел эту строчку пропускать
struct BitmapFileHeader {
    std::uint16_t bfType = 0;
    std::uint32_t bfSize = 0;
    std::uint16_t bfReserved1 = 0;
    std::uint16_t bfReserved2 = 0;
    std::uint32_t bfOffBits = 0;
};

struct BitmapInfoHeader {
    std::uint32_t biSize = 0;
    std::int32_t biWidth = 0;
    std::int32_t biHeight = 0;
    std::uint16_t biPlanes = 0;
    std::uint16_t biBitCount = 0;
    std::uint32_t biCompression = 0;
    std::uint32_t biSizeImage = 0;
    std::int32_t biXPelsPerMeter = 0;
    std::int32_t biYPelsPerMeter = 0;
    std::uint32_t biClrUsed = 0;
    std::uint32_t biClrImportant = 0;
};
#pragma pack(pop)
// просто структуры холдеры для переменных бмп формата