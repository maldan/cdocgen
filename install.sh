mkdir build
cd build
cmake ..
cmake --build .
cd ..
mkdir -p /var/lib/cdocgen
cp -R resource /var/lib/cdocgen
cp build/cdocgen /usr/bin/cdocgen