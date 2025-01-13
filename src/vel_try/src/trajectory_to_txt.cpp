#include <geometry_msgs/msg/transform_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tf2/exceptions.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h> 
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

using std::placeholders::_1;
using namespace std::chrono_literals;
using namespace std;

class TrajectoryListener : public rclcpp::Node
{
public:
  TrajectoryListener()
  : Node("trajectory_listener")
  {
    this->declare_parameter<std::string>("filename", "trajectory.txt");
    this->get_parameter("filename", filename);
    this->declare_parameter<std::string>("ParentFrame", "enu");
    this->get_parameter("ParentFrame", ParentFrame);
    this->declare_parameter<std::string>("ChildFrame", "gps");
    this->get_parameter("ChildFrame", ChildFrame);
    trajectory.open(filename);
    trajectory << "#" << " " << "filename" << " " << "=" << " " << filename << std::endl;
    trajectory << "#" << " " << "ParentFrame" << " " << "=" << " " << ParentFrame << std::endl;
    trajectory << "#" << " " << "ChildFrame" << " " << "=" << " " << ChildFrame << std::endl;
    trajectory << "#" << " " << "timestamp" << " " << "x" << " " << "y" << " " << "z" << " " << "q_x" << " " << "q_y" << " " << "q_z" << " " << "q_w" << std::endl;
    tf_buffer_ =
      std::make_unique<tf2_ros::Buffer>(this->get_clock());
    transform_listener_ =
      std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    timer_ = this->create_wall_timer(
      1s, std::bind(&TrajectoryListener::on_timer, this));
  }

  ~TrajectoryListener() 
  {
    trajectory.close();
  };

private:

  std::string filename;
  std::string ParentFrame;
  std::string ChildFrame;
  double sec{0.0};
  double nanosec{0.0};
  double timestamp{0.0};
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double q_x{0.0};
  double q_y{0.0};
  double q_z{0.0};
  double q_w{1.0};
  
  ofstream trajectory;

  void on_timer()
  {
    geometry_msgs::msg::TransformStamped transformStamped;
    
    try { 
    transformStamped = tf_buffer_->lookupTransform(
    ParentFrame, ChildFrame,
    tf2::TimePointZero);
    } catch (tf2::TransformException & ex) {
      return;
    }
          
    sec = transformStamped.header.stamp.sec*1.0;
    nanosec = transformStamped.header.stamp.nanosec*1.0*pow(10.0,-9.0);
    timestamp = sec + nanosec;
    x = transformStamped.transform.translation.x;
    y = transformStamped.transform.translation.y;
    z = transformStamped.transform.translation.z;

    trajectory << std::fixed << std::setprecision(9) << timestamp << " " << x << " " << y << " " << z << " " << q_x << " " << q_y << " " << q_z << " " << q_w << std::endl;
  }

  rclcpp::TimerBase::SharedPtr timer_{nullptr};
  std::shared_ptr<tf2_ros::TransformListener> transform_listener_{nullptr};
  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TrajectoryListener>());
  rclcpp::shutdown();
  return 0;
}