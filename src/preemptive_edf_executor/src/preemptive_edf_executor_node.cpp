#include <chrono>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std::chrono_literals;

class PreemptiveEDFExecutorNode : public rclcpp::Node
{
public:
    PreemptiveEDFExecutorNode()
        : Node("preemptive_edf_executor_node")
    {
        // Create a publisher
        publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);

        // Create a timer that publishes a message every 500ms
        timer_ = this->create_wall_timer(
            500ms, std::bind(&PreemptiveEDFExecutorNode::timer_callback, this));

        // Create a subscriber
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "topic", 10, std::bind(&PreemptiveEDFExecutorNode::topic_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Preemptive EDF Executor Node started");
    }

private:
    void timer_callback()
    {
        auto message = std_msgs::msg::String();
        message.data = "Hello, world! " + std::to_string(count_++);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        publisher_->publish(message);
    }

    void topic_callback(const std_msgs::msg::String::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    size_t count_ = 0;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PreemptiveEDFExecutorNode>());
    rclcpp::shutdown();
    return 0;
}
