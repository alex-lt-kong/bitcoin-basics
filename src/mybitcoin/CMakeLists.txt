set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_library(MYCRYPTO_LIB mycrypto)
if(NOT MYCRYPTO_LIB)
  message(
    FATAL_ERROR "mycrypto library not found, follow the link to build it first: https://github.com/alex-lt-kong/libmycrypto"
  )
endif()


find_library(CURL_LIB curl)
if(NOT CURL_LIB)
  message(
    FATAL_ERROR "curl library not found: apt-get install libcurl4-gnutls-dev"
  )
endif()

add_library(ecc ecc.cpp)
add_library(op op.cpp)
add_library(script script.cpp)
add_library(tx tx.cpp)
add_library(utils utils.cpp)

add_library(mybitcoin ecc op script tx utils)
target_link_libraries(mybitcoin mycrypto curl)


set_target_properties(mybitcoin PROPERTIES PUBLIC_HEADER "ecc.h;op.h;script.h;tx.h;utils.h;")

install(TARGETS mybitcoin 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/mybitcoin
)