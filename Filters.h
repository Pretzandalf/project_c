#pragma once

#include "Image.h"

#include <array>
#include <cstddef>
#include <vector>

class BaseFilter {
public:
    BaseFilter();
    explicit BaseFilter(Image* image);

    virtual ~BaseFilter() = default;
    virtual void Active() = 0;

protected:
    Image* image_ = nullptr;
};

class GrayScale : virtual public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void Active() override;
};

class Negative : public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void Active() override;
};

class Crop : public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void SetNum(std::size_t new_width, std::size_t new_height);
    void Active() override;

private:
    std::size_t new_height_ = 0;
    std::size_t new_width_ = 0;
};

class MatrixFiltr : virtual public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void SetMatrix(const std::array<std::array<double, 3>, 3>& matrix_filter);
    void Active() override;

protected:
    std::array<std::array<double, 3>, 3> matrix_{};
};

class Sharpening : virtual public BaseFilter, public MatrixFiltr {
public:
    using BaseFilter::BaseFilter;

    void Active() override;
};

class EdgeDetection : virtual public BaseFilter, public MatrixFiltr, virtual public GrayScale {
public:
    using BaseFilter::BaseFilter;

    void SetThresod(double threshold);
    void BlackWhite();
    void Active() override;

private:
    double threshold_ = 0.0;
};

class GaussianBlur : public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void SetOmega(double omega);
    void Active() override;

private:
    void MakeCoreOne();

private:
    double omega_ = 1.0;
    std::vector<double> gauss_one_core_;
    int size_core_ = 0;
};

class Sepia : public BaseFilter {
public:
    using BaseFilter::BaseFilter;

    void Active() override;
};