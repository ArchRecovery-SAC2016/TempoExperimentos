# Project instructions for people and AI agents

Read [ROS.md](ROS.md) before changing ROS, Tempo, robot motion, sensors, or project startup.

- Use UE 5.8. On Windows, `UNREAL_ENGINE_PATH` must point to `C:\UnrealEngines\UE_5.8`.
- Build and start the editor with VS Code **F5**. Close an existing Editor instance before a full C++ rebuild.
- Run ROS 2 commands in Ubuntu WSL 22.04 with ROS 2 Humble, not through an arbitrary Windows `ros2.exe` wrapper.
- Before every ROS command, run `source /opt/ros/humble/setup.bash` and `export RMW_IMPLEMENTATION=rmw_fastrtps_cpp`.
- `ATempoTestRobot` owns its native `UTempoROSNode` in a `UPROPERTY`. Keep nodes alive this way; create them with `UTempoROSNode::Create`.
- Do not edit `Plugins/Tempo`, `Plugins/TempoROS`, `Plugins/Tempo/TempoROSBridge`, generated ROS/protobuf files, or the Unreal Engine installation to implement project behavior. Put project behavior in `Source/TempoSample`.
- `/cmd_vel` controls only bounded planar teleoperation (`linear.x` and `angular.z`) with a 0.5 s timeout and collision sweep. It is not yet an odometry, SLAM, Nav2, or coverage-planning implementation.
- The TempoROSBridge movement services previously crashed this editor configuration. Do not call `/GetCommandablePawns`, `/CommandVelocity`, `/CommandVehicle`, or `/CommandAcceleration` while investigating native robot integration.
