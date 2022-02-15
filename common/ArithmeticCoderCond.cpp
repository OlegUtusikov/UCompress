#include "ArithmeticCoderCond.h"

#include "JpegCommon.h"
#include "optimizations.h"
#include "utils.h"

void ACoderCond::code(Reader &reader, Writer &writer) {
    if (!reader.is_open() || reader.eof() || !writer.is_open()) {
        LOG_ERROR("Can't open input or output!!!");
        return;
    }

    BitWriter bits;

    const auto code_ari = [&bits, this] (const std::vector<uint8_t> &bytes) {
        uint8_t prev = bytes[0];
        bits.append<uint8_t>(prev);

        for (uint32_t i = 1; i < bytes.size(); ++i) {
            int32_t symbol = char_to_index[prev][bytes[i]];
            encode_symbol(bits, symbol, prev);
            update_tables(symbol, prev);
            prev = bytes[i];
        }

        encode_symbol(bits, EOF_SYMBOL, prev);
        end_encoding(bits);
    };

    auto bytes = reader.read_all_bytes();
    auto data = remove_JPEG_tags(std::move(bytes));
    code_ari(rle(mtf(bwt(std::move(data)))));

    uint32_t bits_cnt = bits.size_data();
    writer.write_nbits(bits_cnt, bits.move());
}

void ACoderCond::encode_symbol(BitWriter &bits, int32_t symbol, uint8_t prev) {
    assert(symbol > 0);
    uint64_t range = high - low + 1U;
    high = low + ((range * cum_freq[prev][symbol - 1]) / cum_freq[prev][0]) - 1U;
    low = low + ((range * cum_freq[prev][symbol]) / cum_freq[prev][0]);
    for (;;) {
        assert(low <= high);
        if (high < HALF) {
            output_bits(bits, false);
        }
        else if (low >= HALF) {
            output_bits(bits, true);
            low -= HALF;
            high -= HALF;
        } else if (FIRST_QTR <= low && high < THIRD_QTR) {
            ++opposite_bits;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }
        low <<= 1U;
        high = (high << 1U) | 1U;
    }
}

void ACoderCond::end_encoding(BitWriter &bits) {
    ++opposite_bits;
    output_bits(bits, low >= FIRST_QTR);
}

void ACoderCond::output_bits(BitWriter &bits, bool bit) {
    bits.append_bit(bit);
    while (opposite_bits > 0) {
        bits.append_bit(!bit);
        --opposite_bits;
    }
}
