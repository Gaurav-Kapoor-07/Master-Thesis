import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import time

class VelocityPublisher(Node):

    def __init__(self):

        super().__init__('velocity_publisher')
        self.v = 0.0
        self.omega = 0.0
        self.publisher_ = self.create_publisher(Twist, 'cmd_vel', 10)
        timer_period = 0.01  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)

    def timer_callback(self):

        vel = Twist()
        vel.linear.x = self.v
        vel.angular.z = self.omega
        self.publisher_.publish(vel)
        self.get_logger().info('Publishing v = %f and omega = %f' % (self.v, self.omega))


def main(args=None):

	rclpy.init(args=args)
	vel_publisher = VelocityPublisher()
	
	scale = 1000

	for i in range(scale):
		vel_publisher.v += 1/scale
		vel_publisher.omega = 0.0
		rclpy.spin_once(vel_publisher)
	
	t = time.time()
	
	while time.time() - t <= 10:
		rclpy.spin_once(vel_publisher)
	
	for i in range(scale):
		vel_publisher.v -= 1/scale
		vel_publisher.omega = 0.0
		rclpy.spin_once(vel_publisher)

	vel_publisher.destroy_node()
	rclpy.shutdown()

if __name__ == '__main__':    
    main()
