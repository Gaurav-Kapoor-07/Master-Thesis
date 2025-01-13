from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument


def generate_launch_description():

    ground_truth_trajectory_filename = LaunchConfiguration('ground_truth_trajectory_filename', default='ground_truth_trajectory.txt')

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')

    return LaunchDescription([

        DeclareLaunchArgument(
            'ground_truth_trajectory_filename',
            default_value=ground_truth_trajectory_filename,
            description='Name of the output ground truth trajectory file'),

        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='map_to_enu_static_tf',
            namespace='map_to_enu_static_tf',
            parameters=[{'use_sim_time': use_sim_time}],
            arguments = ['0.339', '-0.012', '-0.264', '0.8901179185171081', '0.0', '0.0', 'map', 'enu']
        ),

        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='ground_truth_to_imu_data_frame_static_tf',
            namespace='ground_truth_to_imu_data_frame_static_tf',
            parameters=[{'use_sim_time': use_sim_time}],
            arguments = ['-0.339', '0.012', '0.264', '0.0', '0.0', '0.0', 'ground_truth', 'imu_data_frame']
        ),

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
                 'ParentFrame': 'map',
                 'ChildFrame': 'imu_data_frame'
                }
            ]
        )
    ]) 