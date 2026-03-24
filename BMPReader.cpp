#include "BMPReader.h"

#include "BMPStructures.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
const std::uint16_t K_BMP_SIGNATURE = 0x4D42;  // или же по человечески 'BM' (забавная штука)
const std::uint16_t K_BITS_PER_PIXEL24 = 24;
const std::uint32_t K_NO_COMPRESSION = 0;
const std::uint32_t K_BITMAP_INFO_HEADER_SIZE = 40;
const std::size_t K_BYTES_PER_PIXEL = 3;
}

Image BmpReader::Read(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open input file: " + path);
    }

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    input.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    if (!input) {
        throw std::runtime_error("Failed to read BMP file header from: " + path);
    }

    input.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));
    if (!input) {
        throw std::runtime_error("Failed to read BMP info header from: " + path);
    }

    if (file_header.bfType != K_BMP_SIGNATURE) {
        throw std::runtime_error("File is not a BMP image: " + path);
    }

    if (info_header.biSize != K_BITMAP_INFO_HEADER_SIZE) {
        throw std::runtime_error("Unsupported BMP DIB header in file: " + path);
    }

    if (info_header.biBitCount != K_BITS_PER_PIXEL24) {
        throw std::runtime_error("Only 24-bit BMP is supported: " + path);
    }

    if (info_header.biCompression != K_NO_COMPRESSION) {
        throw std::runtime_error("Compressed BMP is not supported: " + path);
    }

    if (info_header.biWidth <= 0 || info_header.biHeight == 0) {
        throw std::runtime_error("Invalid BMP dimensions in file: " + path);
    }

    const bool is_bottom_up = info_header.biHeight > 0;
    const std::size_t width = static_cast<std::size_t>(info_header.biWidth);
    const std::size_t height = static_cast<std::size_t>(is_bottom_up ? info_header.biHeight : -info_header.biHeight);

    Image image(width, height);

    input.seekg(static_cast<std::streamoff>(file_header.bfOffBits), std::ios::beg);
    if (!input) {
        throw std::runtime_error("Failed to seek to BMP pixel data: " + path);
    }

    const std::size_t row_size = CalculateRowSize(width);
    std::vector<std::uint8_t> row(row_size);

    for (std::size_t file_row = 0; file_row < height; ++file_row) {
        input.read(reinterpret_cast<char*>(row.data()), static_cast<std::streamsize>(row_size));
        if (!input) {
            throw std::runtime_error("Failed to read BMP pixel data: " + path);
        }

        const std::size_t image_y = is_bottom_up ? (height - 1 - file_row) : file_row;

        for (std::size_t x = 0; x < width; ++x) {
            const std::size_t offset = x * K_BYTES_PER_PIXEL;

            Pixel pixel;
            pixel.blue = row[offset + 0];
            pixel.green = row[offset + 1];
            pixel.red = row[offset + 2];

            image.SetPixel(x, image_y, pixel);
        }
    }

    return image;
}

std::size_t BmpReader::CalculateRowSize(std::size_t width) {
    const std::size_t raw_row_size = width * K_BYTES_PER_PIXEL;
    return (raw_row_size + 3) / 4 * 4;
}