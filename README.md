# Pawn BIOS Dumping Tool

Copyright 2014-2021 Google LLC.

[![Linux Build Status](https://github.com/google/pawn/workflows/linux-build/badge.svg)](https://github.com/google/pawn/actions?query=workflow%3Alinux-build)

Disclaimer: This is not an official Google product (experimental or otherwise),
it is just code that happens to be owned by Google.

## What is it?

Pawn is a tool to extract the BIOS firmware from Intel-based workstations and
laptops.
The name is a play on an internal tool that is also named after a chess piece.

## How to Build

Dependencies:
  * Linux on x86_64 (uses `/dev/mem`). FreeBSD might also work.
  * GCC >= 7 or Clang >= 7
  * CMake >= 3.14
  * Ninja or GNU Make

To build:

```bash
mkdir -p build && cmake -S . -B build
cmake --build build/
```

The resulting binary can be found in `build/pawn/pawn`.

## Usage

The following command will extract the BIOS firmware and save the image to
`bios_image.bin`:

```bash
sudo build/pawn/pawn bios_image.bin
```

Note: When running a Linux kernel > 4.8.4, make sure that either
`CONFIG_IO_DEVMEM=n` is set or that you've booted with the `iomem=relaxed`
boot option.

After extraction, you can then use other tools like
[UEFITool](https://github.com/LongSoft/UEFITool) to process the firmware
image further.
