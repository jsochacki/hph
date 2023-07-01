project(PROGRAM_NAME_test)
# main.cpp is the main.cpp in this directory
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                         lib_src_inc::com_sys_lib)

add_test(${PROJECT_NAME} PROGRAM_NAME)
