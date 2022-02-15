//
// Created by utusikov on 13.12.2021.
//

#include "utils.h"
#include "Decoder.h"
#include "ArithmeticDecoder.h"

int main(int argc, char * argv[]) {
    if (argc != 3) {
        LOG_ERROR("Wrong count = %d of program arguments", argc);
        return 0;
    }
    Decoder decoder(argv[1], argv[2]);
    decoder.run<ADecoder>();
}