# Crystal Mines II — notes

Second game on [`lynxrecomp`](https://github.com/sp00nznet/lynxrecomp); a
generalization test. General Lynx hardware, the recompiler internals, and the
boot/decrypt problem are documented in the toolkit
([`lynxrecomp/docs/`](../lynxrecomp/docs/)).

## ROM

- File: `Crystal Mines II (USA, Europe).lnx`
- Container: BLL `.lnx`, 64-byte header + 131072-byte cart (128 KiB)
- Header: cart name `CRYSTMII`, manufacturer `Atari`, bank0 page size 512
- sha1: `c3b7e7fe892068bf4b58aec85181f6b00693b47d`

## Boot / recompile (toolkit, unchanged)

- Encrypted loader: **150 bytes / 3 blocks** (Chip's Challenge was 250 / 5);
  the generic RSA decrypt handled it directly. Loader entry `$0200`.
- `lynxexec` reaches **game entry `$5259`** via the loader's `JMP ($004E)`.
- Post-init `lynxrun --snapshot` (60 frames): IRQ vector `$FFFE = $60EB`.
- `recompbin` from the post-init image, seeds:
  `$5259 $5757 $56C6 $56CF $042B $051A` → **0 dispatch gaps**, the recompiled C
  runs the main loop and renders the title (`screenshots/title.png`).

## Open items

- Same as the toolkit's remaining polish: full-fidelity cold boot, interactive
  input, blitter scaling/collision. Gameplay (digging through levels) past the
  title is the next milestone for this title.
