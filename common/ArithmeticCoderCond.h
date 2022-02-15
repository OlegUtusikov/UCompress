#pragma once

#include "ArithmeticUtilsCond.h"

#include <fstream>

#include "Writer.h"
#include "Reader.h"
#include "BitReader.h"
#include "BitWriter.h"

class ACoderCond : public FreqManagerCond {
public:
    ACoderCond() = default;

    ~ACoderCond() = default;

    void output_bits(BitWriter &bits, bool bit);

    void end_encoding(BitWriter &bits);

    void encode_symbol(BitWriter &bits, int32_t symbol, uint8_t prev);

    void code(Reader &reader, Writer &writer);
private:
    uint64_t low{0U};
    uint64_t high{MAX_VALUE};
    uint32_t opposite_bits{0};
};

