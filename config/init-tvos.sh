cd ..
mkdir -p build && mkdir -p build/tvos && cd build/tvos && cmake -Wno-dev -G "Xcode" -DENABLE_VISIBILITY=ON -DCMAKE_TOOLCHAIN_FILE=../../RavEngine/deps/ios/ios.toolchain.cmake -DENABLE_ARC=OFF -DDEPLOYMENT_TARGET=14.0 -DPLATFORM=TVOSCOMBINED ../..
