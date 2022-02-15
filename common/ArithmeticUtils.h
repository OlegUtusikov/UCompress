#pragma once

#include <array>
#include <cstdint>

#include "constants.h"


class FreqManager {
public:
    FreqManager();

    ~FreqManager();

protected:
    static constexpr uint32_t CODE_LEN{ 33 };
    static constexpr uint32_t FREQ_LEN{ 31 };

    static constexpr uint64_t MAX_VALUE{ (1ULL << CODE_LEN) - 1U };
    static constexpr uint64_t MAX_FREQ{ (1ULL << FREQ_LEN) - 1U };

    static constexpr uint64_t HALF{ 1ULL << (CODE_LEN - 1U) };
    static constexpr uint64_t FIRST_QTR{ HALF >> 1U };
    static constexpr uint64_t THIRD_QTR{ FIRST_QTR * 3U };

    static constexpr uint32_t EOF_SYMBOL{ ALPHABET_SIZE + 1 };
    static constexpr uint32_t SYMBOLS{ ALPHABET_SIZE + 1 };

    void update_tables(int32_t sym_index);

    std::array<int32_t, ALPHABET_SIZE> char_to_index{}; // byte -> index in freq mass
    std::array<uint8_t, SYMBOLS> index_to_char{};
    std::array<uint32_t, SYMBOLS + 1> cum_freq{};
    std::array<uint32_t, SYMBOLS + 1> freq{};
};

