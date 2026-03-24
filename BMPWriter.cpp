#include "BMPWriter.h"

#include "BMPStructures.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
const std::uint16_t K_BMP_SIGNATURE = 0x4D42;  // 'BM' тут небольшой повторчик получился
const std::uint16_t K_BITS_PER_PIXEL24 = 24;
const std::uint32_t K_NO_COMPRESSION = 0;
const std::uint32_t K_BITMAP_INFO_HEADER_SIZE = 40;
const std::uint16_t K_BMP_PLANES = 1;
const std::size_t K_BYTES_PER_PIXEL = 3;
}  // namespace

void BmpWriter::Write(const std::string& path, const Image& image) {
    if (image.IsEmpty()) {
        throw std::runtime_error("Cannot write empty image to file: " + path);
    }

    const std::size_t width = image.GetWidth();
    const std::size_t height = image.GetHeight();
    const std::size_t row_size = CalculateRowSize(width);
    const std::size_t image_size = row_size * height;

    BitmapFileHeader file_header;
    file_header.bfType = K_BMP_SIGNATURE;
    file_header.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header.bfSize = static_cast<std::uint32_t>(file_header.bfOffBits + image_size);

    BitmapInfoHeader info_header;
    info_header.biSize = K_BITMAP_INFO_HEADER_SIZE;
    info_header.biWidth = static_cast<std::int32_t>(width);
    info_header.biHeight = static_cast<std::int32_t>(height);  // bottom-up
    info_header.biPlanes = K_BMP_PLANES;
    info_header.biBitCount = K_BITS_PER_PIXEL24;
    info_header.biCompression = K_NO_COMPRESSION;
    info_header.biSizeImage = static_cast<std::uint32_t>(image_size);

    std::ofstream output(path, std::ios::binary);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output file: " + path);
    }

    output.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    if (!output) {
        throw std::runtime_error("Failed to write BMP file header to: " + path);
    }

    output.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
    if (!output) {
        throw std::runtime_error("Failed to write BMP info header to: " + path);
    }

    std::vector<std::uint8_t> row(row_size, 0);

    for (std::size_t file_row = 0; file_row < height; ++file_row) {
        const std::size_t image_y = height - 1 - file_row;

        for (std::size_t x = 0; x < width; ++x) {
            const Pixel& pixel = image.GetPixel(x, image_y);
            const std::size_t offset = x * K_BYTES_PER_PIXEL;

            row[offset + 0] = pixel.blue;
            row[offset + 1] = pixel.green;
            row[offset + 2] = pixel.red;
        }

        output.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row_size));
        if (!output) {
            throw std::runtime_error("Failed to write BMP pixel data to: " + path);
        }
    }
}

std::size_t BmpWriter::CalculateRowSize(std::size_t width) {
    const std::size_t raw_row_size = width * K_BYTES_PER_PIXEL;
    return (raw_row_size + 3) / 4 * 4;
}