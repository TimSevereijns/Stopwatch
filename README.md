# Stopwatch

[![codecov](https://codecov.io/gh/TimSevereijns/Stopwatch/branch/master/graph/badge.svg?token=AwpiiHgIg5)](https://codecov.io/gh/TimSevereijns/Stopwatch)

`Stopwatch` is a header-only class that encapsulates the boilerplate needed to time a block of code using the standard `std::chrono` infrastructure.

# Example

Without further ado, here's a basic example of how the class might be used.

```C++
const auto stopwatch = Stopwatch<std::chrono::milliseconds>([&] { FunctionToBeTimed(); });

std::cout << "Task completed in " << stopwatch.GetElapsedTime().count() << " "
          << stopwatch.GetUnitsAsString() << ".\n";
```

The snippet above will result in the following output being printed to the console, assuming that `FunctionToBeTimed()` completed in exactly one second:

```
Task completed in 1000 milliseconds.
```

The following `std::chrono` types are support as units of measurement:

* `std::chrono::nanoseconds`
* `std::chrono::microseconds`
* `std::chrono::milliseconds`
* `std::chrono::seconds`
* `std::chrono::minutes`
* `std::chrono::hours`

That's it, folks.
