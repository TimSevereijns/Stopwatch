#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "stopwatch.h"
#include "scope_exit.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace
{
    constexpr auto ONE_SECOND = std::chrono::milliseconds{ 1000 };

   const auto IsTimeWithinBounds =
      [] (const std::uint64_t measuredTime, const std::uint64_t expectedTime) noexcept
   {
      constexpr std::uint64_t marginOfError = 50ull;

      return
         (measuredTime >= expectedTime - marginOfError) &&
         (measuredTime <= expectedTime + marginOfError);
   };

   void SleepForOneSecond()
   {
      std::this_thread::sleep_for(ONE_SECOND);
   }
}

TEST_CASE("Timing Code Execution")
{
   std::stringstream buffer;
   auto* oldBuffer = std::cout.rdbuf(buffer.rdbuf());

   ON_SCOPE_EXIT{ std::cout.rdbuf(oldBuffer); };

   SECTION("Saving the Elapsed Time")
   {
      const auto elapsedTime = Stopwatch<std::chrono::milliseconds>([&]
      {
         std::this_thread::sleep_for(std::chrono::seconds{ 1 });
      }).GetElapsedTime();

      const bool withinBounds = IsTimeWithinBounds(elapsedTime.count(), std::chrono::milliseconds{ 1000 }.count());

      REQUIRE(withinBounds);
   }

   SECTION("Outputting the Elapsed Time to the Default Output Stream")
   {
      constexpr const char* const message = "Slept for ";

      Stopwatch<std::chrono::milliseconds>([&]
      {
         std::this_thread::sleep_for(ONE_SECOND);
      }, message);

      std::istream_iterator<std::string> itr{ buffer };
      std::istream_iterator<std::string> end;
      std::vector<std::string> results{ itr, end };

      REQUIRE(results.size() == 4);
      REQUIRE(std::string{ "Slept" } == results[0]);
      REQUIRE(std::string{ "for" } == results[1]);

      const bool withinBounds =
         IsTimeWithinBounds(stoi(results[2]), static_cast<std::uint64_t>(ONE_SECOND.count()));

      REQUIRE(withinBounds);
      REQUIRE(std::string{ "milliseconds." } == results[3]);
   }

   SECTION("Outputting the Elapsed Time to a User Specified Output Stream")
   {
      std::stringstream customBuffer;

      constexpr const char* const message = "Slept for ";

      Stopwatch<std::chrono::milliseconds>([&]
      {
         std::this_thread::sleep_for(ONE_SECOND);
      }, message, customBuffer);

      std::istream_iterator<std::string> itr{ customBuffer };
      std::istream_iterator<std::string> end;
      std::vector<std::string> results{ itr, end };

      REQUIRE(results.size() == 4);
      REQUIRE(std::string{ "Slept" } == results[0]);
      REQUIRE(std::string{ "for" } == results[1]);

      const bool withinBounds =
         IsTimeWithinBounds(stoi(results[2]), static_cast<std::uint64_t>(ONE_SECOND.count()));

      REQUIRE(withinBounds);
      REQUIRE(std::string{ "milliseconds." } == results[3]);
   }

   SECTION("Handling the Elapsed Time with a Callback")
   {
      std::chrono::milliseconds elapsedTime;
      std::string chronoUnits;

      Stopwatch<std::chrono::milliseconds>([&]
      {
         std::this_thread::sleep_for(ONE_SECOND);
      }, [&] (auto elapsed, auto units)
      {
         elapsedTime = elapsed;
         chronoUnits = std::move(units);
      });

      const bool withinBounds = IsTimeWithinBounds(elapsedTime.count(), ONE_SECOND.count());

      REQUIRE(withinBounds);
      REQUIRE(chronoUnits == std::string{ "milliseconds" });
   }

   SECTION("Timing a Bound Function")
   {
      constexpr const char* const message = "Slept for ";

      Stopwatch<std::chrono::milliseconds>(std::bind(&SleepForOneSecond), message);

      std::istream_iterator<std::string> itr{ buffer };
      std::istream_iterator<std::string> end;
      std::vector<std::string> results{ itr, end };

      REQUIRE(results.size() == 4);
      REQUIRE(std::string{ "Slept" } == results[0]);
      REQUIRE(std::string{ "for" } == results[1]);

      const bool withinBounds =
         IsTimeWithinBounds(stoi(results[2]), static_cast<std::uint64_t>(ONE_SECOND.count()));

      REQUIRE(withinBounds);
      REQUIRE(std::string{ "milliseconds." } == results[3]);
   }

   SECTION("Time a Function using a Function Pointer")
   {
      constexpr const char* const message = "Slept for ";

      void(*FunctionPtr)(void) = &SleepForOneSecond;

      Stopwatch<std::chrono::milliseconds>(FunctionPtr, message);

      std::istream_iterator<std::string> itr{ buffer };
      std::istream_iterator<std::string> end;
      std::vector<std::string> results{ itr, end };

      REQUIRE(results.size() == 4);
      REQUIRE(std::string{ "Slept" } == results[0]);
      REQUIRE(std::string{ "for" } == results[1]);

      const bool withinBounds =
         IsTimeWithinBounds(stoi(results[2]), static_cast<std::uint64_t>(ONE_SECOND.count()));

      REQUIRE(withinBounds);
      REQUIRE(std::string{ "milliseconds." } == results[3]);
   }

   SECTION("Using the Convenience Macro")
   {
      TIME_IN_MILLISECONDS(std::this_thread::sleep_for(ONE_SECOND), "Slept for ");

      std::istream_iterator<std::string> itr{ buffer };
      std::istream_iterator<std::string> end;
      std::vector<std::string> results{ itr, end };

      REQUIRE(results.size() == 4);
      REQUIRE(std::string{ "Slept" } == results[0]);
      REQUIRE(std::string{ "for" } == results[1]);

      const bool withinBounds = IsTimeWithinBounds(stoi(results[2]), ONE_SECOND.count());
      REQUIRE(withinBounds);
      REQUIRE(std::string{ "milliseconds." } == results[3]);
   }
}
