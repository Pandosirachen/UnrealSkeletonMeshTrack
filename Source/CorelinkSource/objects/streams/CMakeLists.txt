target_sources (${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_base.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_recv_base.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_recv_tcp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_recv_udp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_send_base.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_send_tcp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_data_send_udp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_main_base.cpp
    ${CMAKE_CURRENT_LIST_DIR}/comm_main_tcp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stream_data.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tcp_recv_handler.cpp
)
