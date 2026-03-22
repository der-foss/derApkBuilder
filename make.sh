cmake -B .build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_BUILD_TYPE=Debug
cmake --build .build
cp .build/compile_commands.json .

mv .build/derapkbuilder .
