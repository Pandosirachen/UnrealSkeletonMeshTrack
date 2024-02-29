target_sources (${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/client_main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/counter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/init.cpp
    ${CMAKE_CURRENT_LIST_DIR}/initialization_data.cpp
)

add_subdirectory(generics)
add_subdirectory(streams)