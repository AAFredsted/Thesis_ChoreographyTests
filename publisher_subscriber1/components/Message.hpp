#pragma once
#include <chrono>
#include <string>

template <typename T> struct Message {
  std::string header;
  size_t timestamp;
  T *data;
};