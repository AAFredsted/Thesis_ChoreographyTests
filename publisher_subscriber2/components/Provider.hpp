#pragma once
#include "Topic.hpp"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

class Provider {
public:
  explicit Provider() = default;
  Provider(const Provider &other) = delete;
  Provider(Provider &&other) = delete;

  Provider &operator=(const Provider &other) = delete;
  Provider &operator=(Provider &&other) = delete;

  ~Provider() = default;

  // Advertise a new topic
  template <typename T> void advertise(const std::string &topicName) {
    std::unique_lock<std::mutex> lock(provider_mutex);
    if (topics.find(topicName) == topics.end()) {
      topics[topicName] = std::static_pointer_cast<TopicBase>(
          std::make_shared<Topic<T>>(topicName));
    }
  }

  // Subscribe to a topic
  template <typename T>
  void subscribe(const std::string &topicName,
                 typename Topic<T>::Callback callback) {
    std::unique_lock<std::mutex> lock(provider_mutex);
    auto it = topics.find(topicName);
    if (it == topics.end()) {
      throw std::runtime_error("Topic not found: " + topicName);
    }

    // Cast TopicBase to Topic<T>
    auto topic = dynamic_cast<Topic<T> *>(it->second.get());
    if (!topic) {
      throw std::runtime_error("Topic Type mismatch for: " + topicName +
                               " \nMessage Type not correct");
    }

    topic->subscribe(callback);
  }

  // Publish a message to a topic
  template <typename T>
  void publish(const std::string &topicName, const Message<T> &msg) {
    std::unique_lock<std::mutex> lock(provider_mutex);
    auto it = topics.find(topicName);
    if (it == topics.end()) {
      throw std::runtime_error("Topic not found: " + topicName);
    }

    // Cast TopicBase to Topic<T>
    auto topic = dynamic_cast<Topic<T> *>(it->second.get());
    if (!topic) {
      throw std::runtime_error("Topic Type mismatch for: " + topicName +
                               " \nMessage Type not correct");
    }

    topic->publish(msg);
  }

private:
  std::map<std::string, std::shared_ptr<TopicBase>> topics; // Use TopicBase
  std::mutex provider_mutex;
};