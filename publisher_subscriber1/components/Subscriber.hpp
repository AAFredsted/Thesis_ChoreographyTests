#include "Message.hpp"
#include <queue>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/chrono.hpp>
#include <print>

template <typename T, typename FN> 
class Subscriber {
public:
    using MessageType = Message<T>;
    using FilterFunc = boost::function<bool(const MessageType&)>;

    Subscriber(FN callback) : callback(callback), stop_flag(false) {
        worker_thread = boost::thread(&Subscriber::processMessages, this);
    }

    ~Subscriber() {
        stop_flag = true;
        cv.notify_all();
        worker_thread.join();
    }

    boost::function<void(MessageType&)> getAdd() {
        std::println("getadd called");
        return [this](MessageType& message) -> void {
            boost::unique_lock<boost::mutex> lock(queue_mutex);
            //std::println("add called");
            _message_queue.push(&message);
            cv.notify_one();
        };
    }   
    boost::function<void(MessageType&)> getFilterAdd(FilterFunc filter) {
        std::println("filteradd called");
        return [= ,this](MessageType& message) -> void {
            if(filter(message)){
                boost::unique_lock<boost::mutex> lock(queue_mutex);
                _message_queue.push(&message);
                cv.notify_one();
            }
        };
    }
    bool isDone() {
        boost::unique_lock<boost::mutex> lock(queue_mutex);
        return _message_queue.empty() && worker_thread.joinable();
    }

private: 
    std::queue<MessageType*> _message_queue;
    FN callback;
    boost::thread worker_thread;
    boost::mutex queue_mutex;
    boost::condition_variable cv;
    bool stop_flag;

    void processMessages() {
        while (!stop_flag) {
            boost::unique_lock<boost::mutex> lock(queue_mutex);
            cv.wait_for(lock, boost::chrono::seconds(1), [this] { return !_message_queue.empty() || stop_flag; });

            while (!_message_queue.empty()) {
                MessageType* message = _message_queue.front();
                _message_queue.pop();
                lock.unlock();
                callback(*message);
                lock.lock();
            }
        }
    }
};