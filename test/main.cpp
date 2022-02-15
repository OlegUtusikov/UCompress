//
// Created by utusikov on 09.11.2021.
//

#include <cstring>

#include "utils.h"
#include "Coder.h"
#include "Decoder.h"
#include "optimizations.h"
#include "ArithmeticCoder.h"
#include "ArithmeticDecoder.h"

static uint32_t get_stream_size(std::ifstream &stream) {
    size_t begin = stream.tellg();
    stream.seekg(0, std::ios::end);
    uint32_t file_size = static_cast<size_t>(stream.tellg()) - begin;
    stream.seekg(0, std::ios::beg);
    return file_size;
}

static bool equalFiles(std::ifstream &in1, std::ifstream &in2) {
    std::ifstream::pos_type size1, size2;

    size1 = in1.seekg(0, std::ifstream::end).tellg();
    in1.seekg(0, std::ifstream::beg);

    size2 = in2.seekg(0, std::ifstream::end).tellg();
    in2.seekg(0, std::ifstream::beg);

    if (size1 != size2) {
        LOG_ERROR("HERE1");
        return false;
    }

    static const size_t BLOCKSIZE = 4096;
    size_t remaining = size1;

    while (remaining) {
        char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
        size_t size = std::min(BLOCKSIZE, remaining);

        in1.read(buffer1, size);
        in2.read(buffer2, size);

        if (0 != std::memcmp(buffer1, buffer2, size)) {
            LOG_ERROR("HERE2");
            return false;
        }

        remaining -= size;
    }

    return true;
}

int main() {
    const std::string DATA_PATH = "C:/Users/utusi/source/repos/UCompress/UCompress/data/jpeg";
    std::vector<std::string> factors = {"30", "80"};
    const std::string DATA_PATH_CODED = DATA_PATH + "_c";
    const std::string DATA_PATH_DECODED = DATA_PATH + "_d";
//    std::vector<std::string> files = {"bib", "book1", "book2", "geo", "news", "obj1", "obj2", "paper1", "paper2", "pic",
//                                      "progc", "progl", "progp", "trans"};
    std::vector<std::string> files = {"airplane", "arctichare", "baboon", "cat", "fruits", "frymire",
                                      "girl", "lena", "monarch", "peppers", "pool", "sails", "serrano", "tulips",
                                      "watch"};
    const auto gen_jpg_name = [](const std::string &path, const std::string &factor, const std::string &name) {
        return path + factor + "/" + name + factor + ".jpg";
    };

    for (const auto &factor: factors) {
        LOG("Coding...\n");
        for (const auto &file: files) {
            Coder coder(gen_jpg_name(DATA_PATH, factor, file), gen_jpg_name(DATA_PATH_CODED, factor, file));
            coder.run<ACoder>();
        }
        LOG("Decoding ...\n");
        for (const auto &file: files) {
            Decoder decoder(gen_jpg_name(DATA_PATH_CODED, factor, file), gen_jpg_name(DATA_PATH_DECODED, factor, file));
            decoder.run<ADecoder>();
        }
        LOG("Checking equal...\n");
        for (const auto &file: files) {
            std::ifstream raw_file(gen_jpg_name(DATA_PATH, factor, file), std::ios::binary);
            std::ifstream decoded_file(gen_jpg_name(DATA_PATH_DECODED, factor, file), std::ios::binary);
            assert(equalFiles(raw_file, decoded_file));
        }

        LOG("Counting sizes...\n");
        uint32_t raw_sz = 0;
        uint32_t coded_sz = 0;
        for (const auto &file: files) {
            LOG("%s: ", file.c_str());
            std::ifstream raw_file(gen_jpg_name(DATA_PATH, factor, file), std::ios::binary);
            std::ifstream coded_file(gen_jpg_name(DATA_PATH_CODED, factor, file), std::ios::binary);
            uint32_t cur_raw_sz = get_stream_size(raw_file);
            uint32_t cur_coded_sz = get_stream_size(coded_file);
            LOG("raw = %u, coded = %u\n", cur_raw_sz, cur_coded_sz);
            raw_sz += cur_raw_sz;
            coded_sz += cur_coded_sz;
        }
        LOG("Profit for factor %s: %lf(raw_total_size = %u, coded_total_size = %u)\n", factor.c_str(), 1.0 * raw_sz / coded_sz, raw_sz, coded_sz);
    }
    LOG("DONE!!!\n");
}