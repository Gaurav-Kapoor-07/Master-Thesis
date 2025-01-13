import os
from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():

    config_directory = os.path.join(
        get_package_share_directory('vel_try'),
        'config')

    params = os.path.join(config_directory, 'joy-params.yaml')

    scale_v = LaunchConfiguration('scale_v', default='1.0')

    scale_omega = LaunchConfiguration('scale_omega', default='1.0') 

    return LaunchDescription([

        DeclareLaunchArgument(
            'scale_v',
            default_value=scale_v,
            description='Scale linear velocity from left analog trigger. Value range +/- scale_v m/s.'),

        DeclareLaunchArgument(
            'scale_omega',
            default_value=scale_omega,
            description='Scale angular velocity from right analog trigger. Value range +/- scale_omega rad/s.'),

        Node(
            package='vel_try',
            executable='listener_and_talker',
            name='velocity_subscriber',
            emulate_tty=True,
            parameters=[
                {'scale_v': scale_v, 'scale_omega': scale_omega}
            ]
        ),

        IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('vel_try'),
                'launch/joy-composed-launch.py', 
            )
        )),
        
        IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('ros2_ouster'),
                'launch/driver_launch.py', 
            )
        )),

        IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('ros2_socketcan'),
                'launch/socket_can_receiver.launch.py', 
            )
        )),

        IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('nmea_navsat_driver'),
                'launch/nmea_serial_driver.launch.py', 
            )
        ))
    ])