import os

import ament_index_python.packages

from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

import yaml


def generate_launch_description():
    config_directory = os.path.join(
        ament_index_python.packages.get_package_share_directory('vel_try'),
        'config')
    param_config = os.path.join(config_directory, 'joy-params.yaml')
    with open(param_config, 'r') as f:
        params = yaml.safe_load(f)['joy_node']['ros__parameters']

    container = ComposableNodeContainer(
            name='joy_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='joy',
                    plugin='joy::Joy',
                    name='joy_node',
                    parameters=[params])
            ],
            output='both',
    )

    return LaunchDescription([container])