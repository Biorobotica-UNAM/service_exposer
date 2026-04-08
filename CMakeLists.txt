cmake_minimum_required(VERSION 3.8)
project(service_exposer)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(head_interfaces REQUIRED)

add_executable(service_exposer_node
  src/service_exposer_node.cpp
)

ament_target_dependencies(service_exposer_node
  rclcpp
  sensor_msgs
  std_srvs
  head_interfaces
)

install(TARGETS service_exposer_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_package()

