from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument


def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')

    ground_truth_trajectory_filename = LaunchConfiguration('ground_truth_trajectory_filename', default='ground_truth_trajectory.txt')

    return LaunchDescription([

        DeclareLaunchArgument(
            'ground_truth_trajectory_filename',
            default_value=ground_truth_trajectory_filename,
            description='Name of the output ground truth trajectory file'),

        Node(
            package='vel_try',
            executable='gps_listener',
            name='gps_subscriber',
        ),

        Node(
            package='vel_try',
            executable='trajectory_listener',
            name='trajectory_listener',
            output='screen',
            emulate_tty=True,
            parameters=[
                {'filename': ground_truth_trajectory_filename,
                 'ParentFrame': 'enu',
                 'ChildFrame': 'ground_truth'
                }
            ]
        )
    ]) 