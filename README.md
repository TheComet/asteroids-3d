Asteroids 3D
============

A 3D remake of the classic game "Asteroids".

Building
========

```sh
# First, clone the necessary repos and create the basic folder structure:
mkdir asteroids-3d && cd asteroids-3d
export PREFIX=$(pwd)
git clone git@github.com:thecomet/asteroids-3d.git asteroids-git
git clone git@github.com:urho3d/urho3d.git Urho3D-git

# Next, build Urho3D and install it:
mkdir Urho3D-build-debug && cd Urho3D-build-debug
cmake -DCMAKE_BUILD_TYPE=DEBUG \
      -DCMAKE_INSTALL_PREFIX=$PREFIX/Urho3D-debug \
      -DURHO3D_SAMPLES=OFF \
      -DURHO3D_LIB_TYPE=SHARED \
      $PREFIX/Urho3D-git
make -j
make install

# Now you can build Asteroids 3D:
cd $PREFIX
mkdir asteroids-build-debug && cd asteroids-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DURHO3D_HOME=$PREFIX/Urho3D-debug \
      $PREFIX/asteroids-git


# All executables will be placed in bin/
# Asteroids can be tested by first starting the server and then
# starting the client and connecting to localhost.
cd bin
./asteroids-server &
./asteroids-client &

```

