#pragma once
#include "Message.hpp"
#include <chrono>
#include <condition_variable>
#include <functional>
#include <print>
#include <queue>
#include <thread>

template <typename T, typename FN> class Subscriber {
public:
  using MessageType = Message<T>;
  using FilterFunc = std::function<bool(const MessageType &)>;

  Subscriber(FN callback) : callback(callback), stop_flag(false) {
    worker_thread = std::jthread(&Subscriber::processMessages, this);
  }

  ~Subscriber() {
    stop_flag = true;
    cv.notify_all();
  }

  std::function<void(const MessageType &)> getAdd() {
    return [this](const MessageType &message) {
      std::unique_lock<std::mutex> lock(queue_mutex);
      message_queue.push(message);
      cv.notify_one();
    };
  }

  std::function<void(const MessageType &)>
  getFilterAdd(std::function<bool(const MessageType &)> filter) {
    return [this, filter](const MessageType &message) {
      if (filter(message)) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        message_queue.push(message);
        cv.notify_one();
      }
    };
  }

  bool isDone() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return message_queue.empty();
  }

private:
  std::queue<MessageType> message_queue;
  FN callback;
  std::jthread worker_thread;
  std::mutex queue_mutex;
  std::condition_variable cv;
  bool stop_flag;

  void processMessages() {
    std::println("Subscriber has started processing Messages");
    while (!stop_flag) {
      std::unique_lock<std::mutex> lock(queue_mutex);
      cv.wait_for(lock, std::chrono::milliseconds(700),
                  [this] { return !message_queue.empty() || stop_flag; });

      while (!message_queue.empty()) {
        MessageType message = std::move(message_queue.front());
        message_queue.pop();
        lock.unlock();
        callback(message);
        lock.lock();
      }
    }
  }
};