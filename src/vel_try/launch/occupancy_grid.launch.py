from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration


def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    
    resolution = LaunchConfiguration('resolution', default='0.05')
    
    publish_period_sec = LaunchConfiguration('publish_period_sec', default='1.0')

    return LaunchDescription([

        Node(
            package='cartographer_ros',
            executable='occupancy_grid_node',
            name='occupancy_grid_node',
            output='screen',
            parameters=[{'use_sim_time': use_sim_time}],
            arguments=['-resolution', resolution, '-publish_period_sec', publish_period_sec])
    ])