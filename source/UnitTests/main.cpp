#pragma once

#define CATCH_CONFIG_MAIN
#include "Catch.hpp"

#include "../Stopwatch/Stopwatch.hpp"

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
   constexpr auto ONE_SECOND = std::chrono::milliseconds(1000);

   const auto IsTimeWithinBounds =
      [] (const auto measuredTime, const auto expectedTime) noexcept -> bool
   {
      constexpr auto marginOfError{ 2 };

      return
         (measuredTime >= expectedTime - marginOfError) &&
         (measuredTime <= expectedTime + marginOfError);
   };

   void SleepForOneSecond()
   {
      std::this_thread::sleep_for(ONE_SECOND);
   }
}

TEST_CASE("Timing Code Blocks and Outputting Message to Buffer")
{
   std::stringstream buffer;
   std::streambuf* oldBuffer = std::cout.rdbuf(buffer.rdbuf());

   SECTION("Using a Lambda and the Default Message Formatting")
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

   SECTION("Using one Lambda to Time and Another to Handle Results")
   {
      std::uint64_t elapsedTime;
      std::string units;

      Stopwatch<std::chrono::milliseconds>([&]
      {
         std::this_thread::sleep_for(ONE_SECOND);
      }, [&](std::uint64_t _elapsedTime, const std::string& _units)
      {
         elapsedTime = _elapsedTime;
         units = _units;
      });

      const bool withinBounds =
         IsTimeWithinBounds(elapsedTime, static_cast<std::uint64_t>(ONE_SECOND.count()));

      REQUIRE(withinBounds);
      REQUIRE(units == std::string{ "milliseconds" });
   }

   SECTION("Binding to a Function")
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

   SECTION("Using a Function Pointer")
   {
      constexpr const char* const message = "Slept for ";

      void(*FunctionPtr)(void);
      FunctionPtr = &SleepForOneSecond;

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

   SECTION("Using the Macro")
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

   std::cout.rdbuf(oldBuffer);
}

TEST_CASE("Timing and Saving Milliseconds Elapsed")
{
   const auto elapsedTime = Stopwatch<std::chrono::milliseconds>([&]
   {
      std::this_thread::sleep_for(ONE_SECOND);
   }).GetElapsedTime();

   const bool withinBounds =
      IsTimeWithinBounds(elapsedTime, static_cast<std::uint64_t>(ONE_SECOND.count()));

   REQUIRE(withinBounds);
}
