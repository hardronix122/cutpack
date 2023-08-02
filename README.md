# Cutpack
A tool for repacking Tony Hawk's Underground 1/2 .cut/.cut.xbx cutscenes!

# Usage
### Flags:

`-x` - eXtract cutscene
`-p` - Pack cutscene
`-i` - Input path
`-o` - Output path
`-v` - Verbose

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

All you need is just GCC 13.1.1 and CMake 3.27 or higher ;3

```
git clone https://github.com/hardronix122/cutpack.git
cd cutpack
mkdir build
cd build
cmake ..
make
```

### Windows:
1. Install msys2 (https://github.com/msys2/msys2-installer/releases/)
2. Clone/download this repository
3. Run MSYS2 MINGW64
4. Execute `pacman -S base-devel gcc cmake`
5. Switch to cutpack directory (`cd [path]`)
6. Execute `mkdir build`
7. Execute `cd build`
8. Execute `cmake -DCMAKE_CXX_COMPILER=g++ ..`
9. Execute `make`