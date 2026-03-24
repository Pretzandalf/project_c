#include "Image.h"

#include <algorithm>
#include <stdexcept>
#include <string>

Image::Image() = default;

Image::Image(std::size_t width, std::size_t height)
    : width_(width), height_(height), pixels_(width * height) {
}

std::size_t Image::GetWidth() const {
    return width_;
}

std::size_t Image::GetHeight() const {
    return height_;
}

bool Image::IsEmpty() const {
    return width_ == 0 || height_ == 0;
}

const Pixel& Image::GetPixel(std::size_t x, std::size_t y) const {
    return pixels_.at(GetIndex(x, y));
}

Pixel& Image::GetPixel(std::size_t x, std::size_t y) {
    return pixels_.at(GetIndex(x, y));
}

const Pixel& Image::GetPixelToNearest(int x, int y) const {
    if (IsEmpty()) {
        throw std::out_of_range("No Image, no Pixel");
    }

    const int clamped_x = std::clamp(x, 0, static_cast<int>(width_) - 1);
    const int clamped_y = std::clamp(y, 0, static_cast<int>(height_) - 1);

    return pixels_[GetIndex(static_cast<std::size_t>(clamped_x), static_cast<std::size_t>(clamped_y))];
}

void Image::SetPixel(std::size_t x, std::size_t y, const Pixel& pixel) {
    pixels_.at(GetIndex(x, y)) = pixel;
}

void Image::Resize(std::size_t new_width, std::size_t new_height) {
    width_ = new_width;
    height_ = new_height;
    pixels_.assign(width_ * height_, Pixel{});
}

void Image::Clear() {
    width_ = 0;
    height_ = 0;
    pixels_.clear();
}

std::size_t Image::GetIndex(std::size_t x, std::size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range(
            "Image pixel index out of range: x=" + std::to_string(x) +
            ", y=" + std::to_string(y)
        );
    }
    return y * width_ + x;
}