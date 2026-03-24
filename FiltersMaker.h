#pragma once

#include "Filters.h"

#include <memory>
#include <string>
#include <vector>

class FiltersMaker {
public:
    static std::unique_ptr<BaseFilter> Create(
        const std::string& filter_name,
        const std::vector<std::string>& filter_params,
        Image* image
    );

    static bool IsKnownFilter(const std::string& filter_name);
};