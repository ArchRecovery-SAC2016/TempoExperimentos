"""A bounded open-loop /cmd_vel experiment for the Tempo test robot."""

import math
import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node


class MoveAndRotate(Node):
    """Publish a small forward-and-turn maneuver, then explicitly stop."""

    def __init__(self) -> None:
        super().__init__("move_and_rotate")
        self.publisher = self.create_publisher(Twist, "/cmd_vel", 10)
        self.phases = [
            ("Driving forward", 3.0, 0.2, 0.0),
            ("Stopping", 0.5, 0.0, 0.0),
            ("Turning left", math.pi, 0.0, 0.5),
            ("Stopping", 0.5, 0.0, 0.0),
        ]
        self.phase_index = 0
        self.phase_started_at = time.monotonic()
        self.finished = False
        self.timer = self.create_timer(0.1, self.tick)
        self.log_phase()

    def publish_velocity(self, linear_x: float, angular_z: float) -> None:
        command = Twist()
        command.linear.x = linear_x
        command.angular.z = angular_z
        self.publisher.publish(command)

    def log_phase(self) -> None:
        name, duration, linear_x, angular_z = self.phases[self.phase_index]
        self.get_logger().info(
            f"{name}: linear.x={linear_x:.2f} m/s, angular.z={angular_z:.2f} rad/s for {duration:.2f} s"
        )

    def tick(self) -> None:
        if self.finished:
            return

        name, duration, linear_x, angular_z = self.phases[self.phase_index]
        self.publish_velocity(linear_x, angular_z)

        if time.monotonic() - self.phase_started_at < duration:
            return

        self.phase_index += 1
        self.phase_started_at = time.monotonic()
        if self.phase_index >= len(self.phases):
            self.publish_velocity(0.0, 0.0)
            self.finished = True
            self.timer.cancel()
            self.get_logger().info("Maneuver complete. Published zero velocity.")
            return

        self.log_phase()

    def stop(self) -> None:
        self.publish_velocity(0.0, 0.0)


def main() -> None:
    rclpy.init()
    node = MoveAndRotate()
    try:
        while rclpy.ok() and not node.finished:
            rclpy.spin_once(node, timeout_sec=0.1)
    except KeyboardInterrupt:
        node.get_logger().info("Interrupted by user.")
    finally:
        node.stop()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
