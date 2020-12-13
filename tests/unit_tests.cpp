#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "stopwatch.h"

#include <algorithm>
#include <functional>
#include <string>
#include <thread>

namespace
{
template <typename Measured, typename Expected>
bool IsTimeWithinBounds(const Measured& measuredTime, const Expected& expectedTime) noexcept
{
    constexpr auto epsilon = std::chrono::milliseconds{ 50 };
    const auto expected = std::chrono::duration_cast<std::chrono::milliseconds>(expectedTime);
    const auto measured = std::chrono::duration_cast<std::chrono::milliseconds>(measuredTime);

    return (measured >= expected - epsilon) && (measured <= expected + epsilon);
}

void SleepForOneSecond()
{
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
}
} // namespace

TEST_CASE("Timing Code Execution")
{
    SECTION("Using a Lambda")
    {
        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(
            [&] { std::this_thread::sleep_for(std::chrono::seconds{ 1 }); });

        const bool withinBounds =
            IsTimeWithinBounds(stopwatch.GetElapsedTime(), std::chrono::milliseconds{ 1000 });

        REQUIRE(withinBounds);
    }

    SECTION("Using a Bound Function")
    {
        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(std::bind(&SleepForOneSecond));

        const bool withinBounds =
            IsTimeWithinBounds(stopwatch.GetElapsedTime(), std::chrono::milliseconds{ 1000 });

        REQUIRE(withinBounds);
    }

    SECTION("Using a Function Pointer")
    {
        void (*FunctionPtr)(void) = &SleepForOneSecond;

        const auto stopwatch = Stopwatch<std::chrono::milliseconds>(FunctionPtr);

        const bool withinBounds =
            IsTimeWithinBounds(stopwatch.GetElapsedTime(), std::chrono::milliseconds{ 1000 });

        REQUIRE(withinBounds);
    }
}
