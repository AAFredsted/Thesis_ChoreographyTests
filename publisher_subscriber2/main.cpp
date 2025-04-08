#include "./components/Message.hpp"
#include "./components/Provider.hpp"
#include "./components/Subscriber.hpp"
#include "./components/Topic.hpp"

#include <chrono>
#include <ctime>
#include <functional>
#include <print>
#include <thread>

int main() {
  using coordinate = std::pair<float, float>;
  using lineString = std::pair<coordinate, coordinate>;

  Provider provider{};

  // Advertise topics
  provider.advertise<coordinate>("input/coordinate");
  provider.advertise<lineString>("output/lineString");

  // Use std::shared_ptr for previous coordinate
  std::shared_ptr<coordinate> previouscoor = nullptr;

  // Subscriber for "input/coordinate" topic
  Subscriber<coordinate, std::function<void(const Message<coordinate> &msg)>>
      subscriber{[&previouscoor, &provider](const Message<coordinate> &msg) {
        std::println("Received Coordinate: ({}, {})", msg.data->first,
                     msg.data->second);

        if (previouscoor != nullptr) {
          // Create a lineString using the previous coordinate and the current
          // one
          lineString line = {*previouscoor, *msg.data};
          std::println("Generated lineString: (({}, {}), ({}, {}))",
                       line.first.first, line.first.second, line.second.first,
                       line.second.second);

          // Publish the lineString to the "output/lineString" topic
          Message<lineString> lineMsg("output/lineString", std::time(nullptr),
                                      std::move(line));
          provider.publish<lineString>("output/lineString", lineMsg);
        }

        // Update the previous coordinate
        previouscoor = msg.data; // Share ownership of the coordinate
      }};

  // Subscribe the coordinate subscriber to the "input/coordinate" topic
  provider.subscribe<coordinate>("input/coordinate", subscriber.getAdd());

  // Use std::shared_ptr for previous coordinate
  std::shared_ptr<coordinate> previouscoorfilter = nullptr;

  // Subscriber for "input/coordinate" topic with filtering logic
  Subscriber<coordinate, std::function<void(const Message<coordinate> &msg)>>
      filterSubscriber{
          [&previouscoorfilter, &provider](const Message<coordinate> &msg) {
            std::println("FilterSubscriber Received Coordinate: ({}, {})",
                         msg.data->first, msg.data->second);

            if (previouscoorfilter != nullptr) {
              // Create a lineString using the previous coordinate and the
              // current one
              lineString line = {*previouscoorfilter, *msg.data};
              std::println("Generated lineString: (({}, {}), ({}, {}))",
                           line.first.first, line.first.second,
                           line.second.first, line.second.second);

              // Publish the lineString to the "output/lineString" topic
              Message<lineString> lineMsg("output/lineString",
                                          std::time(nullptr), std::move(line));
              provider.publish<lineString>("output/lineString", lineMsg);
              previouscoorfilter = nullptr;
            }

            // Update the previous coordinate
            previouscoorfilter = msg.data; // Share ownership of the coordinate
          }};

  // Subscribe the coordinate subscriber to the "input/coordinate" topic with
  // filtering logic
  provider.subscribe<coordinate>(
      "input/coordinate",
      filterSubscriber.getFilterAdd([](const Message<coordinate> &msg) {
        std::println("Filter running for Message:");
        msg.print();
        // Filter: Only process coordinates within a specific area
        return (msg.header == "start" || msg.header == "end");
      }));

  // Subscriber for "output/lineString" topic
  Subscriber<lineString, std::function<void(const Message<lineString> &msg)>>
      lineStringSubscriber{[](const Message<lineString> &msg) {
        const auto &[start, end] = *msg.data;
        std::println("Received lineString: (({}, {}), ({}, {}))", start.first,
                     start.second, end.first, end.second);
      }};

  // Subscribe the lineString subscriber to the "output/lineString" topic
  provider.subscribe<lineString>("output/lineString",
                                 lineStringSubscriber.getAdd());

  // Simulate publishing coordinates
  provider.publish<coordinate>("input/coordinate",
                               Message<coordinate>("start",
                                                   std::time(nullptr),
                                                   {1.0f, 1.0f}));
  provider.publish<coordinate>("input/coordinate",
                               Message<coordinate>("onpath",
                                                   std::time(nullptr),
                                                   {2.0f, 2.0f}));
  provider.publish<coordinate>("input/coordinate",
                               Message<coordinate>("onpath",
                                                   std::time(nullptr),
                                                   {3.0f, 3.0f}));
  provider.publish<coordinate>("input/coordinate",
                               Message<coordinate>("end",
                                                   std::time(nullptr),
                                                   {4.0f, 4.0f}));

  // Wait for subscribers to process messages
  while (!subscriber.isDone() || !lineStringSubscriber.isDone()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(700));
  }

  return 0;
}