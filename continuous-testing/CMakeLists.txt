include_directories (${PROJECT_SOURCE_DIR}/src/) 

add_executable(script-test ./script-test.cpp)
target_link_libraries(script-test boost_random mycrypto mybitcoin)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/script-test-driver.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
