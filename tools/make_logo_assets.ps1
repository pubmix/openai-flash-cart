param(
    [Parameter(Mandatory = $true)][string]$OpenAiLogo,
    [Parameter(Mandatory = $true)][string]$ModRetroLogo,
    [string]$OutHeader = ".\game\src\assets\logo_assets.h",
    [string]$PreviewDir = ".\assets"
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

function Get-VisibleBox([System.Drawing.Bitmap]$Bitmap, [switch]$UseAlphaOnly) {
    $minX = $Bitmap.Width
    $minY = $Bitmap.Height
    $maxX = -1
    $maxY = -1

    for ($y = 0; $y -lt $Bitmap.Height; $y++) {
        for ($x = 0; $x -lt $Bitmap.Width; $x++) {
            $c = $Bitmap.GetPixel($x, $y)
            $lum = [int](0.2126 * $c.R + 0.7152 * $c.G + 0.0722 * $c.B)
            $visible = if ($UseAlphaOnly) { $c.A -gt 8 } else { ($c.A -gt 8) -and (($lum -gt 8) -or ($c.A -gt 180)) }
            if ($visible) {
                if ($x -lt $minX) { $minX = $x }
                if ($y -lt $minY) { $minY = $y }
                if ($x -gt $maxX) { $maxX = $x }
                if ($y -gt $maxY) { $maxY = $y }
            }
        }
    }

    if ($maxX -lt 0) {
        return [System.Drawing.Rectangle]::new(0, 0, $Bitmap.Width, $Bitmap.Height)
    }
    return [System.Drawing.Rectangle]::new($minX, $minY, $maxX - $minX + 1, $maxY - $minY + 1)
}

function New-ScreenPixels([string]$Path, [string]$Kind) {
    $src = [System.Drawing.Bitmap]::FromFile($Path)
    try {
        $useAlphaOnly = $Kind -eq "openai"
        $box = Get-VisibleBox $src -UseAlphaOnly:$useAlphaOnly
        $screenW = 160
        $screenH = 144
        if ($Kind -eq "openai") {
            $maxW = 104
            $maxH = 104
        } else {
            $maxW = 148
            $maxH = 34
        }

        $scale = [Math]::Min($maxW / $box.Width, $maxH / $box.Height)
        $drawW = [Math]::Max(1, [int][Math]::Round($box.Width * $scale))
        $drawH = [Math]::Max(1, [int][Math]::Round($box.Height * $scale))
        $drawX = [int][Math]::Floor(($screenW - $drawW) / 2)
        $drawY = [int][Math]::Floor(($screenH - $drawH) / 2)
        $pixels = New-Object 'int[,]' $screenW, $screenH
        for ($y = 0; $y -lt $screenH; $y++) {
            for ($x = 0; $x -lt $screenW; $x++) {
                $intensity = 0
                if (($x -ge $drawX) -and ($x -lt ($drawX + $drawW)) -and ($y -ge $drawY) -and ($y -lt ($drawY + $drawH))) {
                    $srcX = $box.X + [int][Math]::Floor((($x - $drawX + 0.5) / $drawW) * $box.Width)
                    $srcY = $box.Y + [int][Math]::Floor((($y - $drawY + 0.5) / $drawH) * $box.Height)
                    if ($srcX -ge $box.Right) { $srcX = $box.Right - 1 }
                    if ($srcY -ge $box.Bottom) { $srcY = $box.Bottom - 1 }
                    $c = $src.GetPixel($srcX, $srcY)
                    if ($Kind -eq "openai") {
                        $intensity = $c.A
                    } else {
                        $lum = [int](0.2126 * $c.R + 0.7152 * $c.G + 0.0722 * $c.B)
                        $intensity = [int](($c.A / 255.0) * $lum)
                    }
                } else {
                    $intensity = 0
                }

                if ($intensity -lt 24) { $idx = 3 }
                elseif ($intensity -lt 96) { $idx = 2 }
                elseif ($intensity -lt 178) { $idx = 1 }
                else { $idx = 0 }
                $pixels[$x, $y] = $idx
            }
        }

        $preview = [System.Drawing.Bitmap]::new($screenW, $screenH, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
        $colors = @(
            [System.Drawing.Color]::FromArgb(255, 248, 248, 232),
            [System.Drawing.Color]::FromArgb(255, 168, 188, 136),
            [System.Drawing.Color]::FromArgb(255, 76, 92, 80),
            [System.Drawing.Color]::FromArgb(255, 8, 12, 14)
        )
        for ($y = 0; $y -lt $screenH; $y++) {
            for ($x = 0; $x -lt $screenW; $x++) {
                $colorIndex = $pixels[$x, $y]
                $preview.SetPixel($x, $y, $colors[$colorIndex])
            }
        }
        New-Item -ItemType Directory -Force -Path $PreviewDir | Out-Null
        $preview.Save((Join-Path (Resolve-Path -LiteralPath $PreviewDir) "$Kind-screen-preview.png"), [System.Drawing.Imaging.ImageFormat]::Png)
        $preview.Dispose()
        return ,$pixels
    }
    finally {
        $src.Dispose()
    }
}

function Convert-ToTiles([int[,]]$Pixels, [string]$Name) {
    $tileLookup = @{}
    $tiles = New-Object System.Collections.Generic.List[byte[]]
    $map = New-Object System.Collections.Generic.List[int]

    for ($ty = 0; $ty -lt 18; $ty++) {
        for ($tx = 0; $tx -lt 20; $tx++) {
            $tile = New-Object byte[] 16
            for ($row = 0; $row -lt 8; $row++) {
                $lo = 0
                $hi = 0
                for ($col = 0; $col -lt 8; $col++) {
                    $idx = $Pixels[($tx * 8 + $col), ($ty * 8 + $row)]
                    $bit = 7 - $col
                    if (($idx -band 1) -ne 0) { $lo = $lo -bor (1 -shl $bit) }
                    if (($idx -band 2) -ne 0) { $hi = $hi -bor (1 -shl $bit) }
                }
                $tile[$row * 2] = [byte]$lo
                $tile[$row * 2 + 1] = [byte]$hi
            }

            $key = [Convert]::ToBase64String($tile)
            if (-not $tileLookup.ContainsKey($key)) {
                $tileLookup[$key] = $tiles.Count
                $tiles.Add($tile)
            }
            $map.Add($tileLookup[$key])
        }
    }

    if ($tiles.Count -gt 255) {
        throw "$Name generated $($tiles.Count) unique tiles; reduce detail or logo size."
    }

    return [PSCustomObject]@{ Name = $Name; Tiles = $tiles; Map = $map }
}

function Format-CArray([string]$Type, [string]$Name, [System.Collections.IEnumerable]$Values, [int]$PerLine) {
    $items = @($Values | ForEach-Object {
        if ($_ -is [byte]) { "0x{0:X2}" -f $_ } else { [string]$_ }
    })
    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add("static const $Type $Name[] = {")
    for ($i = 0; $i -lt $items.Count; $i += $PerLine) {
        $end = [Math]::Min($i + $PerLine - 1, $items.Count - 1)
        $lines.Add("    " + (($items[$i..$end]) -join ", ") + ",")
    }
    $lines.Add("};")
    return ($lines -join "`r`n")
}

function Write-AssetHeader($OpenAi, $ModRetro, [string]$Path) {
    $openTileBytes = @()
    foreach ($tile in $OpenAi.Tiles) { $openTileBytes += $tile }
    $modTileBytes = @()
    foreach ($tile in $ModRetro.Tiles) { $modTileBytes += $tile }

    $content = @(
        "#ifndef LOGO_ASSETS_H",
        "#define LOGO_ASSETS_H",
        "",
        "#include <stdint.h>",
        "",
        "#define LOGO_MAP_WIDTH 20",
        "#define LOGO_MAP_HEIGHT 18",
        "#define OPENAI_TILE_COUNT $($OpenAi.Tiles.Count)",
        "#define MODRETRO_TILE_COUNT $($ModRetro.Tiles.Count)",
        "",
        (Format-CArray "uint8_t" "openai_tiles" $openTileBytes 16),
        "",
        (Format-CArray "uint8_t" "openai_map" $OpenAi.Map 20),
        "",
        (Format-CArray "uint8_t" "modretro_tiles" $modTileBytes 16),
        "",
        (Format-CArray "uint8_t" "modretro_map" $ModRetro.Map 20),
        "",
        "#endif"
    ) -join "`r`n"

    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $Path) | Out-Null
    [System.IO.File]::WriteAllText((Resolve-Path -LiteralPath (Split-Path -Parent $Path)).Path + "\" + (Split-Path -Leaf $Path), $content, [System.Text.Encoding]::ASCII)
}

$openPixels = New-ScreenPixels -Path $OpenAiLogo -Kind "openai"
$modPixels = New-ScreenPixels -Path $ModRetroLogo -Kind "modretro"
$openAsset = Convert-ToTiles -Pixels $openPixels -Name "openai"
$modAsset = Convert-ToTiles -Pixels $modPixels -Name "modretro"
Write-AssetHeader -OpenAi $openAsset -ModRetro $modAsset -Path $OutHeader

Write-Host "Wrote $OutHeader"
Write-Host "OpenAI tiles: $($openAsset.Tiles.Count)"
Write-Host "ModRetro tiles: $($modAsset.Tiles.Count)"
Write-Host "Previews: $PreviewDir"
