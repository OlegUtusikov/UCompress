#include "ArithmeticDecoderCond.h"
#include "JpegCommon.h"
#include "optimizations.h"
#include "BitWriter.h"
#include "utils.h"

void ADecoderCond::decode(Reader &reader, Writer &writer) {
    if (!reader.is_open() || reader.eof() || !writer.is_open()) {
        LOG_ERROR("Can't open streams!!!");
        return;
    }

    auto raw_bits = reader.read_nbits();
    BitReader bits(raw_bits.first, raw_bits.second);
    
    std::vector<uint8_t> res;
    res.emplace_back(bits.read<uint8_t>());

    for (int i = 0; i < CODE_LEN; ++i) {
        value = (value << 1U) | bits.read_bit();
    }

    do {
        int32_t sym_index = decode_symbol(bits, res.back());
        if (sym_index == EOF_SYMBOL) {
            break;
        }
        uint8_t decoded_byte = index_to_char[res.back()][sym_index];
        update_tables(sym_index, res.back());
        res.emplace_back(decoded_byte);
    } while (true);

    res = rev_rle(res);
    res = rev_mtf(res);
    res = rev_bwt(res);

    res = add_JPEG_tags(res);
    for (uint8_t byte : res) {
        writer.write_byte(byte);
    }
}

int32_t ADecoderCond::decode_symbol(BitReader &bits, uint8_t prev) {
    uint64_t range = high - low + 1U;
    uint64_t cum = ((value - low + 1U) * cum_freq[prev][0] - 1U) / range;

    int32_t symbol_index = 1U;
    while (cum_freq[prev][symbol_index] > cum) {
        ++symbol_index;
    }
    assert(symbol_index > 0);
    high = low + ((range * cum_freq[prev][symbol_index - 1]) / cum_freq[prev][0]) - 1U;
    low = low + (range * cum_freq[prev][symbol_index]) / cum_freq[prev][0];
    while(true) {
        assert(low <= high);
        if (high < HALF) {
        } else if (low >= HALF) {
            value -= HALF;
            low -= HALF;
            high -= HALF;
        } else if (FIRST_QTR <= low && high < THIRD_QTR) {
            value -= FIRST_QTR;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }
        low <<= 1U;
        high = (high << 1U) | 1U;
        value = value = (value << 1U) | bits.read_bit();
    }
    return symbol_index;
}
