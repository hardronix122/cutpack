# Cutpack
A tool for repacking Tony Hawk's Underground 1/2 .cut/.cut.xbx cutscenes!

![Repacked New Orleans Event Cutscene](gif/flaming-felines.gif)

# Usage
### Flags:

+ `-x` - eXtract cutscene
+ `-p` - Pack cutscene
+ `-t` - checksum Table path
+ `-i` - Input path
+ `-o` - Output path
+ `-v` - Verbose

### Extraction:
`cutpack -x -i somecutscene.cut.xbx -o outputdirectory`

`-o` is optional, cutscene will be extracted into the directory with the same name if not specified

Example: somecutscene.cut.xbx -> comecutscene.cut.xbx_data
### Pack:
`cutpack -p -i outputdirectory -o somecutscene.cut.xbx`

Specifying output is mandatory for pack action

(I don't want to spawn lots of garbage which goes like scene.cut.xbx_data.cut.xbx lmao)

# Building

### Linux
Here, process of building cutpack is pretty straightforward!

All you need is just GCC 13.1.1 and CMake 3.25 or higher ;3

```
git clone https://github.com/hardronix122/cutpack.git
cd cutpack
mkdir build
cd build
cmake ..
make
```

### Windows:
On Windows, it's a bit annoying but still bearable UwU

The resulting exe will be a bit large, but it won't require any libraries ;3

1. Download Cygwin
2. Clone/download this repository
3. Run Cygwin setup
4. During setup, find and install `mingw64-x86_64-gcc-core`, `mingw64-x86_64-gcc-g++`, `cmake` and `make`
5. Open Cygwin
6. Switch to cutpack directory (`cd [path]`)
7. Execute `mkdir build`
8. Execute `cd build`
9. Execute `cmake -DCMAKE_CXX_COMPILER="x86_64-w64-mingw32-g++" -DCMAKE_C_COMPILER="x86_64-w64-mingw32-gcc" -DCMAKE_EXE_LINKER_FLAGS="-static" ..`
10. Execute `make`