# TODO 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)
# TODO 2: Create a project named Tutorial
project(Tutorial)
# TODO 7: Set the project version number as 1.0 in the above project command
project(Tutorial VERSION 1.0)
# TODO 6: Set the variable CMAKE_CXX_STANDARD to 11
#         and the variable CMAKE_CXX_STANDARD_REQUIRED to True
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
set (M5602Sources
        main.cpp M6502Tests.cpp)
add_executable(test ${M5602Sources})
target_link_libraries(test gtest_main)
#add_test(NAME test COMMAND test_me)

# TODO 8: Use configure_file to configure and copy TutorialConfig.h.in to
#         TutorialConfig.h
#configure_file(TutorialConfig.h.in TutorialConfig.h)
# TODO 3: Add an executable called Tutorial to the project
# Hint: Be sure to specify the source file as tutorial.cxx
#add_executable(test main.cpp)
# TODO 9: Use target_include_directories to include ${PROJECT_BINARY_DIR}
target_include_directories(test PUBLIC
                           "${PROJECT_BINARY_DIR}" "${PROJECT_BINARY_DIR}/_deps/googletest-src/googletest/include"
                           )