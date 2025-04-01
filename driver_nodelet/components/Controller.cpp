#include "api.hpp"
namespace kobuki {
    void Controller::init (){
        wheel_left_dropped_ = false;
        wheel_right_dropped_ = false;
        bumper_left_pressed_ = false;
        bumper_center_pressed_ = false;
        bumper_right_pressed_ = false;
        cliff_left_detected_ = false;
        cliff_center_detected_ = false;
        cliff_right_detected_ = false;
        velocity_ = false;

        state_ = State::IDLE;
        msg_ = new Message{};
        std::println("Contoller Initialized");
    }
    void Controller::setDriver(Driver& driver){
        driver_ = &driver;
    }

    void Controller::spin() {
        if(this -> getState() != State::STOP){
            if(velocity_ && wheel_left_dropped_ && wheel_right_dropped_) {
                msg_->reset();
                msg_->linear.x = 0.0;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = 0.0;
                msg_->command = Command::DRIVE;
            }
            else if(wheel_left_dropped_ || wheel_right_dropped_ ) {
                msg_->reset();
                msg_->linear.x = 0.0;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = 0.0;
                msg_->command = Command::TWIST;

            }
            else if (bumper_center_pressed_ || cliff_center_detected_ ){
                msg_->reset();
                msg_->linear.x = -0.1;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = 0.0;
                msg_->command = Command::TWIST;
            }
            else if (bumper_left_pressed_ || cliff_left_detected_) {
                msg_->reset();
                msg_->linear.x = -0.1;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = -0.4;
                msg_->command = Command::TWIST;
            }
            else if (bumper_right_pressed_ || cliff_right_detected_) {
                msg_->reset();
                msg_->linear.x = -0.1;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = 0.4;
                msg_->command = Command::TWIST;
            }
            else {
                msg_->reset();
                msg_->linear.x = 0.0;
                msg_->linear.y = 0.0;
                msg_->linear.z = 0.0;
                msg_->angular.x = 0.0;
                msg_->angular.y = 0.0;
                msg_->angular.z = 0.0;
                msg_->command = Command::IDLE;
            }
            driver_ -> publish(*msg_);
            std::println("Command Published of type {}", kobuki::to_string(msg_->command));
        }

    }
    State Controller::getState() {
        return this->state_;
    }
    void Controller::setState(State &&state){
        this->state_ = state;

        wheel_left_dropped_ = false;
        wheel_right_dropped_ = false;
        bumper_left_pressed_ = false;
        bumper_center_pressed_ = false;
        bumper_right_pressed_ = false;
        cliff_left_detected_ = false;
        cliff_center_detected_ = false;
        cliff_right_detected_ = false;
        velocity_ = false;

        switch(state){
            case State::IDLE: 
                std::println("Controller State Idle");
                break;
            case State::DRIVE: 
                std::println("Controller State Drive");
                velocity_ = true;
                wheel_left_dropped_ = true;
                wheel_right_dropped_ = true;
                break;
            case State::TWIST: 
                bumper_right_pressed_= true;
                break;
        }
    }


}
