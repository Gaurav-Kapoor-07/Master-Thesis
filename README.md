# Master-Thesis: Comparison of the Different Solutions of a Mobile Robot Outdoor Localization using the ROS 2 Components

The aim of this master thesis was to find and evaluate the already available state-of-the-art 3D lidar-based localization algorithms in ROS 2. Localization is carried out to find the pose, i.e., the position and the orientation of a mobile robot with respect to the earth or a local map. This pose is used in robot navigation applications like agricultural and service robotics. ROS 2 is the successor of ROS 1, where ROS is the abbreviation of the Robot Operating System. 
Localization is of two types - absolute localization and relative localization. In absolute localization, the pose of a robot is determined with respect to the earth coordinate frame in the form of
LLA (latitude, longitude and altitude) or ECEF (earth-centered, earth-fixed) coordinates. This is performed by using GNSS (global navigation satellite system) receivers. In relative localization,
the pose of a robot with respect to a known map is determined. In this master thesis, relative localization is performed where the pose of a robot with respect to a map is determined. This is performed by sensors like lidar, IMU (inertial measurement unit) and the wheel encoders. The environment in which the point cloud map is made and all the 3D localization challenges are tested is the InMach’s parking lot. For comparison of the 3D localization algorithms with the ground truth, the RTK (real-time kinematic) GNSS positions are used.

It is also shown that the GNSS receiver outputs very noisy data when the robot is driven near to the buildings. Therefore in places where the GNSS positions are not accurate, or for places
deprived of GNSS signals like a tunnel or indoor environments, relative localization is required. The sensors used for relative localization like lidar, IMU and the wheel encoders do not require a
globally referenced position and therefore not do not face the problems which are faced by the GNSS receivers as described. Also, a lidar sensor is able to perceive the environment and
measures the distances very accurately, as a laser travels at the speed of light.
