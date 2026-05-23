# ESP32 micro-ROS Differential Drive Robot

A WiFi-controlled differential drive robot using an ESP32, micro-ROS, ROS 2, DC motors, and quadrature encoders.

This project allows an ESP32 robot to:

- Receive `/cmd_vel` commands from ROS 2
- Control two DC motors
- Read wheel encoder ticks
- Publish encoder data back to ROS 2
- Communicate wirelessly over WiFi using micro-ROS UDP transport

---

# Features

- ESP32 + micro-ROS integration
- ROS 2 `/cmd_vel` subscriber
- Left and right wheel encoder publishers
- Differential drive movement
- WiFi communication
- Quadrature encoder support
- Compatible with ROS 2 Jazzy / Humble
- Real-time encoder feedback

---

# Hardware Used

| Component | Description |
|---|---|
| ESP32 Dev Board | Main controller |
| L298N / Motor Driver | Dual motor driver |
| N20 DC Motors | Differential drive motors |
| Quadrature Encoders | Wheel odometry |
| Battery Pack | Power source |
| Ubuntu PC | Running ROS 2 + micro-ROS Agent |

---

# Pin Configuration

## Motor Driver Pins

| ESP32 Pin | Function |
|---|---|
| 25 | EN1 |
| 26 | IN1 |
| 27 | IN2 |
| 14 | EN2 |
| 18 | IN3 |
| 13 | IN4 |

---

## Encoder Pins

| ESP32 Pin | Encoder |
|---|---|
| 33 | LEFT_C1 |
| 32 | LEFT_C2 |
| 34 | RIGHT_C1 |
| 35 | RIGHT_C2 |

---

# ROS 2 Topics

## Subscriber

| Topic | Type | Description |
|---|---|---|
| `/cmd_vel` | `geometry_msgs/Twist` | Robot velocity commands |

---

## Publishers

| Topic | Type | Description |
|---|---|---|
| `/left_encoder` | `std_msgs/Int32` | Left wheel encoder ticks |
| `/right_encoder` | `std_msgs/Int32` | Right wheel encoder ticks |

---

# Robot Movements

| Command | Action |
|---|---|
| `linear.x > 0` | Move Forward |
| `linear.x < 0` | Move Backward |
| `angular.z > 0` | Turn Left |
| `angular.z < 0` | Turn Right |
| No input | Stop |

---

# Software Requirements

- Ubuntu 22.04 / 24.04
- ROS 2 Jazzy or Humble
- micro-ROS Agent
- Arduino IDE or PlatformIO
- ESP32 Board Package
- `micro_ros_arduino` library

---

# Configure WiFi

Before uploading the code, replace the following placeholders inside the `.ino` file:

```cpp
WiFi.begin(
    "YOUR_WIFI_NAME",
    "YOUR_WIFI_PASSWORD"
);

set_microros_wifi_transports(
    "YOUR_WIFI_NAME",
    "YOUR_WIFI_PASSWORD",
    "YOUR_AGENT_IP",
    8888
);
```

Replace:

| Placeholder | Description |
|---|---|
| `YOUR_WIFI_NAME` | Your WiFi SSID |
| `YOUR_WIFI_PASSWORD` | Your WiFi password |
| `YOUR_AGENT_IP` | IP address of Ubuntu/ROS 2 PC |

---

# Installing micro-ROS Library

## Arduino IDE

1. Open Arduino IDE
2. Go to:

   ```
   Sketch → Include Library → Manage Libraries
   ```

3. Search:

   ```
   micro_ros_arduino
   ```

4. Install the library

---

# Uploading the Code

1. Open the `.ino` file
2. Select:

   - Board: `ESP32 Dev Module`
   - Correct COM/USB port

3. Upload the code to ESP32

---

# Running micro-ROS Agent

On Ubuntu PC:

```bash
source /opt/ros/jazzy/setup.bash

ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

---

# Running the Robot

## Move Forward

```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.5}, angular: {z: 0.0}}"
```

---

## Turn Left

```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.0}, angular: {z: 1.0}}"
```

---

## Stop Robot

```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.0}, angular: {z: 0.0}}"
```

---

# Checking Encoder Data

## Left Encoder

```bash
ros2 topic echo /left_encoder
```

---

## Right Encoder

```bash
ros2 topic echo /right_encoder
```

---

## Both Encoders Together

```bash
ros2 topic echo /left_encoder &
ros2 topic echo /right_encoder
```

---

# Example Serial Output

```text
BOOT START
Connecting WiFi.....
WiFi CONNECTED
ESP32 IP: 192.168.x.x
micro-ROS transport set
micro-ROS Agent Connected!
Motors Ready
Encoders Ready
ROS READY
```

---

# Project Structure

```text
.
├── README.md
├── LICENSE
├── .gitignore
└── ESP32_MicroRos_Control.ino
```

---

# Future Improvements

- PID motor control
- Wheel odometry calculation
- TF publishing
- SLAM integration
- LiDAR support
- Autonomous navigation
- ROS 2 Navigation Stack
- Web dashboard monitoring

---

# Author

## Aadithya Rajesh

ESP32-Based Robust Odometry Module

---

# License

This project is licensed under the MIT License.
