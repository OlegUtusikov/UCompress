//
// Created by utusikov on 09.11.2021.
//
#pragma once

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>
#include <type_traits>

class BitReader {
public:
    using type_t = uint8_t;

    explicit BitReader(uint32_t size, std::vector<type_t> data) : size(size), data(std::move(data)) {
        assert(this->size <= this->data.size() * 8);
    }
    ~BitReader() = default;

    bool can_read_bit() const { return pos < data.size() * 8U; }
    template<typename T>
    bool can_read(uint32_t need_bits_cnt = sizeof(T) * 8U) const {
        static_assert(!std::is_same<bool, T>::value && std::is_integral<T>::value, "Type isn't integral");
        return pos + (need_bits_cnt - 1U) < data.size() * 8U;
    }

    bool read_bit() {
        bool res = (data[get_block(pos)] >> get_shift(pos)) & 1U;
        ++pos;
        return res;
    }

    template<typename T>
    T read(uint32_t need_bits_cnt = sizeof(T) * 8U) {
        static_assert(!std::is_same<bool, T>::value && std::is_integral<T>::value, "Type isn't integral");
        T res = 0U;
        for (uint32_t i = 0U; i < need_bits_cnt; ++i) {
            res |= read_bit() << i;
        }
        return res;
    }

    bool peek_bit() const { return (data[get_block(pos)] >> get_shift(pos)) & 1U; }
    bool peek_last_bit() const { return (data[get_block(size - 1)] >> get_shift(size - 1)) & 1U; }

    template<typename T>
    T peek(uint32_t need_bits_cnt = sizeof(T) * 8) const {
        T res = 0U;
        for (uint32_t i = 0U; i < need_bits_cnt; ++i) {
            res |= peek_bit() << i;
        }
        return res;
    }
    void skip_bits(uint32_t need_bits_cnt = 1U) { pos += need_bits_cnt; }

private:
    static uint32_t get_block(uint32_t bit) { return bit / (sizeof(type_t) * 8U); };
    static uint32_t get_shift(uint32_t bit) { return bit % (sizeof(type_t) * 8U); };

    std::vector<type_t> data;
    uint32_t pos{0U};
    const uint32_t size{0U};
};
