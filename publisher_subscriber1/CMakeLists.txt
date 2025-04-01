cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PublisherSubscriber2)

# Adds warnings and allows debug with gdb
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra)

add_executable(ps2
    main.cpp
    ./components/Topic.hpp
    ./components/Message.hpp
    ./components/Provider.hpp
    ./components/Subscriber.hpp
)

cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED COMPONENTS thread chrono)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(ps2 ${Boost_LIBRARIES})

#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
#    target_compile_options(ps1 PRIVATE -fsanitize=address -fno-omit-frame-pointer)
#    target_link_options(ps1 PRIVATE -fsanitize=address)
#endif()