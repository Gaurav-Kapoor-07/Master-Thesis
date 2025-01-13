from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument


def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    
    localization_algorithm_trajectory_filename = LaunchConfiguration('localization_algorithm_trajectory_filename', default='localization_algorithm_trajectory.txt')

    return LaunchDescription([

        DeclareLaunchArgument(
            'localization_algorithm_trajectory_filename',
            default_value=localization_algorithm_trajectory_filename,
            description='Name of the output localization algorithm trajectory file'),

        # Node(
        #     package='tf2_ros',
        #     executable='static_transform_publisher',
        #     parameters=[{'use_sim_time': use_sim_time}],
        #     arguments = ['0.339', '-0.012', '-0.264', '0.8901179185171081', '0.0', '0.0', 'map', 'enu']
        # ),

        Node(
            package='vel_try',
            executable='trajectory_listener',
            name='trajectory_listener',
            output='screen',
            emulate_tty=True,
            parameters=[
                {'filename': localization_algorithm_trajectory_filename,
                 'ParentFrame': 'map',
                 'ChildFrame': 'gps'
                 }
            ]
        )
    ]) 