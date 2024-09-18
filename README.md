# live

A complete RTSP server application.

# Build and Run

```bash
# Configure the project
cmake --preset=amd64-linux-static-release
# Build the project
cmake --build --preset=amd64-linux-static-release
# Run the project
cmake --build --preset=amd64-linux-static-release --target=run
```

# Supported presets
- `amd64-linux-dynamic-debug` - build for amd64 linux with dynamic linking and debug symbols
- `amd64-linux-dynamic-release` - build for amd64 linux with dynamic linking
- `amd64-linux-static-debug` - build for amd64 linux with static linking and debug symbols
- `amd64-linux-static-release` - build for amd64 linux with static linking
- `arm64-linux-dynamic-debug` - build for arm64 linux with dynamic linking and debug symbols
- `arm64-linux-dynamic-release` - build for arm64 linux with dynamic linking
- `arm64-linux-static-debug` - build for arm64 linux with static linking and debug symbols
- `arm64-linux-static-release` - build for arm64 linux with static linking
