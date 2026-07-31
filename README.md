# OpenAI Flash Cart

A minimal Game Boy ROM intended for a flash cart.

Boot sequence:

1. ModRetro logo
2. OpenAI logo
3. `OpenAI Dev Day 2026` / `Powered by Codex`
4. `Created by` / `Jeremy Fortner`
5. Blank black screen

The ROM also plays an original watery ambient PSG track during the splash sequence.

The built ROM is included at:

```text
outputs/openai-flash-cart.gb
```

## Requirements

- GBDK-2020
- mGBA for emulator testing
- PowerShell for the helper scripts

The build script looks for GBDK in this order:

1. `GBDK_HOME`
2. `work\tools\gbdk\gbdk` under this repo or a parent folder
3. `gbdk` under this repo or a parent folder
4. `C:\gbdk`
5. `C:\gbdk-2020`
6. `C:\tools\gbdk`

If GBDK is elsewhere:

```powershell
$env:GBDK_HOME = "C:\path\to\gbdk"
```

## Build

```powershell
.\game\build.ps1
```

## Run In mGBA

```powershell
.\game\run.ps1
```

If mGBA is somewhere other than the default Windows install path:

```powershell
$env:MGBA_EXE = "C:\path\to\mGBA.exe"
```

## Assets

The logo screens are stored as Game Boy background tile data in:

```text
game/src/assets/logo_assets.h
```

The previews in `assets/` show the final 160x144 Game Boy screens.

To regenerate the assets from source logo PNGs:

```powershell
.\tools\make_logo_assets.ps1 -OpenAiLogo C:\path\to\openai.png -ModRetroLogo C:\path\to\modretro.png
```

The third screen is rendered from text. If Kallisto Bold is available as a font file, pass it explicitly:

```powershell
.\tools\make_logo_assets.ps1 -OpenAiLogo C:\path\to\openai.png -ModRetroLogo C:\path\to\modretro.png -FontPath C:\path\to\Kallisto-Bold.otf
```

The checked-in generated assets currently use `Bahnschrift SemiBold` as a local fallback when Kallisto Bold is not installed. Text screens are rendered with supersampling before conversion to Game Boy tiles for cleaner edges.

## Cartridge Details

- ROM title: `OPENAIFLASH`
- CGB-aware `.gb`
- Original synth music using Game Boy audio channels
- No save RAM / battery required
