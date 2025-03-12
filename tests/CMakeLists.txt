# Tests CMakeLists.txt

# Find GTest
find_package(GTest)

if(NOT GTEST_FOUND)
    # If GTest is not found, fetch it
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Define test sources
set(TEST_SOURCES
    # Core tests
    # test_engine.cpp
    # test_risk_manager.cpp
    # test_portfolio.cpp
    
    # Models tests
    # test_black_scholes.cpp
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

# Create a placeholder test file if no tests are defined yet
if(NOT TEST_SOURCES)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/placeholder_test.cpp "
        #include <gtest/gtest.h>
        
        TEST(PlaceholderTest, AlwaysPass) {
            EXPECT_TRUE(true);
        }
    ")
    set(TEST_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/placeholder_test.cpp)
endif()

# Add test executable
add_executable(thales_tests ${TEST_SOURCES})

# Link against the library and GTest
if(GTEST_FOUND)
    target_link_libraries(thales_tests PRIVATE
        thales_lib
        ${GTEST_BOTH_LIBRARIES}
    )
    target_include_directories(thales_tests PRIVATE ${GTEST_INCLUDE_DIRS})
else()
    target_link_libraries(thales_tests PRIVATE
        thales_lib
        gtest
        gtest_main
    )
endif()

# Add the test
add_test(NAME thales_tests COMMAND thales_tests)
