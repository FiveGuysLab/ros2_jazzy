# Preemptive EDF Executor

A ROS2 package implementing a Preemptive Earliest Deadline First (EDF) executor for real-time scheduling of ROS2 nodes.

## Overview

This package provides a custom executor that implements the EDF scheduling algorithm, which is optimal for real-time systems. The executor prioritizes tasks based on their deadlines, ensuring that tasks with the earliest deadlines are executed first.

## Features

- Preemptive EDF scheduling algorithm
- Real-time task prioritization
- Compatible with standard ROS2 nodes
- Configurable execution parameters

## Package Structure

```
preemptive_edf_executor/
├── CMakeLists.txt
├── package.xml
├── README.md
├── include/
│   └── preemptive_edf_executor/
│       └── preemptive_edf_executor.hpp
├── src/
│   └── preemptive_edf_executor_node.cpp
└── launch/
    └── preemptive_edf_executor.launch.py
```

## Dependencies

- `rclcpp` - ROS2 C++ client library
- `std_msgs` - Standard ROS2 message types
- `ament_cmake` - Build system

## Building

From your ROS2 workspace root:

```bash
# Source ROS2 (adjust path as needed)
source /opt/ros/jazzy/setup.bash

# Build the package
colcon build --packages-select preemptive_edf_executor

# Source the workspace
source install/setup.bash
```

## Running

### Using the launch file:
```bash
ros2 launch preemptive_edf_executor preemptive_edf_executor.launch.py
```

### Running the node directly:
```bash
ros2 run preemptive_edf_executor preemptive_edf_executor_node
```

## Usage

The package includes a simple example node that demonstrates basic ROS2 functionality:
- Publishes messages on the `/topic` topic
- Subscribes to messages on the `/topic` topic
- Uses a timer for periodic execution

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This package is licensed under the Apache License 2.0.
