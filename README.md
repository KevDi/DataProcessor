#Compilation Instruction#

I used conan for dependency management but the CMakeLists.txt also uses the `FindPackage` Call to search for the Dependencies.
1. `conan install .`
2. `cmake --preset=conan-release` the preset could also be named `conan-default` or `conan-debug`
3. `cmake --build --preset=conan-release` 
4. Test could be executed with `ctest --preset=conan-release`