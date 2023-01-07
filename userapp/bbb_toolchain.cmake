# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C
set(CMAKE_C_COMPILER    /usr/x-tools/arm-cortex_a8-linux-gnueabihf-debian/bin/arm-cortex_a8-linux-gnueabihf-gcc)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /usr/x-tools/arm-cortex_a8-linux-gnueabihf-debian/arm-cortex_a8-linux-gnueabihf/sysroot)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)