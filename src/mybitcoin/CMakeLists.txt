set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_library(MYCRYPTO_LIB mycrypto)
if(NOT MYCRYPTO_LIB)
  message(FATAL_ERROR "mycrypto library not found, install it with: https://github.com/alex-lt-kong/libmycrypto")
endif()
find_library(CRYPTOPP_LIB crypto++)
if(NOT CRYPTOPP_LIB)
    message(FATAL_ERROR "nlohmann-json library not found, install it with 'apt install nlohmann-json3-dev'")
endif()
find_library(BOOST_RANDOM_LIB boost_random)
if(NOT BOOST_RANDOM_LIB)
  message(FATAL_ERROR "boost_random library not found, install it with 'apt install libboost-all-dev'")
endif()
find_library(CURL_LIB curl)
if(NOT CURL_LIB)
  message(FATAL_ERROR "curl library not found, install it with 'apt install libcurl4-gnutls-dev'")
endif()

add_library(ecc ecc.cpp)
add_library(op op.cpp)
add_library(script script.cpp)
add_library(tx tx.cpp)
add_library(utils utils.cpp)

add_library(mybitcoin ecc op script tx utils)
target_link_libraries(mybitcoin mycrypto curl boost_random )


set_target_properties(mybitcoin PROPERTIES PUBLIC_HEADER "ecc.h;op.h;script.h;tx.h;utils.h;")

install(TARGETS mybitcoin 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/mybitcoin
)
