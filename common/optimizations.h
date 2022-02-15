//
// Created by utusikov on 12.12.2021.
//

#pragma once

#include <vector>
#include <cstdint>

#include "constants.h"

std::vector<uint8_t> diffs(std::vector<uint8_t> bytes);
std::vector<uint8_t> rev_diffs(const std::vector<uint8_t> &bytes);

std::vector<uint8_t> mtf(std::vector<uint8_t> bytes);
std::vector<uint8_t> rev_mtf(const std::vector<uint8_t> &bytes);

std::vector<uint8_t> bwt(std::vector<uint8_t> bytes);
std::vector<uint8_t> rev_bwt(std::vector<uint8_t> bytes);

std::vector<uint8_t> rle(std::vector<uint8_t> bytes);
std::vector<uint8_t> rev_rle(const std::vector<uint8_t> &bytes);