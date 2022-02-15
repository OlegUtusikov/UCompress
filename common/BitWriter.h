//
// Created by utusikov on 09.11.2021.
//
#pragma once

#include <cstddef>
#include <vector>
#include <type_traits>

class BitWriter {
public:
    using type_t = uint8_t;

    BitWriter() {
        data.reserve(INIT_BYTES);
    }

    ~BitWriter() = default;

    size_t bytes_size() const { return 4 + data.size(); }
    size_t size() const { return 32 + pos; }

    size_t bytes_size_data() const { return data.size(); }
    size_t size_data() const { return pos; }

    std::vector<type_t> move() {
        std::vector<type_t> res = std::move(data);
        pos = 0U;
        data = {};
        data.reserve(INIT_BYTES);
        update();
        return res;
    }

    void append_bit(bool bit) {
        update();
        data[get_block(pos)] |= bit << (get_shift(pos));
        ++pos;
    }

    template<typename Iterator>
    void append_bits(const Iterator &begin, const Iterator &end) {
        for (Iterator it = begin; it != end; ++it) {
            append_bit(*it);
        }
    }

    void append_bits(const std::initializer_list<bool> &bits) {
        append_bits(bits.begin(), bits.end());
    }

    void append_bits(const std::vector<bool> &bits) {
        append_bits(bits.begin(), bits.end());
    }

    template<typename T>
    void append(T val) {
        static_assert(!std::is_same<bool, T>::value && std::is_integral<T>::value, "Type isn't integral");
        for (size_t i = 0; i < sizeof(T) * 8U; ++i) {
            append_bit((val >> i) & 1U);
        }
    }

private:
    void update() {
        if (get_block(pos) >= data.size()) {
            data.emplace_back(0U);
        }
    }

    static uint32_t get_block(uint32_t bit) { return bit / (sizeof(type_t) * 8U); };
    static uint32_t get_shift(uint32_t bit) { return bit % (sizeof(type_t) * 8U); };

    static constexpr size_t INIT_BYTES = 2048;
    std::vector<type_t> data;
    uint32_t pos{0U};
};
