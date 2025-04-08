#pragma once
#include <memory>
#include <print>

template <typename T> struct Message {
  std::string header;
  size_t time;
  std::shared_ptr<T> data;

  Message() : header(""), time(0), data(nullptr) {}

  Message(const std::string header, size_t time, T &&data)
      : header(header), time(time), data(std::make_shared<T>(std::move(data))) {
  }

  Message(const Message &other)
      : header(other.header), time(other.time), data(other.data) {}

  Message &operator=(const Message &other) {
    if (this != &other) {
      header = other.header;
      time = other.time;
      data = other.data;
    }
    return *this;
  }

  Message(Message &&other) noexcept
      : header(std::move(other.header)), time(other.time),
        data(std::move(other.data)) {}

  Message &operator=(Message &&other) noexcept {
    if (this != &other) {
      header = std::move(other.header);
      time = other.time;
      data = std::move(other.data);
    }
    return *this;
  }

  void print() const {
    std::println("Message with Header {} and Date: {}", header, time);
  }
};