#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <map>
#include <print>
#include <string>
namespace kobuki {
enum class State {
  IDLE,
  DRIVE,
  TWIST,
  STOP,
  DONE,
};
enum class Command { IDLE, TWIST, DRIVE, STOP };

inline std::string to_string(State state) {
    switch (state) {
        case State::IDLE: return "IDLE";
        case State::DRIVE: return "DRIVE";
        case State::TWIST: return "TWIST";
        case State::STOP: return "STOP";
        case State::DONE: return "DONE";
        default: return "UNKNOWN";
    }
}

inline std::string to_string(Command command) {
    switch (command) {
        case Command::IDLE: return "IDLE";
        case Command::TWIST: return "TWIST";
        case Command::DRIVE: return "DRIVE";
        case Command::STOP: return "STOP";
        default: return "UNKNOWN";
    }
}

class Message {
public:
  struct {
    float x;
    float y;
    float z;
  } linear;
  struct {
    float x;
    float y;
    float z;
  } angular;
  Command command;
  std::string topic;

  Message();
  explicit Message(const Message &other);
  explicit Message(Message &&other);

  Message &operator=(const Message &other);
  Message &operator=(Message &&other);

  void reset();
  void print() const;
};

class Driver {

public:
  Driver(): state(State::IDLE), subscribers_() {}
  void subscribe(std::string topic,
                 boost::function<void(const Message &msg)> fn);
  void advertise();
  void publish(const Message &msg);
  void publishAll(const Message &msg);

private:
  State state;
  std::map<std::string, std::vector<boost::function<void(const Message &)>>>
      subscribers_;
};

class Controller {
public:
  void init();
  void spin();
  State getState();
  void publish();
  void setDriver(Driver &driver);
  void setState(State&& state);

private:
  bool wheel_left_dropped_, wheel_right_dropped_;
  bool bumper_left_pressed_, bumper_center_pressed_, bumper_right_pressed_;
  bool cliff_left_detected_, cliff_center_detected_, cliff_right_detected_;
  bool velocity_;
  State state_;
  Message *msg_;
  Driver *driver_;
};

// idea: nodelet contains RAII setup for managing state of Driver and Controller
class Nodelet {

public:
  Nodelet() : shutdown_requested_(false), worker_(&Nodelet::spin, this) {};
  ~Nodelet() {
    shutdown_requested_ = true;
    if (worker_.joinable()) {
      worker_.join();
    }
    std::println("nodelet shut down");
  };

  Nodelet(const Nodelet &other) = delete;
  Nodelet(Nodelet &&other) = delete;
  Nodelet &operator=(const Nodelet &other) = delete;
  Nodelet &operator=(Nodelet &&other) = delete;
  void update();
  void shutdown();

private:
  void spin();

  Controller controller_;
  Driver driver_;
  bool shutdown_requested_;
  boost::thread worker_;

  // wrapper for threads that contains driver and subscribes to other threads
  // and advertises availability
};
} // namespace kobuki
