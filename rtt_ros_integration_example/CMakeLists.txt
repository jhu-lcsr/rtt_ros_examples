cmake_minimum_required(VERSION 2.8.3)

project(rtt_ros_integration_example)

find_package(catkin REQUIRED COMPONENTS rtt_ros)

catkin_package(
  DEPENDS rtt 
  CATKIN_DEPENDS rtt_ros 
)

add_definitions(-DRTT_COMPONENT)
orocos_component(${PROJECT_NAME} src/HelloRobot.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

orocos_generate_package()

install(FILES 
  example.launch 
  example.ops 
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})


