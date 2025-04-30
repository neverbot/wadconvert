
# wadconvert

This is a tool to convert WAD files to JSON format and other formats.

**Work in progress**: currently the tool converts `vertices`, `linedefs`, `sidedefs`, `sectors` and `things` to JSON format. 

The tool is built with C++ and uses the `conan` package manager to manage dependencies.

## Installation

```bash
conan profile detect
make
```

## Usage

```bash
wadconvert <input.wad> <output.json>
```

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


## JSON structure

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
