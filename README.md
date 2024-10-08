# EmbedLog

![CMake](https://github.com/joeinman/EmbedLog/actions/workflows/cmake.yml/badge.svg)

EmbedLog is a lightweight hardware agnostic logging library designed for embedded systems.

## Features:

- **Multiple Log Levels**: Supports logging at various severity levels (INFO, WARNING, ERROR, DEBUG).
- **Lightweight**: Designed to keep the memory footprint low, making it suitable for embedded applications.
- **Customizable**: The library can be easily customized to work with different microcontrollers by providing the appropriate output and timestamp functions.

## Basic Example:

```cpp
// Imports
#include <EmbedLog/EmbedLog.hpp>
#include <pico/stdlib.h>
#include <stdio.h>
#include <memory>

using namespace EmbedLog;
std::unique_ptr<EmbedLog::EmbedLog> client_logger;

int main() {
    client_logger = std::make_unique<EmbedLog::EmbedLog>(
        []() { return stdio_init_all(); },
        []() { return stdio_deinit_all(); },
        [](const std::string& message) { printf("%s", message.c_str()); },
        []() { return to_us_since_boot(get_absolute_time()); },
        "Logger",
        "[%H:%M:%S] [%N] [%L] - %T"
    );

    // Open The Logger
    client_logger->open();
    
    int x = 1, y = 2;

    while (1)
    {
        // Log Coordinates Every 5 Seconds
        client_logger->log_throttled(EMBDLID, 5000, INFO, "Coordinates: (%d, %d)", x, y);

        // Log "Hello, World!" Every Second
        client_logger->log(WARNING, "Hello, World!");
        sleep_ms(1000);
    }

    // Close The Logger
    client_logger->close();
}
```
