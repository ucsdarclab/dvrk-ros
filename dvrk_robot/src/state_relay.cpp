//
// Created by florian on 1/9/20.
//

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs/JointState.h>
#include <stdio.h>

std::string lastStatePublished;
geometry_msgs::Pose lastPosePublished;
sensor_msgs::JointState lastJawRecieved;

ros::Publisher state_pub;
ros::Publisher cartesian_pub;
ros::Publisher jaw_pub;

bool arePosesEqual(geometry_msgs::Pose pose1, geometry_msgs::Pose pose2){
    if (pose1.position.x != pose2.position.x){
        return false;
    }
    if (pose1.position.y != pose2.position.y){
        return false;
    }
    if (pose1.position.z != pose2.position.z){
        return false;
    }
    if (pose1.orientation.x != pose2.orientation.x){
        return false;
    }
    if (pose1.orientation.y != pose2.orientation.y){
        return false;
    }
    if (pose1.orientation.z != pose2.orientation.z){
        return false;
    }
    if (pose1.orientation.w != pose2.orientation.w){
        return false;
    }
    return true;
}

void stateCallback(const std_msgs::String::ConstPtr& msg){
    if(msg->data != lastStatePublished){
        state_pub.publish(msg);
        lastStatePublished = msg->data;
    }
}

void cartesianCallback(const geometry_msgs::Pose::ConstPtr& msg){
    if(lastStatePublished == "READY" && !arePosesEqual(lastPosePublished, *msg)){

        cartesian_pub.publish(msg);
        jaw_pub.publish(lastJawRecieved);

        lastPosePublished = *msg;
    }
}

void jawCallback(const sensor_msgs::JointState::ConstPtr& msg){
    lastJawRecieved = *msg;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "state_relay");
    ros::NodeHandle n;

    if(argc != 7){
        std::cerr << "Failed to launch state_relay, requires 6 arguments" << std::endl;
        std::cerr << "  -- Arg 1 = subscribed state topic name" << std::endl;
        std::cerr << "  -- Arg 2 = to publish state topic name" << std::endl;
        std::cerr << "  -- Arg 3 = subscribed cartesian topic name" << std::endl;
        std::cerr << "  -- Arg 4 = to publish cartesian topic name" << std::endl;
        std::cerr << "  -- Arg 5 = subscribed jaw topic name" << std::endl;
        std::cerr << "  -- Arg 6 = to publish jaw topic name" << std::endl;
        return -1;
    }

    std::string state_subscribe_topic = argv[1];
    std::string state_publish_topic   = argv[2];

    std::string cart_subscribe_topic = argv[3];
    std::string cart_publish_topic   = argv[4];

    std::string jaw_subscribe_topic = argv[5];
    std::string jaw_publish_topic   = argv[6];


    state_pub     = n.advertise<std_msgs::String>   (state_publish_topic, 10);
    cartesian_pub = n.advertise<geometry_msgs::Pose>(cart_publish_topic,  10);
    jaw_pub       = n.advertise<sensor_msgs::JointState>(jaw_publish_topic, 10);

    ros::Subscriber state_sub = n.subscribe(state_subscribe_topic, 10, stateCallback);
    ros::Subscriber cart_sub  = n.subscribe(cart_subscribe_topic,  10, cartesianCallback);
    ros::Subscriber jaw_sub   = n.subscribe(jaw_subscribe_topic,   10, jawCallback);

    ros::spin();

    return 0;
}