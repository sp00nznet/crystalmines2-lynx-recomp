# extract_rom.ps1 - pull the Chip's Challenge .lnx out of a romset zip into roms/.
#
# ROMs are never committed (see .gitignore); this just helps you stage your own
# dump where the build expects it.
#
# Usage: pwsh scripts/extract_rom.ps1 -Zip "Chip's Challenge (USA, Europe).zip"
param(
    [Parameter(Mandatory = $true)][string]$Zip,
    [string]$Dest = "roms"
)

if (-not (Test-Path $Zip)) { throw "zip not found: $Zip" }
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [System.IO.Compression.ZipFile]::OpenRead((Resolve-Path $Zip))
try {
    $entry = $archive.Entries | Where-Object { $_.Name -match '\.lnx$' } | Select-Object -First 1
    if (-not $entry) { throw "no .lnx inside $Zip" }
    $out = Join-Path $Dest $entry.Name
    [System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $out, $true)
    Write-Host "extracted -> $out"
} finally {
    $archive.Dispose()
}
