
conan install . --output-folder=build --build=missing --profile:host=default
cd build && cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
cd build && make