# Common Commands

## Building

```bash
# Standard build (from project root)
cd build && cmake .. && make -j$(nproc)

# Clean rebuild
cd build && rm -rf * && cmake .. && make -j$(nproc)

# Debug build
cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)

# Build with server (Servatrice)
cd build && cmake .. -DWITH_SERVER=1 && make -j$(nproc)

# Build client only (no Oracle)
cd build && cmake .. -DWITH_ORACLE=0 && make -j$(nproc)
```

## Installing / Packaging

```bash
# Local install (into build/release)
cd build && make install

# Create platform installer (DEB/RPM on Linux)
cd build && make package
```

## Testing

```bash
# Enable and run tests
cd build && cmake .. -DTEST=1 && make -j$(nproc) && make test
```

## Code Formatting

```bash
# Format a single file
clang-format -i <filename>

# Use project format script
./format.sh --help
```

## Translations

```bash
# Update translation source files
cd build && cmake .. -DUPDATE_TRANSLATIONS=ON && make
# Then disable after:
cd build && cmake .. -DUPDATE_TRANSLATIONS=OFF
```

## Useful CMake Flags

| Flag | Default | Description |
|------|---------|-------------|
| `-DWITH_SERVER=1` | OFF | Build Servatrice server |
| `-DWITH_CLIENT=0` | ON | Disable Cockatrice client |
| `-DWITH_ORACLE=0` | ON | Disable Oracle tool |
| `-DCMAKE_BUILD_TYPE=Debug` | Release | Debug mode with symbols and logging |
| `-DWARNING_AS_ERROR=0` | ON | Don't treat warnings as errors (debug) |
| `-DTEST=1` | OFF | Enable regression tests |
| `-DFORCE_USE_QT5=1` | OFF | Force Qt5 over Qt6 |
