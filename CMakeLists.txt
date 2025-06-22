cmake_minimum_required(VERSION 3.31)
project(MeinCraft)

set(CMAKE_CXX_STANDARD 20)


configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_config/rootdirectory.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/root_directory.h
)

# Source files
add_executable(MeinCraft
        main.cpp
        libs/glad/src/glad.c
        include/stb_image.h
        classes/Block/Block.cpp
        classes/Block/Block.h
)

# Include directories
include_directories(
        libs/glfw/include
        libs/glad/include
        libs/glm
        include/stb_image.h
        ${CMAKE_CURRENT_BINARY_DIR} # where root_directory.h is generated
)


# Find OpenGL
find_package(OpenGL REQUIRED)

# Link GLFW (you may need to update this if you're using built binaries)
#add_subdirectory(libraries/include/GLFW)

# Link everything together
add_subdirectory(libs/glfw)



# Link everything together
target_link_libraries(MeinCraft
        glfw                       # Links the actual glfw target
        ${OPENGL_gl_LIBRARY}      # Links to Windows' opengl32
)
