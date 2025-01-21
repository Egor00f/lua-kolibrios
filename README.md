
This is port for KolibriOS of Lua 5.4.7

For installation instructions, license details, and
further information about Lua, see `doc/readme.html`.

## Build

for build you need install gcc toolchain([installer](https://github.com/Egor00f/kolibrios-gcc-toolchain))

### Shared build
```
make shared
```

### Static build
```
make static
```

### Static build with syscalls library
```
make static-with-syscalls
```

### Shared syscalls library
```
make syscalls.dll
```
