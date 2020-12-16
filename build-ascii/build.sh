cmake .. -D WASM=OFF || exit 1
make || exit 1
#sleep 1.5
./chessAscii