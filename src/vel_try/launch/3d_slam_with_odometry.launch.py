import os
from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.substitutions import ThisLaunchFileDir


def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    
    cartographer_prefix = get_package_share_directory(
        'vel_try')
    
    cartographer_config_dir = LaunchConfiguration(
        'cartographer_config_dir',
        default=os.path.join(cartographer_prefix, 'config'))
    
    configuration_basename = LaunchConfiguration(
        'configuration_basename', default='3d_slam_with_odometry.lua')

    urdf_file_name = 'urdf_file.urdf.xml'
    
    urdf = os.path.join(
        get_package_share_directory('vel_try'), 'urdf', 
        urdf_file_name)
    
    with open(urdf, 'r') as infp:
        robot_desc = infp.read()

    return LaunchDescription([

        Node(
            package='cartographer_ros',
            executable='cartographer_node',
            name='cartographer_node',
            output='screen',
            parameters=[{'use_sim_time': use_sim_time}],
            arguments=['-configuration_directory', cartographer_config_dir,
                       '-configuration_basename', configuration_basename],
            remappings=[('points2', 'points')]),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([ThisLaunchFileDir(),
                                           '/occupancy_grid.launch.py']),
        ),

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{'use_sim_time': use_sim_time, 'robot_description': robot_desc}],
            arguments=[urdf]),

        Node(
            package='vel_try',
            executable='talker',
            name='odom_to_base_link_publisher',
        )
    ])