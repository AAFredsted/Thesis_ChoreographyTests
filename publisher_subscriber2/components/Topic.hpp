#pragma once
#include "Message.hpp"
#include <functional>
#include <mutex>
#include <string>
#include <vector>

class TopicBase {
public:
  explicit TopicBase(const std::string &name) : name(name) {}

  TopicBase(const TopicBase &other) = delete;
  TopicBase &operator=(const TopicBase &other) = delete;

  virtual ~TopicBase() = default;

  virtual const std::string &getName() const = 0;

protected:
  std::string name;
};

template <typename T> class Topic : public TopicBase {
public:
  using MessageType = Message<T>;
  using Callback = std::function<void(const MessageType &)>;

  explicit Topic(const std::string &name) : TopicBase(name), subscribers() {}

  Topic(const Topic &other) = delete;
  Topic &operator=(const Topic &other) = delete;

  Topic(Topic &&other) noexcept
      : TopicBase(std::move(other)), subscribers(std::move(other.subscribers)) {
  }

  Topic &operator=(Topic &&other) noexcept {
    if (this != &other) {
      name = std::move(other.name);
      subscribers = std::move(other.subscribers);
    }
    return *this;
  }

  ~Topic() override = default;

  void subscribe(Callback callback) {
    std::unique_lock<std::mutex> lock(topic_mutex);
    subscribers.push_back(callback);
  }

  void publish(const MessageType &msg) {
    std::unique_lock<std::mutex> lock(topic_mutex);
    for (const auto &subscriber : subscribers) {
      subscriber(msg);
    }
  }

  const std::string &getName() const override { return name; }

private:
  std::vector<Callback> subscribers;
  std::mutex topic_mutex;
};