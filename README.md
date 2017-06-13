# Pawn BIOS Dumping Tool

Copyright 2014-2017 Google Inc.

Disclaimer: This is not an official Google product (experimental or otherwise),
it is just code that happens to be owned by Google.

## What is it?

Pawn is a tool to extract the BIOS firmware from Intel-based workstations and
laptops.
The name is a play on an internal tool that is also named after a chess piece.

## How to Build

Dependencies:
  * Linux (uses /dev/mem)
  * GCC 4.6 or later
  * GNU Make

To build, just run `make`. The resulting binary can be found in `.build`.

## Usage

The following command will extract the BIOS firmware and save the image to
`bios_image.bin`:
```bash
sudo .build/pawn bios_image.bin
``` 

You can then use other tools like
[UEFITool](https://github.com/LongSoft/UEFITool) to process the firmware
image further.
