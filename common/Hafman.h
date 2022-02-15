//
// Created by utusikov on 08.11.2021.
//

#pragma once


#include <array>
#include <queue>
#include <unordered_map>
#include <memory>

#include "BitReader.h"
#include "BitWriter.h"
#include "Reader.h"
#include "Writer.h"
#include "constants.h"


class Hafman {
public:
    void code(Reader &reader, Writer &writer);
    void decode(Reader &reader, Writer &writer);

private:
    struct node_t;
    using ptr_t = std::unique_ptr<node_t>;

    struct node_t {
        node_t() = default;

        explicit node_t(uint8_t letter, ptr_t left = nullptr, ptr_t right = nullptr) :
                letter(letter),
                left(std::move(left)),
                right(std::move(right)) {}

        ~node_t() = default;

        bool is_leaf() const { return !left && !right; }

        void print(const std::string &indent) const;

        uint8_t letter{0};
        ptr_t left{};
        ptr_t right{};
    };

    void set_codes();

    void set_codes_impl(const ptr_t &ptr, codeword_t &word);

    void build_tree(const std::array<uint32_t, ALPHABET_SIZE> &stat);

    /**
     * Read hafman tree from bits stream/
     * @param reader bits stream
     * @return count of bits.
     */
    size_t fetch(BitReader &bits);

    /**
     * Write hafman tree into writer.
     */
    void store(BitWriter &bits) const;

    template<typename Visitor>
    static void dfs(const ptr_t &ptr, const Visitor &visit) {
        if (!ptr->is_leaf()) {
            dfs(ptr->left, visit);
            dfs(ptr->right, visit);
        }
        visit(ptr.get());
    }

    template<typename Visitor>
    static void bfs(const ptr_t &ptr, const Visitor &visit) {
        std::queue<const node_t *> queue;
        queue.push(ptr.get());
        while (!queue.empty()) {
            const node_t *cur = queue.front();
            queue.pop();
            visit(cur);
            if (!cur->is_leaf()) {
                queue.push(cur->left.get());
                queue.push(cur->right.get());
            }
        }
    }

    ptr_t root;
    std::unordered_map<codeword_t, uint8_t> code_to_letter;
    std::array<codeword_t, ALPHABET_SIZE> letter_to_code;
};
