#include "components/Message.hpp"
#include "components/Topic.hpp"
#include "components/Provider.hpp"
#include <utility>
#include <ctime>
#include <print>
#include <map>

template <typename T> 
void printMessage(const Message<T>& msg){
    std::println("m1 is: {}, {}, ({}, {})", msg.header, msg.timestamp, (msg.data)->first, (msg.data)->second);
}

void messageCallback(const Message<std::pair<float, float>>& msg) {
    std::println("Recieved Message");
    printMessage(msg);
}

struct TopicsContainer {
    std::map<std::string, std::shared_ptr<void>> topics;
  
    TopicsContainer() {
      topics["Coordinate"] = std::make_shared<Topic<std::pair<float, float>>>("Coordinate");
      topics["linestring"] = std::make_shared<Topic<std::pair<std::pair<float, float>, std::pair<float, float>>>>("linestring");
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

    provider.subscribe<coordinate>("Coordinate", messageCallback);

    // Publish the message to the "Coordinate" topic
    provider.publish<coordinate>("Coordinate", m1);
    provider.publish<coordinate>("Coordinate", m2);
    provider.publish<coordinate>("Coordinate", m3);

    return 0;
}