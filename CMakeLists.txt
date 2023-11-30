cmake_minimum_required(VERSION 3.16)
project(diffdrive_ev3 LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra)
endif()

# find dependencies
set(THIS_PACKAGE_INCLUDE_DEPENDS
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

# find dependencies
find_package(ament_cmake REQUIRED)
foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${Dependency} REQUIRED)
endforeach()


## COMPILE
add_library(
  diffdrive_ev3
  SHARED
  hardware/diffbot_system.cpp
)
target_compile_features(diffdrive_ev3 PUBLIC cxx_std_17)
target_include_directories(diffdrive_ev3 PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/hardware/include>
$<INSTALL_INTERFACE:include/diffdrive_ev3>
)
ament_target_dependencies(
  diffdrive_ev3 PUBLIC
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

#target_link_libraries(diffdrive_ev3 PUBLIC serial)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "diffdrive_ev3_BUILDING_DLL")

# Export hardware plugins
pluginlib_export_plugin_description_file(hardware_interface diffdrive_ev3.xml)

# INSTALL
install(
  DIRECTORY hardware/include/
  DESTINATION include/diffdrive_ev3
)
#install(
#  DIRECTORY description/launch description/ros2_control description/urdf description/rviz
#  DESTINATION share/diffdrive_ev3
#)
#install(
#  DIRECTORY bringup/launch bringup/config
#  DESTINATION share/diffdrive_ev3
#)
install(TARGETS diffdrive_ev3
  EXPORT export_diffdrive_ev3
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
endif()

## EXPORTS
ament_export_targets(export_diffdrive_ev3 HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})
ament_package()
