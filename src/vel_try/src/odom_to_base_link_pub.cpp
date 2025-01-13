#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

using std::placeholders::_1;

class OdomToBaseLinkPublisher : public rclcpp::Node
{
public:
  OdomToBaseLinkPublisher() 
  : Node("odom_to_base_link_publisher")
  {
    subscription_ = this->create_subscription<nav_msgs::msg::Odometry>("odom", 10, std::bind(&OdomToBaseLinkPublisher::odom_tracks_subscriber_callback, this, _1));
    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
  }

private:

  void odom_tracks_subscriber_callback(const nav_msgs::msg::Odometry::SharedPtr Odometry_Tracks)
  {
    geometry_msgs::msg::TransformStamped transformStamped;
    transformStamped.header = Odometry_Tracks->header;
    transformStamped.child_frame_id = Odometry_Tracks->child_frame_id;
    transformStamped.transform.translation.x = Odometry_Tracks->pose.pose.position.x;
    transformStamped.transform.translation.y = Odometry_Tracks->pose.pose.position.y;
    transformStamped.transform.translation.z = Odometry_Tracks->pose.pose.position.z;
    transformStamped.transform.rotation.w = Odometry_Tracks->pose.pose.orientation.w;
    transformStamped.transform.rotation.x = Odometry_Tracks->pose.pose.orientation.x;
    transformStamped.transform.rotation.y = Odometry_Tracks->pose.pose.orientation.y;
    transformStamped.transform.rotation.z = Odometry_Tracks->pose.pose.orientation.z;     
    tf_broadcaster_->sendTransform(transformStamped);
  }

  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<OdomToBaseLinkPublisher>());
  rclcpp::shutdown();
  return 0;
}