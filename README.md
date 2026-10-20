# D2Client 1.10

A rewrite of `D2Client.dll` from Diablo II: Lord of Destruction 1.10 in C++, function by function, written to behave
exactly as the original does -- its bugs included.

Every game function of the original DLL is rewritten here. The Visual C++ 6 runtime the original was linked with is
not: the rewrite uses the runtime of the compiler that builds it. The built DLL loads no part of the original
`D2Client.dll`; it calls the other game modules (D2Common, D2Win, D2Gfx, Fog, Storm, ...) through their exports, as
the original does.

## Reading the source

* `//1.10f: D2Client.0x6FXXXXXX` above a function or a global is its address in the original DLL (image base
  0x6FAA0000).
* `BUG:` marks a defect of the original that the rewrite reproduces on purpose, with a verdict: `LATENT` (the code is
  wrong but nothing in the game triggers it), `REACHABLE`, `UNREACHABLE`, `INERT`, `HARMLESS` or `NOT ESTABLISHED`.
  `BUGS.md` holds every defect found, each with the measurement that settled it and its verdict.
* `DIFFERENCE:` marks a place where the rewrite does not do exactly what the original does -- where a faithful copy
  would be undefined behaviour in C++, the rewrite takes the defined program and says what differs.
* `D2ExpRes <SiteName>` marks a line where Dav92's D2ExpRes resolution patch hooks the original, for whoever ports
  that patch onto this source.
* One `.cpp` per unit of the original. Where an assert of the original names its source file, the file is named after
  it; `Shared.cpp` holds the functions no single unit can claim.

## Generated headers

`D2Constants.h`, `D2Structs.h`, `D2Ptrs.h`, `D2Vars.h` and `D2Imports.h` are generated from the reverse-engineering
database the rewrite was made from, and are not edited by hand. `D2Ptrs.h` and `D2Vars.h` list every game function
and global of the original with its offset; every line is commented out, since this source implements them all.
`implib/named/` holds the import libraries (and their `.def` files) for the game modules, so every call links to the
export the original imports. `D2Client.def` gives the DLL the original's four exports at their ordinals.

## Building

Visual Studio 2022 or later, platform toolset v143, C++20, configuration `Release|Win32`:

    msbuild D2Client.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32

Floating-point code compiles for the x87 (`/arch:IA32`), as the original computes it.

## Using it

The DLL is built to stand in for `D2Client.dll` in a Diablo II 1.10 installation. Keep a copy of the original. A
copy of the game is needed; nothing of Blizzard's is in this repository.

## Credits

* [D2MOO](https://github.com/ThePhrozenKeep/D2MOO), the Phrozen Keep community's reconstruction of the 1.10 sources,
  for many enum, structure and function names.
* [D2Template](https://github.com/olivier-verville/D2Template) by Olivier Verville, which `DLLmain.cpp` and `DLLmain.h`
  come from (Apache License 2.0).
* Dav92, the author of D2ExpRes, the high-resolution patch for Diablo II 1.10; the `D2ExpRes:` comments in the source
  mark the places its detours land on.

## License

Apache License 2.0 -- see `LICENSE`. Diablo II is a trademark of Blizzard Entertainment; this project is not affiliated
with Blizzard.
