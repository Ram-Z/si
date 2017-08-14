cmake_minimum_required(VERSION 3.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_CURRENT_SOURCE_DIR}/cmake/common)

project(SI LANGUAGES CXX)

include(GitDescribe)
git_describe(PREFIX ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTING "Enable testing" ON)

add_library(si INTERFACE)
add_library(SI::SI ALIAS si)

target_include_directories(si
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING)
  include(Catch)
  get_catch(VERSION 1.9.3)

  add_test_executable(si_test
    test/tests.cpp
    test/si.test.cpp
  )

  target_link_libraries(si_test
    PRIVATE Catch::Catch
    PRIVATE SI::SI
  )

  include(ClangTools)
  clang_tidy(si_test)
endif()

install(DIRECTORY include/ DESTINATION include)
install(TARGETS si EXPORT si-targets)
install(EXPORT si-targets
  NAMESPACE SI::
  DESTINATION lib/cmake/si
)
install(FILES cmake/si-config.cmake
  DESTINATION lib/cmake/si
)
