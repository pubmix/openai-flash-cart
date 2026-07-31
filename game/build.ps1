$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $projectRoot
function Find-GbdkHome {
    if ($env:GBDK_HOME) { return $env:GBDK_HOME }
    $dir = Get-Item -LiteralPath $repoRoot
    while ($dir) {
        $candidate = Join-Path $dir.FullName 'work\tools\gbdk\gbdk'
        if (Test-Path -LiteralPath (Join-Path $candidate 'bin\lcc.exe') -PathType Leaf) { return $candidate }
        $candidate = Join-Path $dir.FullName 'gbdk'
        if (Test-Path -LiteralPath (Join-Path $candidate 'bin\lcc.exe') -PathType Leaf) { return $candidate }
        $dir = $dir.Parent
    }
    foreach ($candidate in @('C:\gbdk', 'C:\gbdk-2020', 'C:\tools\gbdk')) {
        if (Test-Path -LiteralPath (Join-Path $candidate 'bin\lcc.exe') -PathType Leaf) { return $candidate }
    }
    return (Join-Path $repoRoot 'work\tools\gbdk\gbdk')
}
$gbdkHome = Find-GbdkHome
$lcc = Join-Path $gbdkHome 'bin\lcc.exe'

if (-not (Test-Path -LiteralPath $lcc -PathType Leaf)) {
    throw "Could not find GBDK lcc.exe. Set GBDK_HOME to your GBDK-2020 folder. Tried: $lcc"
}

$outDir = Join-Path $repoRoot 'outputs'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$rom = Join-Path $outDir 'openai-flash-cart.gb'
$sources = @(
    (Join-Path $projectRoot 'src\main.c'),
    (Join-Path $projectRoot 'src\music.c')
)

& $lcc -Wm-yC -Wm-yn'OPENAIFLASH' -o $rom $sources
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Built $rom"
