//
// Created by utusikov on 13.12.2021.
//

#include "ArithmeticCoder.h"
#include "Coder.h"
#include "utils.h"

int main(int argc, char * argv[]) {
    if (argc != 3) {
        LOG_ERROR("Wrong count = %d of program arguments", argc);
        return 0;
    }
    Coder coder(argv[1], argv[2]);
    coder.run<ACoder>();
}