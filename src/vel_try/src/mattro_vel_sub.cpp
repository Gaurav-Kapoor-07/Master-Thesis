#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <vel_try/rovo2.hpp>
// #include <signal.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using namespace std::chrono_literals;

class VelocitySubscriber : public rclcpp::Node
{
public:
  VelocitySubscriber() 
  : Node("velocity_subscriber")
  , m_rovo2()
  , m_init_update ( false )
  {
    m_rovo2.open ( "can0" );
    m_rovo2.start();
    subscription_ = this->create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, std::bind(&VelocitySubscriber::vel_cmd_subscriber_callback, this, _1));
    timer_ = this->create_wall_timer(10ms, std::bind(&VelocitySubscriber::timer_callback, this));
    m_rovo2.get_velocity_async ( std::bind ( &VelocitySubscriber::topic_callback, this, _1, _2, _3 ) );
    publisher_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 10);
  }

private:

  inmach::mattro::Rovo2 m_rovo2;
  rclcpp::Time now;

  double m_wheel_base{0.936};
  double v{0.0};
  double omega{0.0}; 
  double vl{0.0};
  double vr{0.0}; 
  double v_wish{0.0};
  double omega_wish{0.0}; 
  double vl_wish{0.0};
  double vr_wish{0.0};
  double delta_s{0.0};
  double delta_theta{0.0};
  double theta{0.0};
  double x{0.0};
  double y{0.0};
  double z{0.17};
  double v_x{0.0};
  double v_y{0.0};    

  inmach::mattro::timestamp_type m_ts_prev;
  bool m_init_update;

  void topic_callback ( const double vr, const double vl, inmach::mattro::timestamp_type const& ts )
  {
    if ( m_init_update )
    {
      const std::chrono::duration<double> ts_diff = ( ts - m_ts_prev );
      const double dt = ts_diff.count();
      
      v     = ( vr + vl ) / 2;
      omega = ( vr - vl ) / m_wheel_base;

      delta_s     = v     * dt;
      delta_theta = omega * dt;

      x     += delta_s * std::cos ( theta + delta_theta/2 );
      y     += delta_s * std::sin ( theta + delta_theta/2 );
      theta += delta_theta;
      
      v_x = v*std::cos(theta);
      v_y = v*std::sin(theta);

      now = rclcpp::Node::now();

      auto odometry = nav_msgs::msg::Odometry();
      odometry.header.stamp = now; 
      odometry.header.frame_id = "odom"; 
      odometry.child_frame_id = "base_link";    
      odometry.pose.pose.position.x = x;
      odometry.pose.pose.position.y = y;
      odometry.pose.pose.position.z = z;
      odometry.pose.pose.orientation.w = std::cos(theta/2);
      odometry.pose.pose.orientation.x = 0.0;
      odometry.pose.pose.orientation.y = 0.0;
      odometry.pose.pose.orientation.z = std::sin(theta/2);
      odometry.twist.twist.linear.x = v_x;
      odometry.twist.twist.linear.y = v_y;
      odometry.twist.twist.angular.z = omega;
      publisher_->publish(odometry);
    }
    m_ts_prev = ts;
    m_init_update = true;
  }

  void timer_callback()
  {
    m_rovo2.set_velocity(vr_wish,vl_wish);
  }

  void vel_cmd_subscriber_callback(const geometry_msgs::msg::Twist::SharedPtr velocity)
  {
    v_wish = velocity->linear.x;
    omega_wish = velocity->angular.z;
    vl_wish = v_wish - omega_wish*m_wheel_base/2;
    vr_wish = v_wish + omega_wish*m_wheel_base/2;
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;
};

// static std::shared_ptr < VelocitySubscriber > node;

// static bool running = false;

// void h_sig_sigint( int signum )
// {
//   RCLCPP_INFO(node->get_logger(), "Receive signum: %i", signum);
//   running = false;
// }

int main(int argc, char * argv[])
{
  // running = true;
  // signal(SIGINT, h_sig_sigint);
  rclcpp::init(argc, argv);
  // node.reset ( new VelocitySubscriber() );
  // rclcpp::executors::MultiThreadedExecutor executor;
  // executor.add_node ( node );
  // while ( running )
  // {
  //   executor.spin_once ( std::chrono::nanoseconds( 100000000 ) );
  // }
  // RCLCPP_INFO(node->get_logger(), "spin exitted" );
  rclcpp::spin(std::make_shared<VelocitySubscriber>());
  rclcpp::shutdown();
  return 0;
}