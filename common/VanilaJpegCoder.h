//
// Created by utusikov on 15.12.2021.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>

class VanilaJpegCoder {
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> get_header_and_coefs(const std::string &file);
};

