//
// Created by utusikov on 12.12.2021.
//

#include "JpegCommon.h"
#include <cassert>
#include "utils.h"

const std::vector<uint8_t> START_HEADER = {IS_FLAG, START_FLAG, IS_FLAG, COMMENT_FLAG};
const std::vector<uint8_t> END_HEADER = {IS_FLAG, END_FLAG};

bool is_good_flag(uint8_t byte) {
    return byte == START_FLAG || byte == COMMENT_FLAG || byte == DQT_FLAG || byte == SOFO_FLAG || byte == DHT_FLAG ||
           byte == SOS_FLAG || byte == END_FLAG;
}

uint8_t read_flag(BitReader &bits) {
    assert(bits.read<uint8_t>() == IS_FLAG);
    auto byte = bits.read<uint8_t>();
    LOG("flag : %u\n", byte);
    assert(is_good_flag(byte));
    return byte;
}

void write_flag(BitWriter &writer, uint8_t flag) {
    writer.append<uint8_t>(IS_FLAG);
    writer.append<uint8_t>(flag);
}

uint32_t read_length(BitReader &bits) {
    return bits.read<uint16_t>();
}

void write_length(BitWriter &writer, uint32_t length) {
    writer.append<uint16_t>(length);
}

DataSection::DataSection(BitReader &reader, bool is_size_included) : is_size_included(is_size_included) {
    auto size = read_length(reader) - 2U * static_cast<uint32_t>(this->is_size_included);
    bytes.resize(size);
    for (uint8_t &byte: bytes) {
        byte = reader.read<uint8_t>();
    }
}

void DataSection::write_extra(BitWriter &writer) const {
    write_length(writer, bytes.size() + 2U * static_cast<uint32_t>(is_size_included));
    for (uint8_t byte: bytes) {
        writer.append<uint8_t>(byte);
    }
}

CommentSection::CommentSection(BitReader &reader) : DataSection(reader, false) {}

bool check_JPEG_flag(const std::vector<uint8_t> &bytes, uint32_t pos, uint8_t mask) {
    return bytes[pos] == IS_FLAG && bytes[pos + 1] == mask;
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> split_JPEG_format(std::vector<uint8_t> bytes) {
    std::vector<uint8_t> header;
    uint32_t pos = 0;
    while (pos + 1 < bytes.size() && !check_JPEG_flag(bytes, pos, SOS_FLAG)) {
        header.emplace_back(bytes[pos++]);
    }
    assert(pos + 2 < bytes.size());
    std::vector<uint8_t> data(bytes.begin() + pos + 2, bytes.end());
    return std::make_pair(header, data);
}

std::vector<uint8_t> add_JPEG_tags(std::vector<uint8_t> bytes) {
    bytes.insert(bytes.begin(), START_HEADER.begin(), START_HEADER.end());
    bytes.insert(bytes.end(), END_HEADER.begin(), END_HEADER.end());
    return bytes;
}

std::vector<uint8_t> remove_JPEG_tags(std::vector<uint8_t> bytes) {
    assert(START_HEADER.size() + END_HEADER.size() <= bytes.size());

    for (uint32_t i = 0; i < START_HEADER.size(); ++i) {
        assert(bytes[i] == START_HEADER[i]);
    }
    for (uint32_t i = 1; i <= END_HEADER.size(); ++i) {
        assert(bytes[bytes.size() - i] == END_HEADER[END_HEADER.size() - i]);
    }

    for (uint32_t i = 0; !bytes.empty() && i < START_HEADER.size(); ++i) {
        bytes.erase(bytes.begin());
    }
    for (uint32_t i = 0; !bytes.empty() && i < END_HEADER.size(); ++i) {
        bytes.pop_back();
    }
    return bytes;
}