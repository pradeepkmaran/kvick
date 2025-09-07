set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kvick
    KVick.cpp
    KVickServer.cpp
    main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(kvick PRIVATE Threads::Threads)