cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PublisherSubscriber)

# Adds warnings and allows debug with gdb
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra)

add_executable(ps
    main.cpp
    ./components/Topic.hpp
    ./components/Message.hpp
    ./components/Provider.hpp
    ./components/Subscriber.hpp
)

cmake_policy(SET CMP0167 NEW)