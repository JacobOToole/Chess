add_executable(chess_gui main.cpp Renderer.cpp
        EngineProcess.h
        EngineProcess.cpp)
target_link_libraries(chess_gui PRIVATE
        chess_common
        sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET chess_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:chess_gui>/assets)