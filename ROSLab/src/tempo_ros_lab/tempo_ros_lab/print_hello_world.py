"""Minimal ROS 2 publisher that proves ROSLab can reach Unreal TempoROS."""

import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class PrintHelloWorld(Node):
    """Publish a small diagnostic message to the native Unreal ROS node."""

    def __init__(self) -> None:
        super().__init__("print_hello_world")
        self.publisher = self.create_publisher(String, "/tempo_echo", 10)
        self.timer = self.create_timer(1.0, self.publish_hello)
        self.get_logger().info("Publishing /tempo_echo once per second. Press Ctrl+C to stop.")

    def publish_hello(self) -> None:
        message = String()
        message.data = "Hello from ROSLab"
        self.publisher.publish(message)
        self.get_logger().info(f"Published: {message.data}")


def main() -> None:
    rclpy.init()
    node = PrintHelloWorld()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
