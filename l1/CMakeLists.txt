add_executable(streq streq.cc)
target_link_libraries(streq PRIVATE cpp-master-settings)

add_executable(conv-qual conv-qual.cc)
target_link_libraries(conv-qual PRIVATE cpp-master-settings)
