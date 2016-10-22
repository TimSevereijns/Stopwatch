# Stopwatch

`Stopwatch` is a header-only class that encapsulates the boilerplate needed to time blocks of code using `std::chrono`.

# Motivation

In timing several aspects of a program I was working on, I got tired of writing all of the boilerplate associated with calculating the time needed to complete one or more function calls. Thinking I could reduce the amount of typing needed to accomplish this task, I came up with this header-only solution.

Without this class, timing a block of code generally looks like this:

```C++
const auto startTime = std::chrono::high_resolution_clock::now();
SomeTask();
const auto endTime = std::chrono::high_resolution_clock::now();
const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
std::cout << " Some task ran in " << duration.count() << std::endl;
```

With this class, the snippet above can be reduced to the following:

```C++
TIME_IN_MILLISECONDS( SomeTask(), "Some task ran in " );
```

# Requirements

A C++11 compliant compiler, like MSVC2015.

# Usage Examples

The `Stopwatch` class can be used in several ways to time a particular block of code. In addition to the three constructors that the `Stopwatch` class provides, there are also several macros that will reduce the boilerplate even further.

Using the `Stopwatch(const std::function<void()>&, const char* const)` constructor, timing information can be easily printed out to the console:

```C++
Stopwatch<std::chrono::milliseconds>([&]
{
  FunctionToBeTimed();
}, "FunctionToBeTimed() ran in ");
```

The snippet above will result in the following output being printed to the console, assuming that `FunctionToBeTimed()` completed in exactly one second:

```
FunctionToBeTimed() ran in 1000 milliseconds.
```

In general, this constructor will output messages of the form: message + time + units. The units used will be derived from the template argument, and the following `std::chrono` units are supported:

* `std::chrono::nanoseconds`
* `std::chrono::microseconds`
* `std::chrono::milliseconds`
* `std::chrono::seconds`
* `std::chrono::minutes`
* `std::chrono::hours`

Should you desire to save the elapsed time to a variable, the `Stopwatch(const std::function<void()>&)` constructor can be used:

```C++
const auto elapsedTime = Stopwatch<std::chrono::seconds>([&]
{
  // Time consuming task...
}).GetElapsedTime();
```

Lastly, if you're really looking to save on the boilerplate, consider using one of the provided macros:

```
TIME_IN_MILLISECONDS( AnotherSlowTask(), "Slow task ran in " );
```

Similar macros are provided for each of the `std::chrono` time units:

* `TIME_IN_NANOSECONDS`
* `TIME_IN_MICROSECONDS`
* `TIME_IN_MILLISECONDS`
* `TIME_IN_SECONDS`
* `TIME_IN_MINUTES`
* `TIME_IN_HOURS`
