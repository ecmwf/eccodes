add_executable(metric
    metric.cc
)

add_executable(main
    main.cc
)

add_executable(main_spaec
    main_spaec.cc
)

target_link_libraries(main_spaec
    libaec::aec
)

target_link_libraries(main
    libaec::aec
)

add_executable(main_parallel main_parallel.cc )
target_link_libraries(main_parallel libaec::aec pthread)

add_executable(benchmark benchmark.cc)
target_link_libraries(benchmark libaec::aec)

add_executable(benchmark_spaec benchmark_spaec.cc Message.cc)
target_link_libraries(benchmark_spaec libaec::aec)
