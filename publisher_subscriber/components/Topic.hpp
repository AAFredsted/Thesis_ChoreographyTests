#pragma once
#include "Message.hpp"
#include <boost/function.hpp>
#include <string>
#include <vector>

template <typename T>
class Topic {
public:
  using MessageType = Message<T>;
  using Callback = boost::function<void(const MessageType&)>;

  explicit Topic(const std::string& name)
      : _name(name), _messages(), _subscribers() {}

  explicit Topic(std::string&& name)
      : _name(std::move(name)), _messages(), _subscribers() {}

  Topic(const Topic<T>& other) = delete;
  Topic<T>& operator=(const Topic<T>& other) = delete;

  Topic(Topic<T>&& other) noexcept
      : _name(std::move(other._name)), _messages(std::move(other._messages)), _subscribers(std::move(other._subscribers)) {}

  Topic<T>& operator=(Topic<T>&& other) noexcept {
    if (this != &other) {
      _name = std::move(other._name);
      _messages = std::move(other._messages);
      _subscribers = std::move(other._subscribers);
    }
    return *this;
  }

  ~Topic() = default;

  void add(const MessageType& msg) {
    _messages.push_back(msg);
    for (auto& cb : _subscribers) {
      cb(msg);
    }
  }

  void add(MessageType&& msg) {
    _messages.emplace_back(std::move(msg));
    for (auto& cb : _subscribers) {
      cb(msg);
    }
  }

  void subscribe(Callback callback) {
    _subscribers.push_back(callback);
  }

  MessageType& operator[](size_t i) {
    return _messages[i];
  }

  const MessageType& operator[](size_t i) const {
    return _messages[i];
  }

  typename std::vector<MessageType>::iterator begin() {
    return _messages.begin();
  }

  typename std::vector<MessageType>::iterator end() {
    return _messages.end();
  }

  typename std::vector<MessageType>::const_iterator cbegin() const {
    return _messages.cbegin();
  }

  typename std::vector<MessageType>::const_iterator cend() const {
    return _messages.cend();
  }

  std::string getName() const {
    return _name;
  }

  void clear() {
    _messages.clear();
  }

  void erase(size_t i) {
    _messages.erase(_messages.begin() + i);
  }

private:
  std::string _name;
  std::vector<MessageType> _messages;
  std::vector<Callback> _subscribers;
};