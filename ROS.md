# ROS 2 + Unreal / Tempo: operating guide

This project uses the native TempoROS integration: Unreal hosts ROS 2 nodes through `rclcpp`; Ubuntu WSL is an external ROS 2 client. There is no manually written socket bridge.

## Supported setup

- Windows: Unreal Engine 5.8 at `C:\UnrealEngines\UE_5.8`.
- Ubuntu WSL 2: Ubuntu 22.04 with ROS 2 Humble Desktop.
- DDS middleware on both sides: Fast DDS (`rmw_fastrtps_cpp`).
- Project robot: `BP_MyStreetSweeper`, derived from the C++ class `ATempoTestRobot` and placed in the map.

Do not modify Tempo, TempoROS, TempoROSBridge, generated code, or the Unreal Engine installation for project features. Add project-specific C++ under `Source/TempoSample`.

## Start the simulation

1. Close a running Unreal Editor instance.
2. In VS Code, open this project and press **F5**. It builds `TempoSampleEditor` and opens the project with the configured UE 5.8.
3. Open the map containing `BP_MyStreetSweeper` and press **Play**.

For any ROS command, open Ubuntu WSL and start with:

```bash
source /opt/ros/humble/setup.bash
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp
```

Use `--no-daemon` when inspecting the ROS graph if a stale daemon gives incomplete results:

```bash
ros2 node list --no-daemon
ros2 topic list --no-daemon
```

## Verified native TempoROS smoke test

`ATempoTestRobot` creates `UTempoROSNode` named `TempoTestRobotSmoke` in `BeginPlay`. The node is retained in a `UPROPERTY`, as required for Unreal garbage collection safety.

### Unreal -> ROS

Before pressing Play, wait for the startup message in Ubuntu:

```bash
ros2 topic echo /tempo_smoke std_msgs/msg/String --once
```

Expected result after Play:

```text
data: Hello from Unreal TempoROS
```

### ROS -> Unreal

With the simulation in Play, send:

```bash
ros2 topic pub --once /tempo_echo std_msgs/msg/String "{data: 'Ola do Ubuntu'}"
```

Expected Unreal Output Log line:

```text
LogTemp: Display: TempoROS received: Ola do Ubuntu
```

## Current robot ROS interface

| Topic | Type | Direction | Current behavior |
| --- | --- | --- | --- |
| `/tempo_smoke` | `std_msgs/msg/String` | Unreal -> ROS | One startup confirmation message. |
| `/tempo_echo` | `std_msgs/msg/String` | ROS -> Unreal | Writes its payload to Unreal Output Log. |
| `/cmd_vel` | `geometry_msgs/msg/Twist` | ROS -> Unreal | Planar teleoperation: `linear.x` moves forward/backward and `angular.z` turns. Commands expire after 0.5 s. |
| `/tempo_test_robot/pose` | `geometry_msgs/msg/Transform` | Unreal -> ROS | Robot transform at 10 Hz. |
| `/tf` | `tf2_msgs/msg/TFMessage` | Unreal -> ROS | Dynamic transform `map -> base_link` at 10 Hz. |

TempoROS converts the native `FTwist` and `FTransform` types: Unreal centimetres/left-handed axes become ROS metres/right-handed axes. Do not manually convert those values a second time.

Test command reception:

```bash
ros2 topic pub --once /cmd_vel geometry_msgs/msg/Twist \
  "{linear: {x: 0.2}, angular: {z: 0.0}}"
```

Expected Output Log line (the exact signs follow TempoROS coordinate conversion):

```text
TempoROS received /cmd_vel: forward=20.0 cm/s, yaw=0.0 deg/s
```

The robot moves for up to 0.5 seconds after this one-shot command, then stops. For continuous teleoperation, publish at 10 Hz and stop with `Ctrl+C`:

```bash
ros2 topic pub -r 10 /cmd_vel geometry_msgs/msg/Twist \
  "{linear: {x: 0.2}, angular: {z: 0.0}}"
```

The native robot applies a collision sweep while translating. It currently supports only `linear.x` and `angular.z`, limits them to 1 m/s and 90 deg/s, and does not yet publish odometry. These are intentional limits for the first motion milestone.

Test pose and TF:

```bash
ros2 topic echo /tempo_test_robot/pose geometry_msgs/msg/Transform --once
ros2 run tf2_ros tf2_echo map base_link
```

`tf2_echo` keeps running; use `Ctrl+C` to stop it.

## Development rules

- The C++ module uses `TempoROS`, `rclcpp`, and `bEnableExceptions = true` in `TempoSample.Build.cs`, as required by TempoROS for a module that creates nodes.
- Create nodes through `UTempoROSNode::Create(...)`, and keep each returned node in a `UPROPERTY`.
- Standard converters already exist for `FString`, `FVector`, `FRotator`, `FTransform`, and `FTwist` in `TempoROSCommonConverters.h`.
- Keep the first motion integration independent of TempoROSBridge movement services. In this specific project/editor combination those services have caused a native Editor crash and are not yet validated.
- The TempoROSBridge currently provides camera, clock, and TF-related ROS interfaces. LiDAR is not yet a validated ROS topic in this project. Do not claim LiDAR, SLAM, Nav2, odometry, or physical robot control works until each has an explicit end-to-end test.

## Next milestones

1. Validate `/cmd_vel` motion with collision and command timeout (current milestone).
2. Publish a validated LiDAR ROS message from project C++ and inspect it in RViz2.
3. Add odometry and change the TF tree to `map -> odom -> base_link` for SLAM.
4. Integrate SLAM Toolbox, then Nav2 waypoint navigation.
5. Add coverage planning for a small rectangular area before scaling to the full terrain.
