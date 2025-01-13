import os
from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration


def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')

    urdf_file_name = 'urdf_file.urdf.xml'
    
    urdf = os.path.join(
        get_package_share_directory('vel_try'), 'urdf', 
        urdf_file_name)
    
    with open(urdf, 'r') as infp:
        robot_desc = infp.read()

    return LaunchDescription([

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