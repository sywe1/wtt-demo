cmake_minimum_required(VERSION 3.5.0)
project(wtt-demo)

message(STATUS "PATH: ${CMAKE_PREFIX_PATH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  add_definitions("-frounding-math")
endif()

find_package(CGAL COMPONENTS Core)
find_package(Qt5 COMPONENTS Gui Widgets OpenGL REQUIRED)
find_package(wtlib REQUIRED)

include_directories(
  include  
  ${wtlib_INCLUDE_DIRS})

set(MAINWINDOW "create_mainwindow")

set(${MAINWINDOW}_SRC
    src/main.cpp
    src/mainwindow.cpp
    src/opengl_widget.cpp
    src/action_panel.cpp
    src/arcball_camera.cpp
    src/scene_object.cpp
    src/triangle_mesh_scene.cpp
    src/modal_widget.cpp
    src/message_box.cpp
    src/wtt_manager.cpp
    src/integer_setter.cpp
    src/input_prop.cpp
)


qt5_wrap_cpp(${MAINWINDOW}_MOCS 
              include/mainwindow.hpp
              include/opengl_widget.hpp
              include/action_panel.hpp
              include/arcball_camera.hpp
              include/scene_object.hpp
              include/triangle_mesh_scene.hpp
              include/modal_widget.hpp
              include/message_box.hpp
              include/wtt_manager.hpp
              include/integer_setter.hpp
              include/input_prop.hpp
            )

qt5_wrap_ui(${MAINWINDOW}_UIS
            uis/mainwindow.ui
            uis/action_panel.ui
            uis/messagebox.ui
            uis/integer_setter.ui
            uis/input_prop.ui)

qt5_add_resources(${MAINWINDOW}_RESOURCES
                  resources/resources.qrc)

add_executable(${MAINWINDOW}
                ${${MAINWINDOW}_SRC}
                ${${MAINWINDOW}_UIS}
                ${${MAINWINDOW}_RESOURCES}
                ${${MAINWINDOW}_MOCS}
              )

target_include_directories(${MAINWINDOW}
              PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
              PUBLIC ${CGAL_INCLUDE_DIRS}
              PUBLIC include
              )

message(STATUS "${TEXT_WHITE}UI destination: ${TEXT_GREEN}${CMAKE_CURRENT_BINARY_DIR}${TEXT_RESET}")
message(STATUS "${TEXT_WHITE}UI name: ${TEXT_GREEN}${${MAINWINDOW}_UIS}${TEXT_RESET}")

target_link_libraries(${MAINWINDOW}
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Gui
                      Qt5::OpenGL
                      ${CGAL_LIBRARY})