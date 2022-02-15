//
// Created by utusikov on 12.12.2021.
//

#pragma once

#include <cstdint>
#include <BitReader.h>
#include <BitWriter.h>

constexpr uint8_t IS_FLAG{0xFF};
constexpr uint8_t START_FLAG{0xD8}; // first bytes in file
constexpr uint8_t COMMENT_FLAG{0xE0}; // comment section
constexpr uint8_t DQT_FLAG{0xDB}; // quantification table
constexpr uint8_t SOFO_FLAG{0xC0}; // is base coding method
constexpr uint8_t DHT_FLAG{0xC4}; // hafman tables
constexpr uint8_t SOS_FLAG{0xDA}; // start of data section
constexpr uint8_t END_FLAG{0xD9}; // end of data

extern const std::vector<uint8_t> START_HEADER;
extern const std::vector<uint8_t> END_HEADER;

bool check_JPEG_flag(const std::vector<uint8_t> &bytes, uint32_t pos, uint8_t mask);
std::pair<std::vector<uint8_t>, std::vector<uint8_t>> split_JPEG_format(std::vector<uint8_t> bytes);
std::vector<uint8_t> add_JPEG_tags(std::vector<uint8_t> bytes);
std::vector<uint8_t> remove_JPEG_tags(std::vector<uint8_t> bytes);


bool is_good_flag(uint8_t byte);

uint8_t read_flag(BitReader &bits);

void write_flag(BitWriter &writer, uint8_t flag);

uint32_t read_length(BitReader &bits);

void write_length(BitWriter &writer, uint32_t length);

struct Section {
public:
    Section() = default;

    virtual ~Section() = default;

    virtual uint8_t flag() const = 0;

    void write(BitWriter &writer) {
        writer.append<uint8_t>(IS_FLAG);
        writer.append<uint8_t>(flag());
        write_extra(writer);
    }

protected:
    virtual void write_extra(BitWriter &writer) const = 0;
};

struct DataSection : public Section {
public:
    explicit DataSection(BitReader &reader, bool is_size_included);

    ~DataSection() override = default;

protected:
    void write_extra(BitWriter &writer) const override;

private:
    bool is_size_included{false};
    std::vector<uint8_t> bytes;
};

struct CommentSection : public DataSection {
    explicit CommentSection(BitReader &reader);

    ~CommentSection() override = default;

    uint8_t flag() const override { return COMMENT_FLAG; }
};