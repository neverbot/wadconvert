
# wadconvert

This is a tool to convert WAD files to JSON format and other formats.

**Work in progress**: currently the tool converts `vertices`, `linedefs`, `sidedefs`, `sectors` and `things` to JSON format. 

## Tools needed

The tool is built with C++ and uses the `conan` package manager to manage dependencies and `make` to build the project.

- [Conan](https://conan.io/) - C++ package manager
- [Make](https://www.gnu.org/software/make/) - Build automation tool
- [CLang](https://clang.llvm.org/) - C/C++ compiler

At the moment I'm testing the tool on MacOS. Please tell me if you have any issues or you make it work with other platforms. Issues and comments are welcome.

## Installation

```bash
# this will create the conan profile and will install the needed libraries
make install
# compile
make
```

## Usage

```bash
wadconvert -<format> <input.wad> <output.json>
```

Accepted formats are:

- `json`: JSON format
- `jsonverbose`: JSON format with more verbose object names
- `dsl`: Domain Specific Language format (custom)
- `dslverbose`: Domain Specific Language format with more verbose object names (custom)

The later two formats are not standard, completely custom for my own use. The JSON format is more useful and maybe could be of use for other people.

## WAD file structure

A WAD file has three main parts:

1. Header (12 bytes):

 • identification (4 bytes): "IWAD" or "PWAD"
 • numlumps (4 bytes): Number of lumps (entries)
 • infotableofs (4 bytes): Offset in the file where the directory starts

2. Lumps:

 • These are the raw data blocks that include things like THINGS, VERTEXES, LINEDEFS, SECTORS, sound data, images, etc.

3. Directory:

 • A list of fixed-size entries (16 bytes each) at the end or near-end of the file.
 • Each entry has:
 • filepos (4 bytes): Offset of the lump in the file
 • size (4 bytes): Size of the lump in bytes
 • name (8 bytes): ASCII name of the lump (null-padded)


## File outputs

### Brief JSON structure `-json`

This is the smaller file size that is still human readable. It is a more compact version of the verbose JSON structure. The keys are shortened to save space, but the structure is still clear.

```json
{
 "v": [
  {"x":0,"y":0},
  {"x":128,"y":0},
  ...
  ],
 "l": [
  {"s":0,"e":1,"f":0,"t":0,"g":0,"r":-1,"l":-1},
  {"s":1,"e":2,"f":0,"t":0,"g":0,"r":-1,"l":-1},
  ...
 ],
 "si": [
  {"l":"-","m":"STARTAN2","s":0,"u":"-","x":0,"y":0},
  {"l":"STARTAN2","m":"-","s":0,"u":"STARTAN2","x":0,"y":0},
  ...
 ],
 "se": [
  {"c":128,"f":128,"g":"FLOOR0_1","l":"CEIL1_1","t":"FLOOR1_1","x":"CEIL4_1","y":"FLAT14"},
  {"c":128,"f":128,"g":"FLOOR0_1","l":"CEIL1_1","t":"FLOOR1_1","x":"FLAT14","y":"FLAT14"},
  ...
 ],
 "t": [
  {"a":90,"f":7,"t":"PlayerStart","x":512,"y":256},
  {"a":180,"f":7,"t":"PlayerStart","x":1024,"y":512},
  ...
 ]
}

```json
{
 "v": [
  {"x":-9344,"y":7104},
  {"x":-9344,"y":7552},
  ...
  ],
 "l": [
  {"e":18,"f":1,"g":0,"l":65535,"r":5,"s":19,"t":0},
  {"e":17,"f":1,"g":0,"l":65535,"r":4,"s":18,"t":0},
  ...
 ],
 "si": [
  {"l":"-","m":"ZZWOLF9","s":0,"u":"-","x":0,"y":0},
  {"l":"ZZWOLF9","m":"-","s":0,"u":"ZZWOLF9","x":0,"y":0},
  ...
 ],
 "se": [
  {"c":300,"f":0,"g":0,"l":200,"t":"FLOOR1_1","x":"CEIL4_1","y":0},
  {"c":264,"f":200,"g":0,"l":190,"t":"FLOOR1_1","x":"FLAT14","y":0},  
  ...
 ],
 "t": [
  {"a":90,"f":7,"t":1,"x":-9024,"y":7072},
  {"a":0,"f":7,"t":14,"x":-8224,"y":6112},
  ...
 ]
}
```

### Verbose JSON structure `-jsonverbose`

```json
{
  "vertices": [
    {"x": 0, "y": 0},
    {"x": 128, "y": 0},
    {"x": 128, "y": 128}
  ],
  "linedefs": [
    {
      "start": 0,
      "end": 1,
      "flags": 0,
      "type": 0,
      "tag": 0,
      "right_sidedef": 0,
      "left_sidedef": -1
    }
  ],
  "sidedefs": [
    {
      "x_offset": 0,
      "y_offset": 0,
      "upper_texture": "-",
      "lower_texture": "-",
      "middle_texture": "STARTAN2",
      "sector": 0
    }
  ],
  "sectors": [
    {
      "floor_height": 0,
      "ceiling_height": 128,
      "floor_texture": "FLOOR0_1",
      "ceiling_texture": "CEIL1_1",
      "light_level": 160,
      "type": 0,
      "tag": 0
    }
  ],
  "things": [
    {
      "x": 512,
      "y": 256,
      "angle": 90,
      "type": "PlayerStart",
      "flags": 0
    }
  ]
}
```

### Custom DSL structure `-dsl`

```txt
LEVEL START
VERTICES:
(-9344, 7104)
(-9344, 7552)
...

LINEDEFS:
19 -> 18 | flags: 1 | type: 0 | tag: 0 | right: 5 | left: 65535
18 -> 17 | flags: 1 | type: 0 | tag: 0 | right: 4 | left: 65535
...

SECTORS:
floor: 0 | ceil: 300 | light: 200 | floor_tex: FLOOR1_1 | ceil_tex: CEIL4_1
floor: 200 | ceil: 264 | light: 190 | floor_tex: FLOOR1_1 | ceil_tex: FLAT14
...

THINGS:
PlayerStart at (-9024, 7072) | angle: 90 | type: 1
Thing at (-8224, 6112) | angle: 0 | type: 14
Thing at (-5728, 5984) | angle: 180 | type: 14
...

LEVEL END
```
