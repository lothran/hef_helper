cmake_minimum_required(VERSION 3.10)
project(hef_helper)
add_executable(hef_helper main.cpp)

find_package(HailoRT)
target_link_libraries(hef_helper PUBLIC HailoRT::libhailort)
install(TARGETS hef_helper DESTINATION bin)

