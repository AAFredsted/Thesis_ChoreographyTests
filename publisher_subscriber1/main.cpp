#include "components/Message.hpp"
#include "components/Topic.hpp"
#include "components/Provider.hpp"
#include "components/Subscriber.hpp"
#include <utility>
#include <ctime>
#include <print>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <map>

template <typename T> 
void printMessage(const Message<T>& msg){
    std::println("Message: {}, {}, ({}, {})", msg.header, msg.timestamp, msg.data->first, msg.data->second);
}

void messageCallback(Message<std::pair<float, float>>& msg) {
    std::println("Received Message");
    printMessage(msg);
}

void filterMessageCallback(Message<std::pair<float, float>>& msg) {
  std::println("Filtered Message Recieved");
  printMessage(msg);
}

struct TopicsContainer {
  std::map<std::string, std::shared_ptr<void>> topics;

  TopicsContainer() {
    topics["Coordinate"] = std::make_shared<Topic<std::pair<float, float>>>("Coordinate");
    topics["Linestring"] = std::make_shared<Topic<std::pair<std::pair<float, float>, std::pair<float, float>>>>("Linestring");
  }

  template <typename T>
  Topic<T>& getTopic(const std::string& topicName) {
    auto it = topics.find(topicName);
    if (it == topics.end()) {
      throw std::runtime_error("Topic not found");
    }
    return *std::static_pointer_cast<Topic<T>>(it->second);
  }
};


int main() {
    using coordinate = std::pair<float, float>;
    using lineString = std::pair<coordinate, coordinate>;

    Message<coordinate> m1{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{1.2, 1.3}};
    Message<coordinate> m2{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{1.4, 1.5}};
    Message<coordinate> m3{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{2, 1.3}};

    TopicsContainer topics;
    Provider<TopicsContainer> provider(std::move(topics));

    // Create a subscriber
    Subscriber<coordinate, boost::function<void(Message<coordinate>&)>> subscriber(messageCallback);
    Subscriber<coordinate, boost::function<void(Message<coordinate>&)>> filtersubscriber(filterMessageCallback);

    
    // Subscribe to the "Coordinate" topic using the subscriber's getAdd method
    provider.subscribe<coordinate>("Coordinate", subscriber.getAdd());
    provider.subscribe<coordinate>("Coordinate", filtersubscriber.getFilterAdd([](const Message<coordinate>& msg) {
      float x = msg.data->first;
      float y = msg.data->second;

      msg.data->first +=1;
      return x > 1 && x < 2 && y > 1 && y < 2;
  }));


    // Publish the message to the "Coordinate" topic
    provider.publish<coordinate>("Coordinate", m1);
    provider.publish<coordinate>("Coordinate", m2);
    provider.publish<coordinate>("Coordinate", m3);

    while(!subscriber.isDone() && !filtersubscriber.isDone()){
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    return 0;
}