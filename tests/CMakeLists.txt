# Find GTest and GMock
find_package(GTest QUIET)
find_package(GMock QUIET)

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
    core/test_order.cpp
    core/test_position.cpp
    core/test_contract.cpp
    core/test_portfolio.cpp
    # core/test_engine.cpp
    # core/test_risk_manager.cpp

    # Models tests
    models/test_black_scholes_model.cpp
    models/test_black_scholes_model_greeks.cpp
    # models/test_implied_volatility.cpp

    # Strategies tests
    # strategies/test_strategy_base.cpp
    # strategies/test_strategy_registry.cpp

    # Data tests
    data/test_ib_client.cpp
    # data/test_market_data.cpp
    # data/test_data_manager.cpp

    # Backtesting tests
    # backtesting/test_backtest_engine.cpp
    # backtesting/test_performance_metrics.cpp

    # Utils tests
    utils/test_symbol_lookup.cpp
    # utils/test_logger.cpp
    # utils/test_config.cpp
    # utils/test_time_utils.cpp
)

# Add test executable
add_executable(thales_tests ${TEST_SOURCES})

# Link against the individual libraries and GTest
target_link_libraries(thales_tests PRIVATE 
    core 
    data 
    models 
    strategies 
    utils
    twsapi
)

# Link against the appropriate GTest and GMock libraries
if(GTEST_FOUND)
    target_link_libraries(thales_tests PRIVATE ${GTEST_BOTH_LIBRARIES})
    target_include_directories(thales_tests PRIVATE ${GTEST_INCLUDE_DIRS})
    
    # Try to find GMock as part of GTest
    if(EXISTS "${GTEST_INCLUDE_DIRS}/gmock/gmock.h")
        # GMock headers are in the same directory as GTest
        # Try to find the library
        find_library(GMOCK_LIBRARY gmock PATHS ${GTEST_LIBRARY_DIRS})
        find_library(GMOCK_MAIN_LIBRARY gmock_main PATHS ${GTEST_LIBRARY_DIRS})
        
        if(GMOCK_LIBRARY AND GMOCK_MAIN_LIBRARY)
            target_link_libraries(thales_tests PRIVATE ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY})
        endif()
    endif()
else()
    # When using FetchContent, link against both GTest and GMock
    target_link_libraries(thales_tests PRIVATE gtest gtest_main gmock gmock_main)
endif()

# Add pthread if needed (usually required on Unix-like systems)
find_package(Threads REQUIRED)
target_link_libraries(thales_tests PRIVATE Threads::Threads)

# Add the test
add_test(NAME thales_tests COMMAND thales_tests)
