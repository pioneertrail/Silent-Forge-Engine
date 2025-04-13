# Preload all required dependencies
$dependencies = @(
    "glm:x64-windows",
    "sdl2:x64-windows",
    "opengl:x64-windows",
    "gtest:x64-windows",
    "catch2:x64-windows",
    "nlohmann-json:x64-windows",
    "glfw3:x64-windows"
)

Write-Host "Preloading dependencies..."
foreach ($dep in $dependencies) {
    Write-Host "Installing $dep..."
    ./vcpkg/vcpkg.exe install $dep
}

Write-Host "All dependencies preloaded successfully!" 