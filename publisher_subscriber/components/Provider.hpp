#include "Message.hpp"
#include "Topic.hpp"
#include <boost/function.hpp>
#include <string>

template <typename TopicsContainer>
class Provider {
public:
  Provider(TopicsContainer&& topics) : topics_(std::move(topics)) {}

  template <typename T>
  void subscribe(const std::string& topicName, boost::function<void(const Message<T>&)> callback) {
    auto& topic = topics_.template getTopic<T>(topicName);
    topic.subscribe(callback);
  }

  template <typename T>
  void publish(const std::string& topicName, const Message<T>& msg) {
    auto& topic = topics_.template getTopic<T>(topicName);
    topic.add(msg);
  }

private:
  TopicsContainer topics_;
};