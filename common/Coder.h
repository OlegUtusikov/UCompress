//
// Created by utusikov on 09.11.2021.
//

#pragma once

#include "Reader.h"
#include "Writer.h"

class Coder {
public:
    Coder(std::string in, std::string out) : in_path(std::move(in)), out_path(std::move(out)) {}
    ~Coder() = default;

    template<typename CoderImpl>
    void run() {
        Reader reader(in_path);
        Writer writer(out_path);
        CoderImpl c;
        c.code(reader, writer);
    }

private:
    std::string in_path;
    std::string out_path;
};
