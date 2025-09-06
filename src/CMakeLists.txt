# Collect sources
set(SOURCES
    main.cpp
)

# Create executable in top-level build folder
add_executable(kvick ${SOURCES})

# Optional: specify output directory
set_target_properties(kvick PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
