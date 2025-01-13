#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <cmath>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>

using std::placeholders::_1;

using namespace std;
using namespace GeographicLib;

class GpsSubscriber : public rclcpp::Node
{
  public:
    GpsSubscriber()
    : Node("gps_subscriber") 
    , earth(Constants::WGS84_a(), Constants::WGS84_f()) 
    , proj(48.365952318333335, 9.940394193333333, 524.365, earth)
    {  
      subscription_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(
      "fix", 10, std::bind(&GpsSubscriber::gps_callback, this, _1));
      tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    }

  private:
    
    Geocentric earth;
    LocalCartesian proj;
    double x{0.0};
    double y{0.0};
    double z{0.0}; 
    
    void gps_callback(const sensor_msgs::msg::NavSatFix::SharedPtr gps)
    {
      proj.Forward(gps->latitude, gps->longitude, gps->altitude, x, y, z);

      geometry_msgs::msg::TransformStamped transformStamped;
      transformStamped.header.stamp = gps->header.stamp;
      transformStamped.header.frame_id = "enu";
      transformStamped.child_frame_id = "ground_truth";
      transformStamped.transform.translation.x = x;
      transformStamped.transform.translation.y = y;
      transformStamped.transform.translation.z = z;
      transformStamped.transform.rotation.w = 1.0;
      transformStamped.transform.rotation.x = 0.0;
      transformStamped.transform.rotation.y = 0.0;
      transformStamped.transform.rotation.z = 0.0;     
      tf_broadcaster_->sendTransform(transformStamped);
    }

    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr subscription_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GpsSubscriber>());
  rclcpp::shutdown();
  return 0;
}