add_executable(kirushka main.cpp
        Eval.h
        Eval.cpp)
set_target_properties(kirushka PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/assets/engines")

target_link_libraries(kirushka PRIVATE chess_common)