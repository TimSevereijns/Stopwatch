#pragma once

#define CATCH_CONFIG_MAIN
#include "Catch.hpp"

#include "../Stopwatch/Stopwatch.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace
{
   const auto ONE_SECOND = std::chrono::milliseconds(1000);

   const auto isMeasuredTimeWithinBounds =
      [](const auto measuredTime, const auto expectedTime) -> bool
   {
      const auto marginOfError{ 1 };

      return 
         (measuredTime >= expectedTime - marginOfError) &&
         (measuredTime <= expectedTime + marginOfError);
   };
}

TEST_CASE("Timing Code Blocks and Outputting Message to Buffer")
{
   std::stringstream buffer;
   std::streambuf* oldBuffer = std::cout.rdbuf(buffer.rdbuf());

   SECTION("Using a Lambda and the Default Message Formatting")
   {
      const char* const message = "Slept for ";

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
      REQUIRE(isMeasuredTimeWithinBounds(stoi(results[2]), ONE_SECOND.count()));
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

      REQUIRE(isMeasuredTimeWithinBounds(elapsedTime, static_cast<std::uint64_t>(ONE_SECOND.count())));
      REQUIRE(units == std::string{ "milliseconds" });
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
      REQUIRE(isMeasuredTimeWithinBounds(stoi(results[2]), ONE_SECOND.count()));
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

   REQUIRE(isMeasuredTimeWithinBounds(elapsedTime, static_cast<std::uint64_t>(ONE_SECOND.count())));
}
