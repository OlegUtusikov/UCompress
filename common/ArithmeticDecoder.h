#pragma once

#include "ArithmeticUtils.h"

#include "Reader.h"
#include "Writer.h"
#include "BitReader.h"

class ADecoder : public FreqManager {
public:
    ADecoder() = default;
    ~ADecoder() = default;

    void decode(Reader &reader, Writer &writer);
    int32_t decode_symbol(BitReader &bits);
private:
    uint64_t low{0U};
    uint64_t high{MAX_VALUE};
    uint64_t value{0};
};

