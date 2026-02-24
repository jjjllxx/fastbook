find_package(GTest REQUIRED)

set(TEST_TARGET fb_test)

set(FB_TEST
    MatchingEngine_Test.cpp  
)

add_executable(${TEST_TARGET}
    ${FB_CORE}
    ${FB_TEST}
    main.cpp
)

target_link_libraries(${TEST_TARGET} PRIVATE
    GTest::gtest)

target_include_directories(${TEST_TARGET} PRIVATE 
    ${PROJECT_SOURCE_DIR}
)

set_target_properties(${TEST_TARGET} PROPERTIES
     RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build
)