@echo off
setlocal enabledelayedexpansion

echo === Minimal C++ Project Benchmark ===

set MINI_DIR=mini_benchmark
mkdir %MINI_DIR% 2>NUL

:: Create a minimal C++ project
echo Creating minimal C++ project...
mkdir %MINI_DIR%\src 2>NUL

:: Create main.cpp
echo #include ^<iostream^> > %MINI_DIR%\src\main.cpp
echo #include ^<vector^> >> %MINI_DIR%\src\main.cpp
echo #include ^<string^> >> %MINI_DIR%\src\main.cpp
echo #include ^<algorithm^> >> %MINI_DIR%\src\main.cpp
echo #include ^<chrono^> >> %MINI_DIR%\src\main.cpp
echo. >> %MINI_DIR%\src\main.cpp
echo int main() { >> %MINI_DIR%\src\main.cpp
echo     std::vector^<std::string^> items; >> %MINI_DIR%\src\main.cpp
echo     for (int i = 0; i ^< 100000; ++i) { >> %MINI_DIR%\src\main.cpp
echo         items.push_back("Item " + std::to_string(i)); >> %MINI_DIR%\src\main.cpp
echo     } >> %MINI_DIR%\src\main.cpp
echo     std::cout ^<^< "Created " ^<^< items.size() ^<^< " items" ^<^< std::endl; >> %MINI_DIR%\src\main.cpp
echo     return 0; >> %MINI_DIR%\src\main.cpp
echo } >> %MINI_DIR%\src\main.cpp

:: Create a standard CMakeLists.txt
echo cmake_minimum_required(VERSION 3.16) > %MINI_DIR%\CMakeLists.txt
echo project(MiniBenchmark LANGUAGES CXX) >> %MINI_DIR%\CMakeLists.txt
echo. >> %MINI_DIR%\CMakeLists.txt
echo option(USE_PRECOMPILED_HEADERS "Use precompiled headers" OFF) >> %MINI_DIR%\CMakeLists.txt
echo. >> %MINI_DIR%\CMakeLists.txt
echo set(CMAKE_CXX_STANDARD 17) >> %MINI_DIR%\CMakeLists.txt
echo set(CMAKE_CXX_STANDARD_REQUIRED ON) >> %MINI_DIR%\CMakeLists.txt
echo. >> %MINI_DIR%\CMakeLists.txt
echo add_executable(mini_app src/main.cpp) >> %MINI_DIR%\CMakeLists.txt
echo. >> %MINI_DIR%\CMakeLists.txt
echo if(USE_PRECOMPILED_HEADERS) >> %MINI_DIR%\CMakeLists.txt
echo   target_precompile_headers(mini_app PRIVATE >> %MINI_DIR%\CMakeLists.txt
echo     ^<vector^> >> %MINI_DIR%\CMakeLists.txt
echo     ^<string^> >> %MINI_DIR%\CMakeLists.txt
echo     ^<iostream^> >> %MINI_DIR%\CMakeLists.txt
echo     ^<algorithm^> >> %MINI_DIR%\CMakeLists.txt
echo   ) >> %MINI_DIR%\CMakeLists.txt
echo endif() >> %MINI_DIR%\CMakeLists.txt

:: Run benchmark
cd %MINI_DIR%

:: Benchmark 1: Regular build with MinGW
if exist build_mingw rmdir /s /q build_mingw
mkdir build_mingw
cd build_mingw

echo.
echo === Benchmark 1: Regular build with MinGW ===
set start_time=%time%
cmake .. -G "MinGW Makefiles"
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" mingw_time
echo Regular build took %mingw_time% seconds
cd ..

:: Benchmark 2: Ninja
if exist build_ninja rmdir /s /q build_ninja
mkdir build_ninja
cd build_ninja

echo.
echo === Benchmark 2: Ninja build system ===
set start_time=%time%
cmake .. -G "Ninja"
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" ninja_time
echo Ninja build took %ninja_time% seconds
cd ..

:: Benchmark 3: ccache
if exist build_ccache rmdir /s /q build_ccache
mkdir build_ccache
cd build_ccache

echo.
echo === Benchmark 3: ccache ===
set start_time=%time%
cmake .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" ccache_time
echo ccache build took %ccache_time% seconds
cd ..

:: Benchmark 4: PCH
if exist build_pch rmdir /s /q build_pch
mkdir build_pch
cd build_pch

echo.
echo === Benchmark 4: Precompiled Headers ===
set start_time=%time%
cmake .. -G "MinGW Makefiles" -DUSE_PRECOMPILED_HEADERS=ON
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" pch_time
echo PCH build took %pch_time% seconds
cd ..

:: Benchmark 5: Full Optimization
if exist build_full rmdir /s /q build_full
mkdir build_full
cd build_full

echo.
echo === Benchmark 5: All optimizations combined ===
set start_time=%time%
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DUSE_PRECOMPILED_HEADERS=ON
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" full_time
echo Full optimization build took %full_time% seconds
cd ..

:: Second run of the fully optimized build (warm cache)
echo.
echo === Benchmark 6: Warm cache build ===
cd build_full
set start_time=%time%
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" warm_time
echo Warm cache build took %warm_time% seconds
cd ..

echo.
echo === Benchmark Summary ===
echo MinGW:           %mingw_time% seconds
echo Ninja:           %ninja_time% seconds
echo ccache:          %ccache_time% seconds
echo PCH:             %pch_time% seconds
echo All combined:    %full_time% seconds
echo Warm cache:      %warm_time% seconds

:: Calculate speedups
set /a ninja_speedup=(%mingw_time% * 100) / %ninja_time%
set /a ccache_speedup=(%mingw_time% * 100) / %ccache_time%
set /a pch_speedup=(%mingw_time% * 100) / %pch_time%
set /a full_speedup=(%mingw_time% * 100) / %full_time%
set /a warm_speedup=(%mingw_time% * 100) / %warm_time%

echo.
echo === Speedup vs MinGW ===
echo Ninja:           %ninja_speedup%%%
echo ccache:          %ccache_speedup%%%
echo PCH:             %pch_speedup%%%
echo All combined:    %full_speedup%%%
echo Warm cache:      %warm_speedup%%%

cd ..
exit /b 0

:calc_time_diff
setlocal
set start=%~1
set end=%~2

:: Convert start time to seconds
set /a start_h=1%start:~0,2%-100
set /a start_m=1%start:~3,2%-100
set /a start_s=1%start:~6,2%-100
set /a start_total=%start_h%*3600 + %start_m%*60 + %start_s%

:: Convert end time to seconds
set /a end_h=1%end:~0,2%-100
set /a end_m=1%end:~3,2%-100
set /a end_s=1%end:~6,2%-100
set /a end_total=%end_h%*3600 + %end_m%*60 + %end_s%

:: Calculate difference
set /a diff=%end_total% - %start_total%
if %diff% lss 0 set /a diff+=86400

endlocal & set %~3=%diff%
exit /b 0 