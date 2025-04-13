# Build script for SilentForge
param(
    [string]$BuildType = "Release",
    [string]$Generator = "Visual Studio 17 2022",
    [switch]$Clean = $false
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Function to run a command and check its exit code
function Run-Command {
    param($Command)
    Write-Host "Running: $Command"
    Invoke-Expression $Command
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE"
    }
}

# Get the project root directory
$projectRoot = $PSScriptRoot | Split-Path -Parent

# Clean build directory if requested
if ($Clean) {
    Write-Host "Cleaning build directory..."
    Remove-Item -Recurse -Force "$projectRoot/build" -ErrorAction SilentlyContinue
}

# Create build directory
if (-not (Test-Path "$projectRoot/build")) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path "$projectRoot/build" | Out-Null
}

# Configure CMake
Write-Host "Configuring CMake..."
$cmakeArgs = @(
    "-G `"$Generator`"",
    "-A x64",
    "-DCMAKE_TOOLCHAIN_FILE=$projectRoot/vcpkg/scripts/buildsystems/vcpkg.cmake",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DBUILD_TESTING=ON"
)
Run-Command "cd $projectRoot/build; cmake .. $cmakeArgs"

# Build the project
Write-Host "Building project..."
if ($Generator -match "Visual Studio") {
    Run-Command "cd $projectRoot/build; cmake --build . --config $BuildType --parallel"
} else {
    Run-Command "cd $projectRoot/build; cmake --build . --config $BuildType -j"
}

Write-Host "Build completed successfully!" 