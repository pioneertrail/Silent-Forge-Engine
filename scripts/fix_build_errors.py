#!/usr/bin/env python3
import os
import re
import sys
import glob
from pathlib import Path

def main():
    print("[FixBuildErrors] Starting build error fixes...")
    
    # Get the project root directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    
    # Fix CMakeLists.txt issues
    # Comment out for now to prevent breaking the file further
    # fix_cmakelists(project_root)
    
    # Fix OpenGL issues in source files
    fix_opengl_includes(project_root)
    
    # Fix other common errors
    fix_common_errors(project_root)
    
    print("[FixBuildErrors] Fixes completed successfully!")
    return 0

def fix_cmakelists(root_dir):
    """Fix issues in CMakeLists.txt files"""
    print("[FixBuildErrors] Fixing CMakeLists.txt files...")
    
    main_cmake = os.path.join(root_dir, "CMakeLists.txt")
    if not os.path.exists(main_cmake):
        print(f"[FixBuildErrors] Error: {main_cmake} not found")
        return
    
    # Skip this function to avoid breaking the CMakeLists.txt file
    print("[FixBuildErrors] Skipping CMakeLists.txt modifications")
    return

def fix_opengl_includes(root_dir):
    """Fix OpenGL include issues in source files"""
    print("[FixBuildErrors] Fixing OpenGL includes...")
    
    # Find all C++ source files
    cpp_files = []
    for ext in ['.cpp', '.cc', '.cxx', '.h', '.hpp']:
        cpp_files.extend(glob.glob(f"{root_dir}/src/**/*{ext}", recursive=True))
    
    opengl_fixes = 0
    for file_path in cpp_files:
        with open(file_path, 'r', errors='ignore') as f:
            content = f.read()
        
        # Replace direct GL includes with glew
        if "#include <GL/gl.h>" in content and "#include <GL/glew.h>" not in content:
            content = content.replace("#include <GL/gl.h>", "#include <GL/glew.h>")
            with open(file_path, 'w') as f:
                f.write(content)
            opengl_fixes += 1
    
    if opengl_fixes > 0:
        print(f"[FixBuildErrors] Fixed OpenGL includes in {opengl_fixes} files")

def fix_common_errors(root_dir):
    """Fix other common build errors"""
    print("[FixBuildErrors] Checking for other common errors...")
    
    # Create a dummy vcpkg_config.cmake if it doesn't exist
    vcpkg_config = os.path.join(root_dir, "vcpkg_config.cmake")
    if not os.path.exists(vcpkg_config):
        with open(vcpkg_config, 'w') as f:
            f.write("""# Auto-generated vcpkg config
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
set(VCPKG_TARGET_TRIPLET x64-windows CACHE STRING "")
""")
        print("[FixBuildErrors] Created vcpkg_config.cmake")
    
    # Ensure the src directory exists
    src_dir = os.path.join(root_dir, "src")
    if not os.path.exists(src_dir):
        os.makedirs(src_dir)
        print("[FixBuildErrors] Created missing src directory")

if __name__ == "__main__":
    sys.exit(main())