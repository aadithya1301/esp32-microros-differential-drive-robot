#include <WiFi.h>
#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <geometry_msgs/msg/twist.h>
#include <std_msgs/msg/int32.h>

// ============================
// MOTOR PINS
// ============================
#define EN1 25
#define IN1 26
#define IN2 27

#define EN2 14
#define IN3 18
#define IN4 13

// ============================
// ENCODERS
// ============================
#define LEFT_C1 34
#define LEFT_C2 35

#define RIGHT_C1 33
#define RIGHT_C2 32

volatile long leftTicks = 0;
volatile long rightTicks = 0;

// ============================
// ROS
// ============================
rcl_subscription_t subscriber;

rcl_publisher_t left_encoder_pub;
rcl_publisher_t right_encoder_pub;

geometry_msgs__msg__Twist msg;

std_msgs__msg__Int32 left_msg;
std_msgs__msg__Int32 right_msg;

rclc_executor_t executor;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;

// ============================
// ENCODER ISR
// ============================
void IRAM_ATTR leftEncoder()
{
  if (digitalRead(LEFT_C2))
    leftTicks++;
  else
    leftTicks--;
}

void IRAM_ATTR rightEncoder()
{
  if (digitalRead(RIGHT_C2))
    rightTicks++;
  else
    rightTicks--;
}

// ============================
// MOTOR CONTROL
// ============================
void stopMotors()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveBackward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// ============================
// CMD_VEL CALLBACK
// ============================
void subscription_callback(const void * msgin)
{
  const geometry_msgs__msg__Twist * data =
    (const geometry_msgs__msg__Twist *)msgin;

  if (data->linear.x > 0.1)
  {
    moveForward();
  }
  else if (data->linear.x < -0.1)
  {
    moveBackward();
  }
  else if (data->angular.z > 0.1)
  {
    turnLeft();
  }
  else if (data->angular.z < -0.1)
  {
    turnRight();
  }
  else
  {
    stopMotors();
  }
}

// ============================
// SETUP
// ============================
void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("BOOT START");

  // ============================
  // WIFI
  // ============================
  WiFi.begin(
    "TECNO POVA 5 Pro 5G",
    "aadi1301"
  );

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi CONNECTED");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // ============================
  // MICRO-ROS TRANSPORT
  // ============================
  set_microros_wifi_transports(
    "TECNO POVA 5 Pro 5G",
    "aadi1301",
    "10.234.141.105",
    8888
  );

  Serial.println("micro-ROS transport set");

  // ============================
  // WAIT FOR AGENT
  // ============================
  while (rmw_uros_ping_agent(1000, 1) != RMW_RET_OK)
  {
    Serial.println("Waiting for micro-ROS agent...");
    delay(1000);
  }

  Serial.println("micro-ROS Agent Connected!");

  // ============================
  // MOTOR PINS
  // ============================
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);

  stopMotors();

  Serial.println("Motors Ready");

  // ============================
  // ENCODER PINS
  // ============================
  pinMode(LEFT_C1, INPUT);
  pinMode(LEFT_C2, INPUT);

  pinMode(RIGHT_C1, INPUT);
  pinMode(RIGHT_C2, INPUT);

  attachInterrupt(
    digitalPinToInterrupt(LEFT_C1),
    leftEncoder,
    RISING
  );

  attachInterrupt(
    digitalPinToInterrupt(RIGHT_C1),
    rightEncoder,
    RISING
  );

  Serial.println("Encoders Ready");

  // ============================
  // ROS INIT
  // ============================
  allocator = rcl_get_default_allocator();

  rclc_support_init(
    &support,
    0,
    NULL,
    &allocator
  );

  rclc_node_init_default(
    &node,
    "esp32_robot",
    "",
    &support
  );

  // ============================
  // SUBSCRIBER
  // ============================
  rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(
      geometry_msgs,
      msg,
      Twist
    ),
    "/cmd_vel"
  );

  // ============================
  // PUBLISHERS
  // ============================
  rclc_publisher_init_default(
    &left_encoder_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(
      std_msgs,
      msg,
      Int32
    ),
    "/left_encoder"
  );

  rclc_publisher_init_default(
    &right_encoder_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(
      std_msgs,
      msg,
      Int32
    ),
    "/right_encoder"
  );

  // ============================
  // EXECUTOR
  // ============================
  rclc_executor_init(
    &executor,
    &support.context,
    1,
    &allocator
  );

  rclc_executor_add_subscription(
    &executor,
    &subscriber,
    &msg,
    &subscription_callback,
    ON_NEW_DATA
  );

  Serial.println("ROS READY");
}

// ============================
// LOOP
// ============================
void loop()
{
  // publish encoder data
  left_msg.data = leftTicks;
  right_msg.data = rightTicks;

  rcl_publish(
    &left_encoder_pub,
    &left_msg,
    NULL
  );

  rcl_publish(
    &right_encoder_pub,
    &right_msg,
    NULL
  );

  // handle ROS callbacks
  rclc_executor_spin_some(
    &executor,
    RCL_MS_TO_NS(10)
  );

  delay(50);
}