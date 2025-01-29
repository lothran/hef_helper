cmake_minimum_required(VERSION 3.10)
project(hef_helper)
add_executable(hef_helper main.cpp)

target_link_libraries(hef_helper PUBLIC hailort)
install(TARGETS hef_helper DESTINATION bin)

