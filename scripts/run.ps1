Param(
  [switch]$Trace
)

# Usage: .\scripts\run.ps1 [-Trace]
# Run from project root

$traceFlag = ""
if ($Trace) { $traceFlag = "-DTRACE" }
$target = "ll_isort.exe"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$srcDir = Join-Path $scriptDir "..\src"
$incDir = Join-Path $scriptDir "..\include"

function Have($cmd) {
  $null -ne (Get-Command $cmd -ErrorAction SilentlyContinue)
}

if (Have "g++") {
  Write-Host "Building with g++ $traceFlag"
  & g++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"$incDir" $traceFlag "$srcDir\main.cpp" -o $target
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Host "Running $target"
  & .\$target
  exit $LASTEXITCODE
}

if (Have "clang++") {
  Write-Host "Building with clang++ $traceFlag"
  & clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"$incDir" $traceFlag "$srcDir\main.cpp" -o $target
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Host "Running $target"
  & .\$target
  exit $LASTEXITCODE
}

Write-Host "No g++/clang++ found. Install MSYS2/MinGW, LLVM, or use CMake with Visual Studio."
exit 1