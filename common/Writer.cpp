//
// Created by utusikov on 12.12.2021.
//

#include "Writer.h"

#include <cassert>

#include "utils.h"

Writer::Writer(const std::string &path) : output(path, std::ios::binary) {
    if (!output.is_open()) {
        LOG_ERROR("Wrong output filepath: %s", path.c_str());
    }
}

void Writer::write_nbits(uint32_t n, const std::vector<uint8_t>& v) {
    assert((n / 8U) + (static_cast<uint32_t>(n % 8U != 0)) == v.size());
    write_uint(n);
    output.write(reinterpret_cast<const char*>(v.data()), v.size());
}