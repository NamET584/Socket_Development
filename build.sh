BUILD_DIR="build"
INSTALL_DIR="install"

PROJECT_ROOT="$(pwd)"

rm -rf $INSTALL_DIR
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_INSTALL_PREFIX="$PROJECT_ROOT/$INSTALL_DIR" \
    -DCMAKE_C_COMPILER="gcc" \
    -DCMAKE_CXX_COMPILER="g++" 

echo "🔨 Compiling project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
else
    echo "✅ Build succeeded!"
fi

# Install the project
echo "📦 Installing project..."
make install

cd ..

echo "✅ Build completed successfully!"
echo "📁 Executable location: $INSTALL_DIR/..."