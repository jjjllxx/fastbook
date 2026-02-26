find_package(benchmark REQUIRED)

set(BENCH_TARGET fb_bench)

set(FB_BENCH
    MatchingEngine_Bench.cpp
)

add_executable(${BENCH_TARGET}
    ${FB_BENCH}
    ${FB_CORE}
    main.cpp
)

target_link_libraries(${BENCH_TARGET} PRIVATE
    benchmark::benchmark)

target_include_directories(${BENCH_TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}
)

set_target_properties(${BENCH_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build
)