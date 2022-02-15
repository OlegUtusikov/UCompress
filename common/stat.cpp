//
// Created by utusikov on 12.12.2021.
//

#include "stat.h"
#include <map>
#include <unordered_map>
#include <cmath>
#include <sstream>

#include "utils.h"


std::string compute_hs(const std::vector<uint8_t> &bytes) {
    auto size = static_cast<double>(bytes.size());
    const auto h_1 = [&]() {
        std::unordered_map<uint8_t, double> stat;
        for (uint8_t byte: bytes) {
            stat[byte]++;
        }

        double res = 0;
        for (const auto &p: stat) {
            double prob = p.second / size;
            res -= prob * std::log2(prob);
        }
        return res;
    };

    const auto h_2 = [&]() {
        std::unordered_map<std::uint8_t, double> stat;
        std::map<std::pair<uint8_t, uint8_t>, double> stat_2;

        for (uint32_t i = 0U; i < bytes.size(); ++i) {
            ++stat[bytes[i]];
            if (i > 0) {
                ++stat_2[{bytes[i], bytes[i - 1]}];
            }
        }

        double res = 0;
        for (const auto &p: stat_2) {
            double cond_prob = p.second / stat[p.first.second];
            double prob = stat[p.first.second] / size;
            res -= prob * (cond_prob * std::log2(cond_prob));
        }

        return res;
    };
    const auto h_3 = [&]() {
        std::map<std::pair<uint8_t, uint8_t>, double> stat_2;
        std::map<std::tuple<uint8_t, uint8_t, uint8_t>, double> stat_3;

        for (uint32_t i = 0U; i < bytes.size(); ++i) {
            if (i == 0) {
                continue;
            }
            ++stat_2[{bytes[i], bytes[i - 1]}];
            if (i == 1) {
                continue;
            }
            ++stat_3[std::forward_as_tuple(bytes[i], bytes[i - 1], bytes[i - 2])];
        }

        double res = 0;
        for (const auto &p: stat_3) {
            uint8_t prev = std::get<1>(p.first);
            uint8_t pprev = std::get<2>(p.first);

            double cond_prob = p.second / stat_2[{prev, pprev}];
            double prob = stat_2[{prev, pprev}] / (size - 2);
            res -= prob * cond_prob * std::log2(cond_prob);
        }

        return res;
    };
    std::stringstream ss;
    ss << "h(x): " << h_1() << ", h(x|x): " << h_2() << ", h(x|xx): " << h_3();
    return ss.str();
}

std::array<uint32_t, ALPHABET_SIZE> bytes_freq(const std::vector<uint8_t> &bytes) {
    std::array<uint32_t, ALPHABET_SIZE> byte_stat{};
    std::fill(byte_stat.begin(), byte_stat.end(), 0U);
    for (uint8_t byte: bytes) {
        ++byte_stat[byte];
    }
    return byte_stat;
}