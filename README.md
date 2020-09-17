# `nesdump`

Dump info/data from iNES ROMs.

## Building

Get an unix with a C++17 compiler like g++ or clang++ and type `make`.

## Usage

    ./nesdump rom.nes <item>

valid items:

- `info`: prints misc info about the ROM to stdout.
- `prg`: dumps PRG-ROM to stdout.
- `prg:<num>`: dumps one PRG-ROM bank to stdout. Fails if the identified mapper isn't known to support PRG-ROM bank switching.
- `chr`: dumps CHR-ROM to stdout.
- `chr:<num>`: dumps one CHR-ROM bank to stdout. Fails if the identified mapper isn't known to support CHR-ROM bank switching.

The size of banks depends on the identified mapper. As a rule of thumb, if the mapper supports multiple different bank sizes, the smallest size is considered.

## Supported mappers

See [nes.cc](./nes.cc).
