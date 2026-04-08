#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"
#include "head_interfaces/srv/move_head.hpp"
#include "arms_interfaces/srv/move_arms.hpp"
#include "torso_interfaces/srv/move_torso.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

namespace service_exposer
{

class ServiceExposerNode : public rclcpp::Node
{
public:
    ServiceExposerNode()
    : Node("service_exposer_node"),
      head_goal_reached_(false),
      arm_goal_reached_(false),
      torso_goal_reached_(false)
    {
        //############
        // Publishers
        pub_head_goal_pose_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/hardware/head/goal_pose",
            rclcpp::QoS(10).transient_local());

        pub_arm_goal_pose_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/hardware/arm/goal_pose",
            rclcpp::QoS(10).transient_local());

        pub_torso_goal_pose_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/hardware/torso/goal_pose",
            rclcpp::QoS(10).transient_local());

        //############
        // Subscribers
        sub_head_goal_reached_ = this->create_subscription<std_msgs::msg::Bool>(
            "/hardware/head/goal_reached",
            rclcpp::SensorDataQoS(),
            std::bind(&ServiceExposerNode::callback_head_goal_reached, this, _1));

        sub_arm_goal_reached_ = this->create_subscription<std_msgs::msg::Bool>(
            "/hardware/arm/goal_reached",
            rclcpp::SensorDataQoS(),
            std::bind(&ServiceExposerNode::callback_arm_goal_reached, this, _1));

        sub_torso_goal_reached_ = this->create_subscription<std_msgs::msg::Bool>(
            "/hardware/torso/goal_reached",
            rclcpp::SensorDataQoS(),
            std::bind(&ServiceExposerNode::callback_torso_goal_reached, this, _1));

        //############
        // Services
        srv_move_head_ = this->create_service<head_interfaces::srv::MoveHead>(
            "/hardware/head/move_head",
            std::bind(&ServiceExposerNode::callback_move_head, this, _1, _2));

        srv_move_left_arm_ = this->create_service<arms_interfaces::srv::MoveArms>(
            "/hardware/arms/move_left_arm",
            std::bind(&ServiceExposerNode::callback_move_left_arm, this, _1, _2));

        srv_move_torso_ = this->create_service<torso_interfaces::srv::MoveTorso>(
            "/hardware/torso/move_torso",
            std::bind(&ServiceExposerNode::callback_move_torso, this, _1, _2));

        RCLCPP_INFO(this->get_logger(), "ServiceExposer.-> Node ready. Exposing services:");
        RCLCPP_INFO(this->get_logger(), "ServiceExposer.->   /hardware/head/move_head");
        RCLCPP_INFO(this->get_logger(), "ServiceExposer.->   /hardware/arms/move_left_arm");
        RCLCPP_INFO(this->get_logger(), "ServiceExposer.->   /hardware/torso/move_torso");
    }

private:
    //############
    // State
    bool head_goal_reached_;
    bool arm_goal_reached_;
    bool torso_goal_reached_;

    //############
    // Publishers
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr pub_head_goal_pose_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr pub_arm_goal_pose_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr pub_torso_goal_pose_;

    //############
    // Subscribers
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_head_goal_reached_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_arm_goal_reached_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_torso_goal_reached_;

    //############
    // Services
    rclcpp::Service<head_interfaces::srv::MoveHead>::SharedPtr    srv_move_head_;
    rclcpp::Service<arms_interfaces::srv::MoveArms>::SharedPtr    srv_move_left_arm_;
    rclcpp::Service<torso_interfaces::srv::MoveTorso>::SharedPtr  srv_move_torso_;

    //############
    // Subscriber callbacks
    void callback_head_goal_reached(const std_msgs::msg::Bool::SharedPtr msg)
    {
        head_goal_reached_ = msg->data;
    }

    void callback_arm_goal_reached(const std_msgs::msg::Bool::SharedPtr msg)
    {
        arm_goal_reached_ = msg->data;
    }

    void callback_torso_goal_reached(const std_msgs::msg::Bool::SharedPtr msg)
    {
        torso_goal_reached_ = msg->data;
    }

    //############
    // Service callbacks
    void callback_move_head(
        const std::shared_ptr<head_interfaces::srv::MoveHead::Request> request,
        std::shared_ptr<head_interfaces::srv::MoveHead::Response> response)
    {
        try
        {
            if (request->positions.empty())
            {
                response->success = false;
                response->message = "ServiceExposer.-> [move_head] positions array is empty.";
                RCLCPP_WARN(this->get_logger(), "%s", response->message.c_str());
                return;
            }

            std_msgs::msg::Float64MultiArray msg;
            msg.data = request->positions;
            pub_head_goal_pose_->publish(msg);

            response->success = true;
            response->message = "ServiceExposer.-> [move_head] Goal pose published successfully.";
            RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
        }
        catch (const std::exception &e)
        {
            response->success = false;
            response->message = std::string("ServiceExposer.-> [move_head] Exception: ") + e.what();
            RCLCPP_ERROR(this->get_logger(), "%s", response->message.c_str());
        }
    }

    void callback_move_left_arm(
        const std::shared_ptr<arms_interfaces::srv::MoveArms::Request> request,
        std::shared_ptr<arms_interfaces::srv::MoveArms::Response> response)
    {
        try
        {
            if (request->positions.empty())
            {
                response->success = false;
                response->message = "ServiceExposer.-> [move_left_arm] positions array is empty.";
                RCLCPP_WARN(this->get_logger(), "%s", response->message.c_str());
                return;
            }

            std_msgs::msg::Float64MultiArray msg;
            msg.data = request->positions;
            pub_arm_goal_pose_->publish(msg);

            response->success = true;
            response->message = "ServiceExposer.-> [move_left_arm] Goal pose published successfully.";
            RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
        }
        catch (const std::exception &e)
        {
            response->success = false;
            response->message = std::string("ServiceExposer.-> [move_left_arm] Exception: ") + e.what();
            RCLCPP_ERROR(this->get_logger(), "%s", response->message.c_str());
        }
    }

    void callback_move_torso(
        const std::shared_ptr<torso_interfaces::srv::MoveTorso::Request> request,
        std::shared_ptr<torso_interfaces::srv::MoveTorso::Response> response)
    {
        try
        {
            std_msgs::msg::Float64MultiArray msg;
            msg.data.push_back(request->position);
            pub_torso_goal_pose_->publish(msg);

            response->success = true;
            response->message = "ServiceExposer.-> [move_torso] Goal pose published successfully.";
            RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
        }
        catch (const std::exception &e)
        {
            response->success = false;
            response->message = std::string("ServiceExposer.-> [move_torso] Exception: ") + e.what();
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
