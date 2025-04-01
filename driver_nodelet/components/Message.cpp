#include "api.hpp"
namespace kobuki {

Message::Message()
    : linear({0.0f, 0.0f, 0.0f}), angular({0.0f, 0.0f, 0.0f}),
      command(Command::IDLE), topic("event/global") {}
Message::Message(const Message &other)
    : linear(other.linear), angular(other.angular), command(other.command),
      topic(other.topic) {}
Message::Message(Message &&other)
    : linear(std::move(other.linear)), angular(std::move(other.angular)),
      command(std::move(other.command)), topic(std::move(other.topic)) {}

Message &Message::operator=(const Message &other) {
  if (this != &other) {
    linear = other.linear;
    angular = other.angular;
    command = other.command;
    topic = other.topic;
  }
  return *this;
}
Message &Message::operator=(Message &&other) {
  if (this != &other) {
    linear = std::move(other.linear);
    angular = std::move(other.angular);
    command = std::move(other.command);
    topic = std::move(other.topic);
    other.reset();
  }
  return *this;
}
void Message::reset() {
  linear.x = 0.0f;
  linear.y = 0.0f;
  linear.z = 0.0f;
  angular.x = 0.0f;
  angular.y = 0.0f;
  angular.z = 0.0f;
  command = Command::IDLE;
  topic = "event/global";
}

void Message::print() const {
  std::println("Message:");
  std::println("  Linear: x = {}, y = {}, z = {}", linear.x, linear.y,
               linear.z);
  std::println("  Angular: x = {}, y = {}, z = {}", angular.x, angular.y,
               angular.z);
  std::println("  Command: {}", static_cast<int>(command));
  std::println("  Topic: {}", topic);
}
} // namespace kobuki
