//
// Created by utusikov on 08.11.2021.
//

#pragma once

#include <cassert>
#include <iostream>
#include <cstdio>
#include <vector>

#define LOG(fmt, ...) do {                                       \
  printf(fmt, ##__VA_ARGS__);                                    \
} while (false)


#define LOG_ERROR(fmt, ...) do {                             \
  printf("ERROR in %s(line: %d): ", __FILE__, __LINE__);     \
  printf(fmt, ##__VA_ARGS__);                                \
  printf("\n");                                              \
} while (false)

