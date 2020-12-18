echo "Lines of code:"
git ls-files ../include ../src | xargs wc -l | grep total
emcmake cmake .. -D WASM=ON || (echo "Error With emcmake" && exit 1)
make || echo "Error With make" && exit 1
