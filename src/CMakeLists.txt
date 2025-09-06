set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kvick
    KVick.cpp
    KVickServer.cpp
    main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(kvick PRIVATE Threads::Threads)

if (WIN32)
    target_compile_definitions(kvick PRIVATE _WIN32_WINNT=0x0601)
    target_link_libraries(kvick PRIVATE ws2_32)
endif()
