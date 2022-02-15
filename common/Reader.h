//
// Created by utusikov on 09.11.2021.
//

#pragma once
#include <fstream>
#include <vector>

class Reader {
public:
    explicit Reader(const std::string &path);
    ~Reader() = default;

    bool is_open() const { return input.is_open(); }
    bool eof() const { return input.eof(); }
    uint32_t read_uint();
    uint8_t read_byte();
    std::pair<uint32_t, std::vector<uint8_t>> read_nbits();
    std::vector<uint8_t> read_nbytes(uint32_t need);
    std::vector<uint8_t> read_all_bytes();
    size_t file_sz() const { return file_size; }
    void seek_begin();

private:
    std::ifstream input;
    size_t file_size{0U};
};
