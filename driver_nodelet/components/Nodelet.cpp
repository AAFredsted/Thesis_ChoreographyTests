#include "api.hpp"

namespace kobuki {

void Nodelet::spin() {
  std::println("Nodelet is spinning");
  while (!shutdown_requested_) {
    // find some code to go here!
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
  }
  std::println("Nodelet exiting spinning");
}

void Nodelet::update() {
  std::println("Nodelet starting up");
  driver_.advertise();
  controller_.setDriver(driver_);
  controller_.init();

  driver_.subscribe("disable", [this](const Message &msg) {
    std::println("Nodelet disable triggered\nMessage is: {}", msg.topic);
  });
  driver_.subscribe("events/bumper", [this](const Message &msg) {
    std::println("Bumper event callback triggered");
    controller_.spin();
  });
}
void Nodelet::shutdown() {
    std::println("Nodelet shutting down...");
    shutdown_requested_ = true;
}
} // namespace kobuki
