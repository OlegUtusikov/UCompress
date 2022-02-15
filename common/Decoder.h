//
// Created by utusikov on 09.11.2021.
//

#pragma once

#include "Reader.h"
#include "Writer.h"

class Decoder {
public:
    Decoder(std::string in, std::string out) : in_path(std::move(in)), out_path(std::move(out)) {}
    ~Decoder() = default;

    template<typename DecoderImpl>
    void run() {
        Reader reader(in_path);
        Writer writer(out_path);
        DecoderImpl d;
        d.decode(reader, writer);
    }

private:
    std::string in_path;
    std::string out_path;
};