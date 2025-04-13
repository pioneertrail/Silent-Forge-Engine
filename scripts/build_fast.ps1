# Fast build script using CMake presets
param(
    [string]$Preset = "ninja-debug",
    [switch]$Clean = $false
)

# Ensure we're in the right directory
Set-Location (Split-Path -Parent $PSScriptRoot)

# Check if ninja is installed
$ninjaPath = $null
try {
    $ninjaPath = (Get-Command ninja -ErrorAction SilentlyContinue).Source
} catch {}

if (-not $ninjaPath) {
    Write-Host "Ninja build system not found. Using default generator instead."
    $useNinja = $false
} else {
    Write-Host "Using Ninja from: $ninjaPath"
    $useNinja = $true
}

# Check for ccache
$ccachePath = $null
try {
    $ccachePath = (Get-Command ccache -ErrorAction SilentlyContinue).Source
} catch {}

if ($ccachePath) {
    Write-Host "Using ccache from: $ccachePath"
    $env:CMAKE_CXX_COMPILER_LAUNCHER = "ccache"
}

# Clean build directories if requested
if ($Clean) {
    Write-Host "Cleaning build directories..."
    if (Test-Path "build") {
        Remove-Item -Path "build" -Recurse -Force
    }
}

# Create build directory
$buildPath = "build"
if (-not (Test-Path $buildPath)) {
    New-Item -ItemType Directory -Path $buildPath -Force | Out-Null
}
Set-Location $buildPath

# Configure and build the project
Write-Host "Configuring project..."
if ($useNinja) {
    cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
} else {
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
}

Write-Host "Building project..."
cmake --build . -j $env:NUMBER_OF_PROCESSORS

# Run tests
Write-Host "Running tests..."
ctest --output-on-failure

Set-Location ..
Write-Host "Build completed!" 