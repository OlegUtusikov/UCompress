#pragma once

#include "ArithmeticUtilsCond.h"

#include "Reader.h"
#include "Writer.h"
#include "BitReader.h"

class ADecoderCond : public FreqManagerCond {
public:
    ADecoderCond() = default;
    ~ADecoderCond() = default;

    void decode(Reader &reader, Writer &writer);
    int32_t decode_symbol(BitReader &bits, uint8_t prev);
private:
    uint64_t low{0U};
    uint64_t high{MAX_VALUE};
    uint64_t value{0};
};

