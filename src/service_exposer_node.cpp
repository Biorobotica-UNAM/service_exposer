#include <memory>
#include <vector>
 
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"
#include "head_interfaces/srv/move_head.hpp"
 
using std::placeholders::_1;
using std::placeholders::_2;
 
namespace service_exposer
{
 
class ServiceExposerNode : public rclcpp::Node
{
public:
    ServiceExposerNode()
    : Node("service_exposer_node"),
      head_goal_reached_(false)
    {
        // Publisher → hardware head goal pose
        pub_head_goal_pose_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/hardware/head/goal_pose",
            rclcpp::QoS(10).transient_local());
 
        // Subscriber → hardware head goal reached
        sub_head_goal_reached_ = this->create_subscription<std_msgs::msg::Bool>(
            "/hardware/head/goal_reached",
            rclcpp::SensorDataQoS(),
            std::bind(&ServiceExposerNode::callback_head_goal_reached, this, _1));
 
        // Service → set head pose
        srv_set_head_pose_ = this->create_service<head_interfaces::srv::MoveHead>(
            "/hardware/head/move_head",
            std::bind(&ServiceExposerNode::callback_set_head_pose, this, _1, _2));
 
        RCLCPP_INFO(this->get_logger(), "ServiceExposer.-> Node ready. Exposing head service.");
    }
 
private:
    // State
    bool head_goal_reached_;
 
    // Publisher
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr pub_head_goal_pose_;
 
    // Subscriber
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_head_goal_reached_;
 
    // Service
    rclcpp::Service<head_interfaces::srv::MoveHead>::SharedPtr srv_set_head_pose_;
 
    // Callback: update head goal reached state
    void callback_head_goal_reached(const std_msgs::msg::Bool::SharedPtr msg)
    {
        head_goal_reached_ = msg->data;
    }
 
    // Service callback: publish goal pose to hardware topic
    void callback_set_head_pose(
        const std::shared_ptr<head_interfaces::srv::MoveHead::Request> request,
        std::shared_ptr<head_interfaces::srv::MoveHead::Response> response)
    {
        try
        {
            if (request->positions.empty())
            {
                response->success = false;
                response->message = "ServiceExposer.-> positions array is empty.";
                RCLCPP_WARN(this->get_logger(), "%s", response->message.c_str());
                return;
            }
 
            std_msgs::msg::Float64MultiArray msg;
            msg.data = request->positions;
            pub_head_goal_pose_->publish(msg);
 
            response->success = true;
            response->message = "ServiceExposer.-> Head goal pose published successfully.";
            RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
        }
        catch (const std::exception &e)
        {
            response->success = false;
            response->message = std::string("ServiceExposer.-> Exception: ") + e.what();
            RCLCPP_ERROR(this->get_logger(), "%s", response->message.c_str());
        }
    }
};
 
} // namespace service_exposer
 
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<service_exposer::ServiceExposerNode>());
    rclcpp::shutdown();
    return 0;
}
