#ifndef _COMMAND_HANDLER_H_
#define _COMMAND_HANDLER_H_

#include <ros/ros.h>
#include <std_msgs/String.h>


namespace lcr_cal
{

    class CommandHandler
    {
        public:
            CommandHandler();
            CommandHandler(ros::NodeHandle node_handle, std::string subscribe_topic, std::string publish_topic);
            void sendCommand(const std::string &content);
            std::string getCommand() const;  
            void resetReceivedStatus();

        private:
            void CommandCallback(const std_msgs::String::ConstPtr& msg);
            ros::NodeHandle nh_;
            ros::Publisher pub_;
            ros::Subscriber sub_;
            std::string latest_command_;  
            bool received_status_;
    };


    using namespace lcr_cal;


    CommandHandler::CommandHandler()
    {

        nh_ = ros::NodeHandle("CommandHandler");

        // Initialize publisher
        pub_ = nh_.advertise<std_msgs::String>("command_topic", 1000);

        // Initialize subscriber
        sub_ = nh_.subscribe("command_topic", 1000, &CommandHandler::CommandCallback, this);

        received_status_ = false;
    }
    CommandHandler::CommandHandler(ros::NodeHandle node_handle, std::string subscribe_topic, std::string publish_topic)
    {
        nh_ = node_handle;

        // Initialize subscriber
        sub_ = nh_.subscribe(subscribe_topic, 1000, &CommandHandler::CommandCallback, this);

        // Initialize publisher
        pub_ = nh_.advertise<std_msgs::String>(publish_topic, 1000);

        received_status_ = false;
    }

    void CommandHandler::sendCommand(const std::string &content)
    {
        std_msgs::String msg;
        msg.data = content;
        pub_.publish(msg);
        ROS_INFO("Sent command: %s", content.c_str());
    }

    void CommandHandler::CommandCallback(const std_msgs::String::ConstPtr& msg)
    {
        // latest_command_ = msg->data;
        // ROS_INFO("Received command: %s", msg->data.c_str());
        // this->received_status_ = true;
        latest_command_ = msg->data;
        // ROS_INFO("Received command: %s", latest_command_.c_str());
        this->received_status_ = true;

        // Debugging output
        if (msg->data == "capture_complete") {
            ROS_INFO("Capture complete received");
        }
    }

    std::string CommandHandler::getCommand() const
    {
        return latest_command_;
    }

    void CommandHandler::resetReceivedStatus()
    {
        this->latest_command_ = "";
        this->received_status_ = false;
    }

}



#endif