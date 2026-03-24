#include "Filters.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace { // как будто все таки фильтры написать в одном файлике, чем их разбивать на тысячу маленьких, хоть выглядит жутковато
constexpr std::uint8_t KMaxColorValue = 255;

constexpr double KGrayRedCoefficient = 0.299;
constexpr double KGrayGreenCoefficient = 0.587;
constexpr double KGrayBlueCoefficient = 0.114;

constexpr std::array<std::array<double, 3>, 3> KSharpenMatrix = {{
    {0.0, -1.0, 0.0},
    {-1.0, 5.0, -1.0},
    {0.0, -1.0, 0.0},
}};

constexpr std::array<std::array<double, 3>, 3> KEdgeDetectionMatrix = {{
    {0.0, -1.0, 0.0},
    {-1.0, 4.0, -1.0},
    {0.0, -1.0, 0.0},
}};

constexpr double KSepiaRedRed = 0.393;
constexpr double KSepiaRedGreen = 0.769;
constexpr double KSepiaRedBlue = 0.189;

constexpr double KSepiaGreenRed = 0.349;
constexpr double KSepiaGreenGreen = 0.686;
constexpr double KSepiaGreenBlue = 0.168;

constexpr double KSepiaBlueRed = 0.272;
constexpr double KSepiaBlueGreen = 0.534;
constexpr double KSepiaBlueBlue = 0.131;

constexpr int KSizeCoreGauss = 6;
constexpr double MaxValue = 255.0;
constexpr double MinValue = 255.0;

std::uint8_t ClampColor(double value) {
    return static_cast<std::uint8_t>(std::clamp(value, MinValue, MaxValue));
}
}  // namespace

BaseFilter::BaseFilter() = default;

BaseFilter::BaseFilter(Image* image) : image_(image) {
    if (image_ == nullptr) {
        throw std::invalid_argument("Image pointer is null");
    }
}

void GrayScale::Active() {
    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            Pixel& pixel = image_->GetPixel(x, y);
            const double gray = pixel.red * KGrayRedCoefficient +
                                pixel.green * KGrayGreenCoefficient +
                                pixel.blue * KGrayBlueCoefficient;
            const std::uint8_t value = ClampColor(gray);

            pixel.red = value;
            pixel.green = value;
            pixel.blue = value;
        }
    }
}

void Negative::Active() {
    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            Pixel& pixel = image_->GetPixel(x, y);
            pixel.red = KMaxColorValue - pixel.red;
            pixel.green = KMaxColorValue - pixel.green;
            pixel.blue = KMaxColorValue - pixel.blue;
        }
    }
}

void Crop::SetNum(std::size_t new_width, std::size_t new_height) {
    new_width_ = new_width;
    new_height_ = new_height;
}

void Crop::Active() {
    const std::size_t cropped_width = std::min(new_width_, image_->GetWidth());
    const std::size_t cropped_height = std::min(new_height_, image_->GetHeight());

    Image new_image(cropped_width, cropped_height);

    for (std::size_t y = 0; y < cropped_height; ++y) {
        for (std::size_t x = 0; x < cropped_width; ++x) {
            new_image.SetPixel(x, y, image_->GetPixel(x, y));
        }
    }

    *image_ = std::move(new_image);
}

void MatrixFiltr::SetMatrix(const std::array<std::array<double, 3>, 3>& matrix_filter) {
    matrix_ = matrix_filter;
}

void MatrixFiltr::Active() {
    Image source = *image_;

    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            double new_red = 0.0;
            double new_green = 0.0;
            double new_blue = 0.0;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    const Pixel& pixel = source.GetPixelToNearest(static_cast<int>(x) + dx, static_cast<int>(y) + dy);
                    const double weight = matrix_[dy + 1][dx + 1];

                    new_red += pixel.red * weight;
                    new_green += pixel.green * weight;
                    new_blue += pixel.blue * weight;
                }
            }

            Pixel result;
            result.red = ClampColor(new_red);
            result.green = ClampColor(new_green);
            result.blue = ClampColor(new_blue);

            image_->SetPixel(x, y, result);
        }
    }
}

void Sharpening::Active() {
    MatrixFiltr::SetMatrix(KSharpenMatrix);
    MatrixFiltr::Active();
}

void EdgeDetection::SetThresod(double threshold) {
    threshold_ = threshold;
}

void EdgeDetection::BlackWhite() {
    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            Pixel& pixel = image_->GetPixel(x, y);

            const double normalized = static_cast<double>(pixel.red) / MaxValue;
            const std::uint8_t color = normalized > threshold_ ? 255 : 0;

            pixel.red = color;
            pixel.green = color;
            pixel.blue = color;
        }
    }
}

void EdgeDetection::Active() {
    GrayScale::Active();
    MatrixFiltr::SetMatrix(KEdgeDetectionMatrix);
    MatrixFiltr::Active();
    BlackWhite();
}

void GaussianBlur::MakeCoreOne() {
    size_core_ = KSizeCoreGauss * static_cast<int>(omega_) + 1;
    if (size_core_ % 2 == 0) {
        ++size_core_;
    }

    const int center = size_core_ / 2;
    gauss_one_core_.assign(size_core_, 0.0);

    double sum = 0.0;
    for (int i = 0; i < size_core_; ++i) {
        const int dx = i - center;
        const double value = std::exp(-(dx * dx) / (2.0 * omega_ * omega_));
        gauss_one_core_[i] = value;
        sum += value;
    }

    for (double& value : gauss_one_core_) {
        value /= sum;
    }
}

void GaussianBlur::SetOmega(double omega) {
    omega_ = omega;
    MakeCoreOne();
}

void GaussianBlur::Active() {
    Image source = *image_;
    Image temp = *image_;

    const int center = size_core_ / 2;

    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            double new_red = 0.0;
            double new_green = 0.0;
            double new_blue = 0.0;

            for (int k = 0; k < size_core_; ++k) {
                const int sample_y = std::clamp(
                    static_cast<int>(y) + (k - center),
                    0,
                    static_cast<int>(image_->GetHeight()) - 1
                );

                const Pixel& pixel = source.GetPixel(x, static_cast<std::size_t>(sample_y));
                new_red += pixel.red * gauss_one_core_[k];
                new_green += pixel.green * gauss_one_core_[k];
                new_blue += pixel.blue * gauss_one_core_[k];
            }

            Pixel blurred;
            blurred.red = ClampColor(new_red);
            blurred.green = ClampColor(new_green);
            blurred.blue = ClampColor(new_blue);

            temp.SetPixel(x, y, blurred);
        }
    }

    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            double new_red = 0.0;
            double new_green = 0.0;
            double new_blue = 0.0;

            for (int k = 0; k < size_core_; ++k) {
                const int sample_x = std::clamp(
                    static_cast<int>(x) + (k - center),
                    0,
                    static_cast<int>(image_->GetWidth()) - 1
                );

                const Pixel& pixel = temp.GetPixel(static_cast<std::size_t>(sample_x), y);
                new_red += pixel.red * gauss_one_core_[k];
                new_green += pixel.green * gauss_one_core_[k];
                new_blue += pixel.blue * gauss_one_core_[k];
            }

            Pixel blurred;
            blurred.red = ClampColor(new_red);
            blurred.green = ClampColor(new_green);
            blurred.blue = ClampColor(new_blue);

            image_->SetPixel(x, y, blurred);
        }
    }
}

void Sepia::Active() {
    for (std::size_t y = 0; y < image_->GetHeight(); ++y) {
        for (std::size_t x = 0; x < image_->GetWidth(); ++x) {
            const Pixel pixel = image_->GetPixel(x, y);

            const double new_red =
                pixel.red * KSepiaRedRed + pixel.green * KSepiaRedGreen + pixel.blue * KSepiaRedBlue;
            const double new_green =
                pixel.red * KSepiaGreenRed + pixel.green * KSepiaGreenGreen + pixel.blue * KSepiaGreenBlue;
            const double new_blue =
                pixel.red * KSepiaBlueRed + pixel.green * KSepiaBlueGreen + pixel.blue * KSepiaBlueBlue;

            Pixel result;
            result.red = ClampColor(new_red);
            result.green = ClampColor(new_green);
            result.blue = ClampColor(new_blue);

            image_->SetPixel(x, y, result);
        }
    }
}