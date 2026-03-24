#include "FiltersMaker.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace { //страшно выглядит но по факту это просто парсер без ифов и элсов (то что вы просили в тз)

using Creator = std::function<std::unique_ptr<BaseFilter>(const std::vector<std::string>&, Image*)>;

int ParseIntStrict(const std::string& value, const std::string& parameter_name) {
    std::size_t pos = 0;
    int result = 0;

    try {
        result = std::stoi(value, &pos);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid integer value for parameter \"" + parameter_name + "\": " + value);
    }

    if (pos != value.size()) {
        throw std::runtime_error("Invalid integer value for parameter \"" + parameter_name + "\": " + value);
    }

    return result;
}

double ParseDoubleStrict(const std::string& value, const std::string& parameter_name) {
    std::size_t pos = 0;
    double result = 0.0;

    try {
        result = std::stod(value, &pos);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid numeric value for parameter \"" + parameter_name + "\": " + value);
    }

    if (pos != value.size()) {
        throw std::runtime_error("Invalid numeric value for parameter \"" + parameter_name + "\": " + value);
    }

    return result;
}

int ParsePositiveInt(const std::string& value, const std::string& parameter_name) {
    const int result = ParseIntStrict(value, parameter_name);
    if (result <= 0) {
        throw std::runtime_error("Parameter \"" + parameter_name + "\" must be positive");
    }
    return result;
}

double ParsePositiveDouble(const std::string& value, const std::string& parameter_name) {
    const double result = ParseDoubleStrict(value, parameter_name);
    if (result <= 0.0) {
        throw std::runtime_error("Parameter \"" + parameter_name + "\" must be positive");
    }
    return result;
}

void RequireParamCount(const std::vector<std::string>& params, std::size_t expected_count,
                       const std::string& filter_name) {
    if (params.size() != expected_count) {
        throw std::runtime_error("Filter \"" + filter_name + "\" requires exactly " + std::to_string(expected_count) +
                                 " parameter(s)");
    }
}

const std::unordered_map<std::string, Creator>& GetRegistry() {
    static const std::unordered_map<std::string, Creator> REGISTRY = {
        {"-gs",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 0, "-gs");
             return std::make_unique<GrayScale>(image);
         }},
        {"-neg",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 0, "-neg");
             return std::make_unique<Negative>(image);
         }},
        {"-crop",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 2, "-crop");

             const int width = ParsePositiveInt(params[0], "width");
             const int height = ParsePositiveInt(params[1], "height");

             auto filter = std::make_unique<Crop>(image);
             filter->SetNum(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
             return filter;
         }},
        {"-sharp",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 0, "-sharp");
             return std::make_unique<Sharpening>(image);
         }},
        {"-edge",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 1, "-edge");

             const double threshold = ParseDoubleStrict(params[0], "threshold");
             if (threshold < 0.0 || threshold > 1.0) {
                 throw std::runtime_error("Parameter \"threshold\" must be in range [0, 1]");
             }

             auto filter = std::make_unique<EdgeDetection>(image);
             filter->SetThresod(threshold);
             return filter;
         }},
        {"-blur",
         [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 1, "-blur");

             const double sigma = ParsePositiveDouble(params[0], "sigma");

             auto filter = std::make_unique<GaussianBlur>(image);
             filter->SetOmega(sigma);
             return filter;
         }},
        {"-sp", [](const std::vector<std::string>& params, Image* image) -> std::unique_ptr<BaseFilter> {
             RequireParamCount(params, 0, "-sp");
             return std::make_unique<Sepia>(image);
         }}};

    return REGISTRY;
}

}  // namespace

std::unique_ptr<BaseFilter> FiltersMaker::Create(const std::string& filter_name,
                                                const std::vector<std::string>& filter_params, Image* image) {
    if (image == nullptr) {
        throw std::runtime_error("Cannot create filter for null image");
    }

    const auto& registry = GetRegistry();
    const auto it = registry.find(filter_name);

    if (it == registry.end()) {
        throw std::runtime_error("Unknown filter: " + filter_name);
    }

    return it->second(filter_params, image);
}

bool FiltersMaker::IsKnownFilter(const std::string& filter_name) {
    const auto& registry = GetRegistry();
    return registry.find(filter_name) != registry.end();
}