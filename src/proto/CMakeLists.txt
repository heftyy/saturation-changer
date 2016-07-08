cmake_minimum_required(VERSION 3.2)

include(FindProtobuf)
find_package(Protobuf REQUIRED)

set(PROTO_FILES
        Configuration.proto
        )

PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTO_FILES})

# adding PROTO_FILES to library so IDEs recognize them
add_library(proto ${PROTO_HEADER} ${PROTO_SRC} ${PROTO_FILES})
target_link_libraries(proto ${PROTOBUF_LIBRARY})
target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIR})
