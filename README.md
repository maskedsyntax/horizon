# Horizon
A modern multi-monitor management tool, replacing arandr, built with Qt and C++.

## Installation (Ubuntu/Debian)

You can install Horizon directly from our APT repository:

```bash
# 1. Add the repository
echo "deb [trusted=yes] https://maskedsyntax.github.io/horizon ./" | sudo tee /etc/apt/sources.list.d/horizon.list

# 2. Update and install
sudo apt update
sudo apt install horizon
```

## Building from Source

### Dependencies
* Qt 6
* CMake
* Ninja (optional)
* C++17 compiler

### Build
```bash
cmake -B build -S .
cmake --build build
```