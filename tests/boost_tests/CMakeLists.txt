
set(BOOST_ROOT "C:/Program Files/Boost")
set(Boost_USE_STATIC_LIBS ON)

set( SHARED_PTR_TESTS ${PROJECT_SOURCE_DIR}/tests/boost_tests/shared_ptr_tests.cpp )

set( SHARED_PTR_INCLUDES ${PROJECT_SOURCE_DIR}/src/include )

include_directories(${SHARED_PTR_INCLUDES})

add_executable( shared_ptr_test ${SHARED_PTR_TESTS} ${SHARED_PTR_INCLUDES})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(shared_ptr_test
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )

add_test(SharedPtrTest shared_ptr_test)
