import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from .MattroCan import mattro
import time

class VelocitySubscriber(Node):

    def __init__(self):

        super().__init__('velocity_subscriber')
        self.subscription = self.create_subscription(Twist, 'cmd_vel', self.listener_callback, 10)
        self.mattro_can = mattro()
        self.mattro_can.activate()

    def listener_callback(self, msg):

        v = msg.linear.x
        omega = msg.angular.z        
        self.mattro_can.move_rot(v, omega)


def main(args=None):

    rclpy.init(args=args)
    minimal_subscriber = VelocitySubscriber()  
    t = time.time()
    while time.time() - t <= 22: 
       rclpy.spin_once(minimal_subscriber)
    minimal_subscriber.mattro_can.plot_vel()
    minimal_subscriber.destroy_node()     
    rclpy.shutdown()


if __name__ == '__main__':
    main()
