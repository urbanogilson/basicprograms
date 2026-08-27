# MyProject

## Build and Run

cmake -S . -B build
cmake --build build

./build/app/myproject_app

## Tests

ctest --test-dir build --output-on-failure
