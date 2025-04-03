# Find GTest
find_package(GTest QUIET)

# Use FetchContent as a fallback if GTest is not found
if(NOT GTEST_FOUND)
    message(STATUS "System GTest not found, fetching from source")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # Prevent overriding parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Define test sources
set(TEST_SOURCES
    # Core tests
    # test_engine.cpp
    # test_risk_manager.cpp
    # test_portfolio.cpp
    
    # Models tests
    test_black_scholes.cpp
    # test_greeks.cpp
    # test_implied_volatility.cpp
    
    # Strategies tests
    # test_strategy_base.cpp
    # test_strategy_registry.cpp
    
    # Data tests
    # test_market_data.cpp
    # test_ib_client.cpp
    # test_data_manager.cpp
    
    # Backtesting tests
    # test_backtest_engine.cpp
    # test_performance_metrics.cpp
    
    # Utils tests
    # test_logger.cpp
    # test_config.cpp
    # test_time_utils.cpp
)

# Add test executable
add_executable(thales_tests ${TEST_SOURCES})

# Link against the library and GTest
target_link_libraries(thales_tests PRIVATE thales_lib)

# Link against the appropriate GTest libraries
if(GTEST_FOUND)
    target_link_libraries(thales_tests PRIVATE ${GTEST_BOTH_LIBRARIES})
    target_include_directories(thales_tests PRIVATE ${GTEST_INCLUDE_DIRS})
else()
    target_link_libraries(thales_tests PRIVATE gtest gtest_main)
endif()

# Add pthread if needed (usually required on Unix-like systems)
find_package(Threads REQUIRED)
target_link_libraries(thales_tests PRIVATE Threads::Threads)

# Add the test
add_test(NAME thales_tests COMMAND thales_tests)
