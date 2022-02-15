//
// Created by utusikov on 12.12.2021.
//

#include "optimizations.h"

#include <algorithm>
#include <list>
#include <cstring>

#include "utils.h"

std::vector<uint8_t> diffs(std::vector<uint8_t> bytes) {
    std::vector<uint8_t> res(bytes.size());
    res[0] = bytes[0];
    for (size_t i = 1; i < bytes.size(); ++i) {
        res[i] = bytes[i] - bytes[i - 1];
    }
    return res;
}

std::vector<uint8_t> rev_diffs(const std::vector<uint8_t> &bytes) {
    std::vector<uint8_t> res(bytes.size());
    res[0] = bytes[0];
    for (size_t i = 1; i < bytes.size(); ++i) {
        res[i] = res[i - 1] + bytes[i];
    }
    return res;
}

std::vector<uint8_t> mtf(std::vector<uint8_t> bytes) {
    std::list<std::uint8_t> list;

    for (uint32_t i = 0U; i < ALPHABET_SIZE; ++i) {
        list.push_back(i);
    }

    std::vector<uint8_t> result;
    result.reserve(bytes.size());

    for (uint8_t byte: bytes) {
        uint8_t index = 0U;
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (*it != byte) {
                ++index;
                continue;
            }
            list.erase(it);
            break;
        }
        list.push_front(byte);
        result.push_back(index);
    }

    return result;
}

std::vector<uint8_t> rev_mtf(const std::vector<uint8_t> &bytes) {
    std::list<uint8_t> list;

    for (uint32_t i = 0; i < ALPHABET_SIZE; ++i) {
        list.push_back(i);
    }
    std::vector<uint8_t> result;
    result.reserve(bytes.size());

    for (uint8_t byte: bytes) {
        uint8_t index = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (index != byte) {
                ++index;
                continue;
            }
            uint8_t old = *it;
            result.push_back(old);
            list.erase(it);
            list.push_front(old);
            break;
        }
    }

    return result;
}

static std::vector<uint32_t> suf_mas(const std::vector<uint8_t>& s) {
    std::vector<uint32_t> suff(s.size());
    for (uint32_t i = 0; i < s.size(); ++i) {
        suff[i] = i;
    }
    std::stable_sort(suff.begin(), suff.end(), [&] (uint32_t l, uint32_t r) { return s[l] < s[r]; });
    std::vector<uint32_t> c(s.size());
    for (uint32_t i = 0; i < s.size(); ++i) {
        c[i] = s[i];
    }

    for (uint32_t i = 1; i < s.size(); i <<= 1) {
        uint32_t mod = c.size();
        std::stable_sort(suff.begin(), suff.end(), [&] (uint32_t l, uint32_t r) {
            return c[l] != c[r] ? c[l] < c[r] : c[(l + i) % mod] < c[(r + i) % mod];
        });
        std::vector<uint32_t> nc(c.size(), 0U);
        for (uint32_t j = 1; j < s.size(); ++j) {
            uint32_t l1 = suff[j - 1];
            uint32_t r1 = (suff[j - 1] + i) % mod;
            uint32_t l2 = suff[j];
            uint32_t r2 = (suff[j] + i) % mod;
            nc[suff[j]] = nc[suff[j - 1]] + (c[l1] != c[l2] || c[r1] != c[r2]);
        }
        c = std::move(nc);
    }
    return suff;
}

std::vector<uint8_t> bwt(std::vector<uint8_t> bytes) {
    uint32_t size = bytes.size();
    std::vector<uint32_t> shifts = suf_mas(bytes);
    uint32_t k = 0U;
    while (shifts[k] > 0U) {
        ++k;
    }
    std::vector<uint8_t> res(4U, 0U);
    std::memcpy(res.data(), reinterpret_cast<const uint8_t *>(&k), sizeof(uint32_t));

    for (uint32_t shift: shifts) {
        res.emplace_back(bytes[(shift + size - 1) % size]);
    }
    return res;
}

std::vector<uint8_t> rev_bwt(std::vector<uint8_t> bytes) {
    if (bytes.size() < 4) {
        LOG_ERROR("Bad input bytes");
        return {};
    }
    uint32_t k = *reinterpret_cast<const uint32_t *>(bytes.data());
    bytes.erase(bytes.begin(), bytes.begin() + sizeof(uint32_t));

    std::vector<uint32_t> index(bytes.size());

    for (uint32_t i = 0; i < index.size(); ++i) {
        index[i] = i;
    }

    std::stable_sort(index.begin(), index.end(), [&bytes](uint32_t i, uint32_t j) {
        return bytes[i] < bytes[j];
    });

    std::vector<uint8_t> res(bytes.size());

    uint32_t cur = k;
    for (uint32_t i = 0; i < bytes.size(); ++i) {
        res[i] = bytes[index[cur]];
        cur = index[cur];
    }
    return res;
}

std::vector<uint8_t> rle(std::vector<uint8_t> bytes) {
    std::vector<uint8_t> res;
    const auto add_seq = [&res] (uint8_t &cnt, uint8_t byte) {
        res.emplace_back(byte);
        res.emplace_back(cnt - 1);
        cnt = 0;
    };

    uint8_t cnt = 0U;
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i == 0) {
            res.emplace_back(bytes[i]);
            continue;
        }
        if (bytes[i - 1] == bytes[i]) {
            ++cnt;
            if (cnt == 255U  || i + 1 == bytes.size()) {
                add_seq(cnt, bytes[i]);
            }
            continue;
        }
        if (cnt > 0) {
            add_seq(cnt, bytes[i - 1]);
        }
        res.emplace_back(bytes[i]);
    }
    return res;
}

std::vector<uint8_t> rev_rle(const std::vector<uint8_t> &bytes) {
    std::vector<uint8_t> res;
    bool was_cnt = false;
    for (size_t i = 0; i < bytes.size();) {
        if (i == 0 || bytes[i - 1] != bytes[i] || was_cnt) {
            res.emplace_back(bytes[i++]);
            was_cnt = false;
            continue;
        }
        if (i + 1 == bytes.size()) {
            res.emplace_back(bytes[i++]);
            break;
        }
        assert(i + 1 < bytes.size());
        for (size_t j = 0; j <= bytes[i + 1]; ++j) {
            res.emplace_back(bytes[i]);
        }
        i += 2;
        was_cnt = true;
    }
    return res;
}