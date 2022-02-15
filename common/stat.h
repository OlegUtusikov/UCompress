//
// Created by utusikov on 12.12.2021.
//

#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include "constants.h"

std::string compute_hs(const std::vector<uint8_t> &bytes);

std::array<uint32_t, ALPHABET_SIZE> bytes_freq(const std::vector<uint8_t> &bytes);