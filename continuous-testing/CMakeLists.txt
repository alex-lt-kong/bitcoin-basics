include_directories (${PROJECT_SOURCE_DIR}/src/) 

add_executable(script-test ./script-test.cpp)
add_executable(tx-test ./tx-test.cpp)
target_link_libraries(script-test boost_random mycrypto mybitcoin)
target_link_libraries(tx-test boost_random mycrypto mybitcoin curl jsoncpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/script-test-driver.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tx-test-driver.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
