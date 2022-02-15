#include "ArithmeticUtils.h"

FreqManager::FreqManager() {
    for (int32_t i = 0; i < ALPHABET_SIZE; ++i) {
        char_to_index[i] = i + 1;
        index_to_char[i + 1] = i;
    }
    uint32_t sum = 0;
    for (int32_t i = SYMBOLS; i >= 0; --i) {
        freq[i] = 1;
        cum_freq[i] = sum;
        sum += freq[i];
    }
    freq[0] = 0;
}

FreqManager::~FreqManager() = default;

void FreqManager::update_tables(int32_t sym_index) {
    if (cum_freq[0] == MAX_FREQ) {
        uint32_t sum = 0;
            for (int32_t i = SYMBOLS; i >= 0; --i) {
                freq[i] = (freq[i] + 1) / 2;
                cum_freq[i] = sum;
                sum += freq[i];
        }
    }
    int32_t i = sym_index;
    while (freq[i] == freq[i - 1]) {
        --i;
    }

    if (i < sym_index) {
        uint8_t ch_i = index_to_char[i];
        uint8_t ch_symbol = index_to_char[sym_index];
        index_to_char[i] = ch_symbol;
        index_to_char[sym_index] = ch_i;
        char_to_index[ch_i] = sym_index;
        char_to_index[ch_symbol] = i;
    }
    ++freq[i];
    while (i > 0) {
        cum_freq[--i]++;
    }
}
