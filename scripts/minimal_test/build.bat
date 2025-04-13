@echo off
echo === Minimal Test Build ===

:: Set environment path
set PATH=%PATH%;C:\Strawberry\c\bin

:: Create build directory
if exist build rmdir /s /q build
mkdir build
cd build

:: First build (cold cache)
echo === First Build (Cold Cache) ===
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
echo === Building (first time) ===
set start=%time%
cmake --build .
set end=%time%
echo First build completed in %start% to %end%

:: Second build (warm cache)
echo === Second Build (Warm Cache) ===
set start=%time%
cmake --build .
set end=%time%
echo Second build completed in %start% to %end%

:: Run the program
echo === Running Program ===
.\minimal_test.exe

cd .. 