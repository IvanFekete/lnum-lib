# lnum-lib
The library for performing arithmetic operations with long numbers in C++

`Lnum` lives in the `lnum` namespace and is now a header-only library at
`include/lnum/lnum.hpp` — no build or link step required to use it.

```cpp
#include <lnum/lnum.hpp>

lnum::Lnum a("123456789123456789");
lnum::Lnum b = 42;
lnum::Lnum c = a * b + lnum::lPow(2LL, 64);
```

## Getting it into your project

**1. Just copy the header.** `include/lnum/lnum.hpp` has no dependencies beyond
the standard library, so dropping it into your own include path works.

**2. CMake `FetchContent` / `add_subdirectory`.**

```cmake
include(FetchContent)
FetchContent_Declare(lnum-lib GIT_REPOSITORY <this-repo-url> GIT_TAG master)
FetchContent_MakeAvailable(lnum-lib)

target_link_libraries(your_target PRIVATE lnum::lnum-lib)
```

(`add_subdirectory(path/to/lnum-lib)` followed by the same
`target_link_libraries` line works the same way for a vendored copy.)

**3. Install it and use `find_package`.**

```sh
cmake -S . -B build
cmake --install build --prefix /usr/local   # or any prefix on CMAKE_PREFIX_PATH
```

Then, from an unrelated CMake project:

```cmake
find_package(lnum-lib CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE lnum::lnum-lib)
```

Since it's header-only, "linking" just adds the include path — there's no
compiled binary or ABI to match.

Packaging this as a vcpkg port or Conan recipe on top of this CMake layout is
a natural next step, but isn't set up yet.
