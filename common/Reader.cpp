//
// Created by utusikov on 12.12.2021.
//

#include "Reader.h"

#include "utils.h"

Reader::Reader(const std::string &path) : input(path, std::ios::binary) {
    if (!input.is_open()) {
        LOG_ERROR("Wrong input filepath: %s", path.c_str());
    } else {
        size_t begin = input.tellg();
        input.seekg(0, std::ios::end);
        file_size = static_cast<size_t>(input.tellg()) - begin;
        input.seekg(0, std::ios::beg);
    }
}

uint32_t Reader::read_uint() {
    uint32_t res;
    input.read(reinterpret_cast<char*>(&res), sizeof(uint32_t));
    return res;
}

std::pair<uint32_t, std::vector<uint8_t>> Reader::read_nbits() {
    std::pair<uint32_t, std::vector<uint8_t>> res;
    res.first = read_uint();
    uint32_t vec_sz = (res.first / 8) + static_cast<uint32_t>(res.first % 8 != 0);
    res.second.resize(vec_sz);
    input.read(reinterpret_cast<char*>(res.second.data()), vec_sz);
    assert(input.tellg() = vec_sz);
    return res;
}

std::vector<uint8_t> Reader::read_nbytes(uint32_t need) {
    std::vector<uint8_t> res(need);
    input.read(reinterpret_cast<char*>(res.data()), res.size());
    return res;
}

std::vector<uint8_t> Reader::read_all_bytes() {
    return read_nbytes(file_size);
}

void Reader::seek_begin() {
    input.clear();
    input.seekg(0, std::ios::beg);
}

uint8_t Reader::read_byte() {
    uint8_t res;
    input.read(reinterpret_cast<char*>(&res), sizeof(uint8_t));
    return res;
}

