cmake -B .build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE
cmake --build .build
cp .build/compile_commands.json .