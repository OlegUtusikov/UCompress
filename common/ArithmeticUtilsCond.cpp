#include "ArithmeticUtilsCond.h"

FreqManagerCond::FreqManagerCond() {
    for (uint32_t prev = 0; prev < ALPHABET_SIZE; ++prev) {
        for (int32_t i = 0; i < ALPHABET_SIZE; ++i) {
            char_to_index[prev][i] = i + 1;
            index_to_char[prev][i + 1] = i;
        }
        uint32_t sum = 0;
        for (int32_t i = SYMBOLS; i >= 0; --i) {
            freq[prev][i] = 1;
            cum_freq[prev][i] = sum;
            sum += freq[prev][i];
        }
        freq[prev][0] = 0;
    }
}

FreqManagerCond::~FreqManagerCond() = default;

void FreqManagerCond::update_tables(int32_t sym_index, uint8_t prev) {
    if (cum_freq[prev][0] == MAX_FREQ) {
        uint32_t sum = 0;
            for (int32_t i = SYMBOLS; i >= 0; --i) {
                freq[prev][i] = (freq[prev][i] + 1) / 2;
                cum_freq[prev][i] = sum;
                sum += freq[prev][i];
        }
    }
    int32_t i = sym_index;
    while (freq[prev][i] == freq[prev][i - 1]) {
        --i;
    }

    if (i < sym_index) {
        uint8_t ch_i = index_to_char[prev][i];
        uint8_t ch_symbol = index_to_char[prev][sym_index];
        index_to_char[prev][i] = ch_symbol;
        index_to_char[prev][sym_index] = ch_i;
        char_to_index[prev][ch_i] = sym_index;
        char_to_index[prev][ch_symbol] = i;
    }
    ++freq[prev][i];
    while (i > 0) {
        cum_freq[prev][--i]++;
    }
}
