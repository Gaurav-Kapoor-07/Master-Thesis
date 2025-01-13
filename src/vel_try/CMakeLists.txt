cmake_minimum_required(VERSION 3.5)
project(vel_try)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(GeographicLib REQUIRED)

include_directories(include) 

set(MATTRO_LIB_SRCS src/mattro_rovo2.cpp src/mattro_pdo.cpp src/mattro_util_listener.cpp)

# add_executable(listener src/mattro_vel_sub.cpp ${MATTRO_LIB_SRCS})

add_executable(listener_and_talker src/mattro_vel_pub_and_sub.cpp ${MATTRO_LIB_SRCS})

add_executable(talker src/odom_to_base_link_pub.cpp)

add_executable(trajectory_listener src/trajectory_to_txt.cpp)

add_executable(gps_listener src/gps_listener.cpp)

# ament_target_dependencies(listener rclcpp geometry_msgs nav_msgs)

ament_target_dependencies(listener_and_talker rclcpp geometry_msgs nav_msgs sensor_msgs)

ament_target_dependencies(talker rclcpp geometry_msgs nav_msgs tf2_ros)

ament_target_dependencies(trajectory_listener rclcpp geometry_msgs tf2_ros)

ament_target_dependencies(gps_listener rclcpp geometry_msgs sensor_msgs tf2_ros GeographicLib)

install(TARGETS
  # listener
  listener_and_talker
  talker
  trajectory_listener
  gps_listener
  DESTINATION lib/${PROJECT_NAME})

# Install launch files.
install(DIRECTORY
  launch 
  config
  rviz
  urdf
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()