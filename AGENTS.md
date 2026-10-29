# Project instructions for people and AI agents

Read [ROS.md](ROS.md) before changing ROS, Tempo, robot motion, sensors, or project startup.

- Use UE 5.8. On Windows, `UNREAL_ENGINE_PATH` must point to `C:\UnrealEngines\UE_5.8`.
- Build and start the editor with VS Code **F5**. Close an existing Editor instance before a full C++ rebuild.
- Run ROS 2 commands in Ubuntu WSL 22.04 with ROS 2 Humble, not through an arbitrary Windows `ros2.exe` wrapper.
- Before every ROS command, run `source /opt/ros/humble/setup.bash` and `export RMW_IMPLEMENTATION=rmw_fastrtps_cpp`.
- `ATempoTestRobot` owns its native `UTempoROSNode` in a `UPROPERTY`. Keep nodes alive this way; create them with `UTempoROSNode::Create`.
- Do not edit `Plugins/Tempo`, `Plugins/TempoROS`, `Plugins/Tempo/TempoROSBridge`, generated ROS/protobuf files, or the Unreal Engine installation to implement project behavior. Put project behavior in `Source/TempoSample`.
- The current `/cmd_vel` subscriber is **telemetry only**. Do not represent it as robot motion until a separately tested motion implementation exists.
- The TempoROSBridge movement services previously crashed this editor configuration. Do not call `/GetCommandablePawns`, `/CommandVelocity`, `/CommandVehicle`, or `/CommandAcceleration` while investigating native robot integration.
