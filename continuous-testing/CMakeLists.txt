include_directories (${PROJECT_SOURCE_DIR}/src/) 

add_executable(script-test ./script-test.cpp)
add_executable(tx-test ./tx-test.cpp)
add_executable(aes ./aes.cpp)
target_link_libraries(script-test boost_random mycrypto mybitcoin)
target_link_libraries(tx-test boost_random mycrypto mybitcoin curl)
target_link_libraries(aes crypto++)

find_library(CRYPTOPP_LIB crypto++)
if(NOT CRYPTOPP_LIB)
    message(FATAL_ERROR "crypto++ library not found, install it with 'apt install libcrypto++-dev'")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/script-test-driver.py
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
