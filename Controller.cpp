#include "Controller.h"

#include "BMPReader.h"
#include "BMPWriter.h"
#include "FiltersMaker.h"
#include "Image.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

int Controller::Run(int argc, const char* argv[]) {
    if (argc < 3) {
        PrintHelp();
        return 0;
    }

    input_file_ = argv[1];
    output_file_ = argv[2];

    const std::vector<ParsedFilter> parsed_filters = ParseFilters(argc, argv);

    Image image = BmpReader::Read(input_file_);

    for (const ParsedFilter& parsed_filter : parsed_filters) {
        std::unique_ptr<BaseFilter> filter = FiltersMaker::Create(parsed_filter.name, parsed_filter.params, &image);
        filter->Active();
    }

    BmpWriter::Write(output_file_, image);
    return 0;
}

void Controller::PrintHelp() const {
    std::cout << "Usage:\n";
    std::cout << "image_processor <input.bmp> <output.bmp> "
                 "[-filter [param1] [param2] ...] ...\n\n";

    std::cout << "Available filters:\n";
    std::cout << "-crop width height    Crop image to given width and height\n";
    std::cout << "-gs                   Convert image to grayscale\n";
    std::cout << "-neg                  Convert image to negative\n";
    std::cout << "-sharp                Sharpen image\n";
    std::cout << "-edge threshold       Detect edges\n";
    std::cout << "-blur sigma           Apply gaussian blur\n";
}

std::vector<Controller::ParsedFilter> Controller::ParseFilters(int argc, const char* argv[]) const {
    std::vector<ParsedFilter> parsed_filters;

    int i = 3;
    while (i < argc) {
        const std::string filter_name = argv[i];

        if (!FiltersMaker::IsKnownFilter(filter_name)) {
            throw std::runtime_error("Unknown filter: " + filter_name);
        }

        ParsedFilter parsed_filter;
        parsed_filter.name = filter_name;
        ++i;

        while (i < argc && !FiltersMaker::IsKnownFilter(argv[i])) {
            parsed_filter.params.push_back(argv[i]);
            ++i;
        }

        parsed_filters.push_back(parsed_filter);
    }

    return parsed_filters;
}