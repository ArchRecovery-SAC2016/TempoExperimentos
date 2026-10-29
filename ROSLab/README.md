# ROSLab

Small, incremental ROS 2 Python experiments for this project. These programs talk only through standard ROS 2 topics, so the same high-level programs can later run with a physical robot whose driver exposes `/cmd_vel`, TF, and sensor topics.

The Unreal-side interface and the project rules are documented in [`../ROS.md`](../ROS.md).

## Layout

```text
ROSLab/
  src/tempo_ros_lab/        ROS 2 Python package (committed)
  build/, install/, log/    local colcon artifacts (ignored by Git)
```

## Build once or after changing Python source

Run in Ubuntu WSL, from this directory:

```bash
source /opt/ros/humble/setup.bash
cd /mnt/u/Doutorado/Experimentos/TempoSim/TempoExperimentos/ROSLab
colcon build --symlink-install
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp
```

`--symlink-install` lets edits to Python files take effect without rebuilding the package. Re-run `source install/setup.bash` whenever opening a new terminal.

The Unreal Editor must be running in **Play** with `BP_MyStreetSweeper` in the map before an example can communicate with it.

## Examples

### 1. PrintHelloWorld

```bash
ros2 run tempo_ros_lab print_hello_world
```

Publishes `Hello from ROSLab` every second on `/tempo_echo`. The Unreal Output Log should show the received message. Stop with `Ctrl+C`.

### 2. MoveAndRotate

```bash
ros2 run tempo_ros_lab move_and_rotate
```

This publishes `/cmd_vel` at 10 Hz and executes a deliberately small open-loop maneuver:

1. Drive forward at 0.2 m/s for 3 seconds (nominally 0.6 m).
2. Stop for 0.5 s.
3. Turn left at 0.5 rad/s for approximately 90 degrees.
4. Publish a zero velocity command and exit.

The simulated robot also has a 0.5 s command timeout, so it stops if this node crashes or loses communication. Collision can make the travelled distance smaller than nominal; that is expected and is why this is only a command-interface test, not navigation.

## Conventions for new examples

- Create a new executable under `src/tempo_ros_lab/tempo_ros_lab/` and register it in `setup.py`.
- Use `rclpy` and standard ROS message types. Do not import Unreal Python APIs or use direct sockets in these programs.
- Publish a zero `Twist` in a `finally` block for anything that controls `/cmd_vel`.
- Keep each example narrow and document its topic contract, expected output, and safety behavior here.
- Do not claim a behavior is ready for a real robot until its ROS input/output contract has been tested end to end.
