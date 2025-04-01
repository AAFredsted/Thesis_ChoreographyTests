#include "api.hpp"
#include <boost/function.hpp>

namespace kobuki {
void Driver::subscribe(std::string topic,
                       boost::function<void(const Message &msg)> fn) {
  auto search = subscribers_.find(topic);
  if (search == subscribers_.end()) {
    subscribers_.insert(
        {topic, std::vector<boost::function<void(const Message &msg)>>{fn}});
  } else {
    search->second.push_back(fn);
  }
}

void Driver::publish(const Message &msg) {
  auto search = subscribers_.find(msg.topic);
  if (search != subscribers_.end()) {
    for (const auto &callback : search->second) {
      callback(msg);
    }
  }
}

void Driver::publishAll(const Message &msg) {
  for (const auto &[topic, callbacks] : subscribers_) {
    for (const auto &callback : callbacks) {
      callback(msg);
    }
  }
}
void Driver::advertise() {
   std::println("Driver is advertising topics...");
}
} // namespace kobuki
