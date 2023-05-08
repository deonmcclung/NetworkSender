# NetworkSender
Sample Socket Code in C++

Build by executing make in the repository root:

**CMake installed:**

mkdir build
cd build
cmake ..
make


**w/o CMake**
make


The binaries will be built into the repository root.

The receiver executable should be run first, but the sender will retry for a few seconds if it cannot connect immediately.

Ctrl-C to exit the receiver.

`./receiver`

`./sender test.txt`

or

`cat test.txt | ./sender -`

**Running the Unit Tests**
The unit tests require CMake to build.
With CMake installed,

`git submodule init`
`git submodule update`

(follow CMake build instructions)

From the build directory, after building, execute:

ctest
