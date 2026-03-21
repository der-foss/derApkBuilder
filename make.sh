cmake -B .build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_BUILD_TYPE=Debug
cmake --build .build
cp .build/compile_commands.json .