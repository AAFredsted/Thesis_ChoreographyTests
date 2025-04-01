# Thesis_ChoreographyTests
An Aggregation of Mock-Apis representing faulty and correct implementations of a parametrizised choreography describing a communication from a .cor-file. The programme should be able to detect valid and invalid implementations.

Mock-API's and Choreographies are based on documented examples in [ROS](https://github.com/ros/ros) and its extensions from
[The Robust Project](https://github.com/robust-rosin/robust).


Publisher_Subscriber:
Bugid: 0a76d5

The [geometry2](https://github.com/ros/geometry2) extension to ROS has a bug in it's api, where the parsing of a Message sent to a Ros Topic can be handles and modified concurrently by one or more nodelet(thread), creating a race condition.

Driver_Nodelet:
Bugid: 0366a

The [Kobuki_Melodic](https://github.com/yujinrobot/kobuki/) had a bug which was fixed as SHA 03660afb5917366cc6d57567e2f695450fd95feb,
where Nodelets, threads containing a Controller and Driver for a hardware component, could publish a topic and subscribe to other nodelets topics. However, as no dependencies exist between two nodelets sending commands to each other, race conditions can occur. I.E. the left and right bumpers on a vacuum robot are activated at the same time, leading to incompatible rotate commands being sent at the same time, leading to unexpected behavior. In the actual Kokubi Robot, this was not an issue, as Commands were dealt with synchronously. 