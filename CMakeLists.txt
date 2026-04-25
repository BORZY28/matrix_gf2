cmake_minimum_required(VERSION 3.10)
project(matrix_gf2 VERSION 1.0 LANGUAGES CXX)

# Требуем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции компиляции
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Библиотека matrix_gf2
add_library(matrix_gf2 STATIC
    src/gf2.cpp
    src/gf_element.cpp
    src/matrix.cpp
)

target_include_directories(matrix_gf2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Тесты (опционально)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)

    add_executable(test_matrix_gf2_new tests/test_matrix_gf2_new.cpp)
    target_link_libraries(test_matrix_gf2_new matrix_gf2)
endif()

# Установка
install(TARGETS matrix_gf2
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/matrix_gf2
    DESTINATION include
)
