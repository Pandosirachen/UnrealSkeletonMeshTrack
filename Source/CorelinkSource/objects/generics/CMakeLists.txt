# NOTE: Warning with generics, Use of template may cause LNK2019 errors unless the code is in the headers only.
target_sources (${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/message_handler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/safe_queue.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stream_map.cpp
)
