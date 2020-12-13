#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "stopwatch.h"

#include <chrono>
#include <thread>
#include <string_view>

namespace
{
template <typename Measured, typename Expected>
bool IsTimeWithinBounds(const Measured& measuredTime, const Expected& expectedTime) noexcept
{
    constexpr auto epsilon = std::chrono::milliseconds{ 25 };
    const auto expected = std::chrono::duration_cast<std::chrono::milliseconds>(expectedTime);
    const auto measured = std::chrono::duration_cast<std::chrono::milliseconds>(measuredTime);

    return (measured >= expected - epsilon) && (measured <= expected + epsilon);
}

void SleepForOneSecond() noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
}
} // namespace

TEST_CASE("Timing Code Execution")
{
    SECTION("Using a Lambda")
    {
        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(
            [] { std::this_thread::sleep_for(std::chrono::seconds{ 1 }); });

        const auto elapsed = stopwatch.GetElapsedTime();
        const auto expected = std::chrono::milliseconds{ 1000 };

        REQUIRE(IsTimeWithinBounds(elapsed, expected));
    }

    SECTION("Using a std::function")
    {
        const std::function<void(void)> function = [] {
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        };

        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(function);

        const auto elapsed = stopwatch.GetElapsedTime();
        const auto expected = std::chrono::milliseconds{ 1000 };

        REQUIRE(IsTimeWithinBounds(elapsed, expected));
    }

    SECTION("Using a Bound Function")
    {
        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(std::bind(&SleepForOneSecond));

        const auto elapsed = stopwatch.GetElapsedTime();
        const auto expected = std::chrono::milliseconds{ 1000 };

        REQUIRE(IsTimeWithinBounds(elapsed, expected));
    }

    SECTION("Using a Function Pointer")
    {
        void (*FunctionPtr)(void) = &SleepForOneSecond;

        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(FunctionPtr);

        const auto elapsed = stopwatch.GetElapsedTime();
        const auto expected = std::chrono::milliseconds{ 1000 };

        REQUIRE(IsTimeWithinBounds(elapsed, expected));
    }
}

TEST_CASE("Units as Strings")
{
    const auto dummyTask = [] { std::this_thread::sleep_for(std::chrono::seconds{ 1 }); };

    SECTION("Nanoseconds")
    {
       const auto stopwatch = Stopwatch<std::chrono::nanoseconds>(dummyTask);
       REQUIRE(stopwatch.GetUnitsAsString() == std::string_view{ "nanoseconds" });
    }

    SECTION("Microseconds")
    {
       const auto stopwatch = Stopwatch<std::chrono::microseconds>(dummyTask);
       REQUIRE(stopwatch.GetUnitsAsString() == std::string_view{ "microseconds" });
    }

    SECTION("Milliseconds")
    {
       const auto stopwatch = Stopwatch<std::chrono::milliseconds>(dummyTask);
       REQUIRE(stopwatch.GetUnitsAsString() == std::string_view{ "milliseconds" });
    }

    SECTION("Seconds")
    {
       const auto stopwatch = Stopwatch<std::chrono::seconds>(dummyTask);
       REQUIRE(stopwatch.GetUnitsAsString() == std::string_view{ "seconds" });
    }

    SECTION("Minutes")
    {
       const auto stopwatch = Stopwatch<std::chrono::minutes>(dummyTask);
       REQUIRE(stopwatch.GetUnitsAsString() == std::string_view{ "minutes" });
    }
}
