#pragma once

#include <cstddef>
#include <vector>
#include <cstdint>

struct Pixel { //да нужно выводить все классы, но такой маленький я бы тут оставил)
    std::uint8_t red = 0;
    std::uint8_t green = 0;
    std::uint8_t blue = 0;
};

class Image {
public:
    Image();
    Image(std::size_t width, std::size_t height);

    std::size_t GetWidth() const;
    std::size_t GetHeight() const;
    bool IsEmpty() const;

    const Pixel& GetPixel(std::size_t x, std::size_t y) const;
    Pixel& GetPixel(std::size_t x, std::size_t y);
    const Pixel& GetPixelToNearest(int x, int y) const;

    void SetPixel(std::size_t x, std::size_t y, const Pixel& pixel);

    void Resize(std::size_t new_width, std::size_t new_height);
    void Clear();

private:
    std::size_t GetIndex(std::size_t x, std::size_t y) const;

    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::vector<Pixel> pixels_;
};

// задаем хранилище изображений, чтобы на него накидывать фильтры без всякой хрени с бмп