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
    src/gf_element.cpp
    src/matrix.cpp
)

target_include_directories(matrix_gf2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Примеры
add_executable(example_basic examples/example_basic.cpp)
target_link_libraries(example_basic matrix_gf2)

add_executable(example_gauss examples/example_gauss.cpp)
target_link_libraries(example_gauss matrix_gf2)

add_executable(example_inverse examples/example_inverse.cpp)
target_link_libraries(example_inverse matrix_gf2)

add_executable(example_educational examples/example_educational.cpp)
target_link_libraries(example_educational matrix_gf2)

# Тесты (опционально)
option(BUILD_TESTS "Собрать тесты" ON)
if(BUILD_TESTS)
    enable_testing()
    add_executable(test_matrix_gf2 tests/test_matrix_gf2.cpp)
    target_link_libraries(test_matrix_gf2 matrix_gf2)
    add_test(NAME test_matrix_gf2 COMMAND test_matrix_gf2)
endif()

# Установка
install(TARGETS matrix_gf2
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/matrix_gf2
    DESTINATION include
)
