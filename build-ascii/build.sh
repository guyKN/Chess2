echo "Lines of code:"
git ls-files ../include ../src | xargs wc -l | grep total
cmake .. -D WASM=OFF -D GENERATE_SEEDS=OFF || exit 1
make || exit 1
#sleep 1.5
./chessAscii || exit 1
