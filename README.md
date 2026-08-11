## Cursor trail effect for KDE Plasma 6

This effect introduces the cursor trail effect from Windows, with the addition of the trails fading out of view. They can be configured in both duration and frequency.

Tested on KDE Plasma 6.7.4 (Wayland), Qt 6.11.1.

![regular](screenshots/regular.gif)

![aura](screenshots/aura.gif)

## Building

### Dependencies

- CMake
- extra-cmake-modules
- Ninja (optional)

```bash
$ cd path/to/this/repo
$ cmake -DCMAKE_INSTALL_PREFIX=/usr -G Ninja -B build . # '-G Ninja' is optional, but recommended for faster compilation
$ cmake --build build
$ sudo cmake --install build
```
