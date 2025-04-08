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
#include <cmath>

template <typename T> 
void printMessage(const Message<T>& msg){
    std::println("Message: {}, {}, ({}, {})", msg.header, msg.timestamp, msg.data->first, msg.data->second);
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
  
    TopicsContainer topics;
    Provider<TopicsContainer> provider(std::move(topics));

    coordinate* coord1 = nullptr;
    Subscriber<coordinate, boost::function<void(Message<coordinate>&)>> setconfig([&coord1, &provider](const Message<coordinate> &msg) {
      std::println("Coordinate recieved");
      if(coord1 != nullptr) {
        std::println("linestring published");
        Message m = Message<lineString>{"linestring", static_cast<size_t>(std::time(nullptr)), new lineString{*coord1, *msg.data}};
        provider.publish("Linestring", m);
      }
      coord1 = msg.data;
    });

    provider.subscribe("Coordinate", setconfig.getAdd());

    //Subscriber that prints out linestrings when they are added. 
    Subscriber<lineString, boost::function<void(Message<lineString>&)>> lineStringSubscriber([](Message<lineString> &msg){
      std::println("linestring recieved");
      std::println("linestring from ({}, {}) to ({},{})", msg.data->first.first, msg.data->first.second, msg.data->second.first, msg.data->second.second);
    });

    provider.subscribe<lineString>("Linestring", lineStringSubscriber.getAdd());


    Message<coordinate> m1{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{1.0f, 1.0f}};
    Message<coordinate> m2{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{1.0f, 1.5f}};
    Message<coordinate> m3{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{1.5f, 1.5f}};
    Message<coordinate> m4{"coordinate", static_cast<size_t>(std::time(nullptr)), new coordinate{2.0f, 2.0f}};

    // Publish the message to the "Coordinate" topic
    provider.publish<coordinate>("Coordinate", m1);
    provider.publish<coordinate>("Coordinate", m2);
    provider.publish<coordinate>("Coordinate", m3);
    provider.publish<coordinate>("Coordinate", m4);
    
    while(!setconfig.isDone()){
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    
    return 0;
}