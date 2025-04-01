#include "components/api.hpp"

void callback(const kobuki::Message& msg) {
    msg.print();
}

int main() {
    kobuki::Message a{};
    kobuki::Driver d;
    d.subscribe("event/global", boost::bind(callback, _1));
    d.publish(a);

    kobuki::Controller c;
    c.setDriver(d);
    c.init();
    c.setState(kobuki::State::DRIVE);
    c.spin();
    std::println("{}", kobuki::to_string(c.getState()));

    kobuki::Nodelet n;
    n.update();
    boost::this_thread::sleep_for(boost::chrono::seconds(2));
    n.shutdown(); 
}