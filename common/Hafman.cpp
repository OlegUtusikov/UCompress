//
// Created by utusikov on 12.12.2021.
//

#include "Hafman.h"

#include <cassert>
#include <set>

#include "optimizations.h"
#include "stat.h"
#include "utils.h"

void Hafman::code(Reader &reader, Writer &writer) {
    if (!reader.is_open() || reader.eof() || !writer.is_open()) {
        return;
    }
    std::vector<uint8_t> raw_bytes = reader.read_all_bytes();
    std::string report = compute_hs(raw_bytes);
    std::vector<uint8_t> bytes = rle(mtf(bwt(raw_bytes)));

    build_tree(bytes_freq(bytes));

    BitWriter bits;
    store(bits);
    for (uint8_t byte: bytes) {
        bits.append_bits(letter_to_code[byte]);
    }

    // if 1 then compress, else write raw data
    bool is_good = bits.bytes_size() < reader.file_sz();

    LOG("%s, avg_bits: %f, in_sz: %zu, out_sz: %zu, rate: %f",
        report.c_str(),
        static_cast<double>(bits.size()) / reader.file_sz(),
        reader.file_sz(), bits.bytes_size(), 1.0 * reader.file_sz() / bits.bytes_size());

    if (is_good) {
        bits.append_bit(true);
        uint32_t bits_cnt = bits.size_data();
        writer.write_nbits(bits_cnt, bits.move());
    } else {
        BitWriter raw_bits;
        for (uint8_t byte : raw_bytes) {
            raw_bits.append<uint8_t>(byte);
        }
        raw_bits.append_bit(false);
        uint32_t bits_cnt = raw_bits.size_data();
        writer.write_nbits(bits_cnt, raw_bits.move());
    }
}

void Hafman::decode(Reader &reader, Writer &writer) {
    if (!reader.is_open() || reader.eof() || !writer.is_open()) {
        return;
    }
    auto bits_data = reader.read_nbits();
    BitReader bits(bits_data.first, bits_data.second);
    if (!bits.peek_last_bit()) {
        uint8_t buffer = 0U;
        for (uint32_t i = 0; i < bits_data.first - 1; ++i) {
            buffer |= (bits.read_bit() << (i % 8));
            if (i % 8 == 7 || i + 2 == bits_data.first) {
                writer.write_byte(buffer);
                buffer = 0U;
            }
        }
        return;
    }
    size_t tree_bits_count = fetch(bits);
    assert(tree_bits_count < bits_data.first);
    set_codes();

    std::vector<uint8_t> decoded;
    codeword_t cur_word;

    const auto process_word = [&]() {
        auto it = code_to_letter.find(cur_word);
        if (it != code_to_letter.end()) {
            decoded.emplace_back(it->second);
            cur_word.clear();
        }
    };

    for (uint32_t i = 0; i < (bits_data.first - tree_bits_count - 1); ++i) {
        cur_word.push_back(bits.read_bit());
        process_word();
    }

    decoded = rev_bwt(rev_mtf(rev_rle(decoded)));
    for (uint8_t byte: decoded) {
        writer.write_byte(byte);
    }
}

void Hafman::set_codes() {
    if (root->is_leaf()) {
        letter_to_code[root->letter] = {true};
        code_to_letter[{true}] = root->letter;
        return;
    }
    codeword_t word;
    set_codes_impl(root, word);
    assert(word.empty());
}

void Hafman::set_codes_impl(const ptr_t &ptr, codeword_t &word) {
    if (ptr->is_leaf()) {
        uint8_t cur_letter = ptr->letter;
        letter_to_code[cur_letter] = word;
        code_to_letter.emplace(word, cur_letter);
        return;
    }
    const auto process_child = [&](const ptr_t &child, bool bit) {
        word.push_back(bit);
        set_codes_impl(child, word);
        word.pop_back();
    };
    process_child(ptr->left, false);
    process_child(ptr->right, true);
}

void Hafman::build_tree(const std::array<uint32_t, ALPHABET_SIZE> &stat) {
    std::set<std::pair<uint32_t, node_t *>> s;
    for (uint32_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (stat[i] > 0) {
            s.emplace(stat[i], new node_t(i));
        }
    }

    const auto get_min = [&]() {
        auto p = *s.begin();
        s.erase(s.begin());
        return p;
    };

    while (s.size() > 1) {
        auto first = get_min();
        auto second = get_min();
        s.emplace(first.first + second.first, new node_t(0, ptr_t(first.second), ptr_t(second.second)));
    }
    root.reset(get_min().second);
    set_codes();
}

/**
 * Read hafman tree from bits stream/
 * @param reader bits stream
 * @return count of bits.
 */
size_t Hafman::fetch(BitReader &bits) {
    uint32_t read_bits = 0;
    root = std::make_unique<node_t>();
    std::queue<node_t *> queue;
    queue.push(root.get());

    while (!queue.empty()) {
        node_t *cur = queue.front();
        queue.pop();
        if (bits.can_read_bit()) {
            ++read_bits;
            if (bits.read_bit()) {
                cur->left = std::make_unique<node_t>();
                cur->right = std::make_unique<node_t>();
                queue.push(cur->left.get());
                queue.push(cur->right.get());
            } else {
                if (bits.can_read<uint8_t>()) {
                    read_bits += 8U;
                    cur->letter = bits.read<uint8_t>();
                } else {
                    LOG_ERROR("Can't read letter for hafman leaf");
                }
            }
            continue;
        }
        LOG_ERROR("Can't read hafman tree bit from reader");
    }
    return read_bits;
}

/**
 * Write hafman tree into writer.
 */
void Hafman::store(BitWriter &bits) const {
    bfs(root, [&](const node_t *ptr) {
        bool bit = !ptr->is_leaf();
        bits.append_bit(bit);
        if (!bit) {
            bits.append(ptr->letter);
        }
    });
}

void Hafman::node_t::print(const std::string &indent) const {
    if (is_leaf()) {
        LOG("%sptr: %p, letter: %c\n", indent.c_str(), this, letter);
        return;
    }
    left->print(indent + "  ");
    LOG("%sptr: %p, letter: %c, left: %p, right: %p\n", indent.c_str(), this, letter, left.get(), right.get());
    right->print(indent + "  ");
}
