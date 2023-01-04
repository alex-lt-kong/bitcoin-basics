cmake_minimum_required(VERSION 3.18.4)


project(LIBMYBITCOIN)

add_compile_options(-Wall -Wextra -pedantic -O3)


add_subdirectory(src/mybitcoin)
add_subdirectory(chapter-test)
add_subdirectory(continuous-testing)
