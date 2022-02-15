//
// Created by utusikov on 09.11.2021.
//

#pragma once
#include <fstream>
#include <vector>

class Writer {
public:
    explicit Writer(const std::string &path);
    ~Writer() = default;

    bool is_open() const { return output.is_open(); }
    void write_byte(uint8_t byte) { output.write(reinterpret_cast<char*>(&byte), sizeof(uint8_t)); }
    void write_uint(uint32_t val) { output.write(reinterpret_cast<char*>(&val), sizeof(uint32_t)); }
    void write_nbits(uint32_t n, const std::vector<uint8_t>& v);

private:
    std::ofstream output;
};
