#!/usr/bin/env python3
"""
This script fully fixes the CMakeLists.txt to properly support minimal builds.
"""
import os
import re
import shutil
import sys

def backup_file(file_path):
    """Create a backup of the file."""
    backup_path = file_path + '.bak'
    shutil.copy2(file_path, backup_path)
    print(f"Created backup at {backup_path}")
    return backup_path

def fix_cmakelists(file_path):
    """Fix the CMakeLists.txt file to properly handle SKIP_DEPENDENCIES flag."""
    
    # Read the original file
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Create a backup
    backup_file(file_path)
    
    # Fix 1: Wrap all find_package calls in proper conditional blocks
    dependency_fixes = [
        # Fix SDL2 dependencies
        (r'find_package\(SDL2 REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(SDL2 REQUIRED)\nelse()\n    add_library(SDL2::SDL2 INTERFACE IMPORTED)\nendif()'),
        
        # Fix GLEW dependencies
        (r'find_package\(GLEW REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(GLEW REQUIRED)\nelse()\n    add_library(GLEW::GLEW INTERFACE IMPORTED)\nendif()'),
        
        # Fix OpenGL dependencies
        (r'find_package\(OpenGL REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(OpenGL REQUIRED)\nelse()\n    add_library(OpenGL::GL INTERFACE IMPORTED)\nendif()'),
        
        # Fix GLM dependencies
        (r'find_package\(glm REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(glm REQUIRED)\nelse()\n    add_library(glm::glm INTERFACE IMPORTED)\nendif()'),
        
        # Fix Assimp dependencies
        (r'find_package\(assimp REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(assimp REQUIRED)\nelse()\n    add_library(assimp::assimp INTERFACE IMPORTED)\nendif()'),
        
        # Fix Bullet dependencies
        (r'find_package\(Bullet REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(Bullet REQUIRED)\nelse()\n    add_library(Bullet::Bullet INTERFACE IMPORTED)\n    add_library(Bullet::LinearMath INTERFACE IMPORTED)\n    add_library(Bullet::Collision INTERFACE IMPORTED)\n    add_library(Bullet::Dynamics INTERFACE IMPORTED)\nendif()'),
        
        # Fix OpenAL dependencies
        (r'find_package\(OpenAL REQUIRED\)', 
         'if(NOT SKIP_DEPENDENCIES)\n    find_package(OpenAL REQUIRED)\nelse()\n    add_library(OpenAL::OpenAL INTERFACE IMPORTED)\nendif()'),
    ]
    
    for pattern, replacement in dependency_fixes:
        content = re.sub(pattern, replacement, content)
    
    # Fix 2: Add SKIP_DEPENDENCIES option if it doesn't exist
    if 'option(SKIP_DEPENDENCIES' not in content:
        option_section = 'option(SKIP_DEPENDENCIES "Skip finding dependencies for faster development builds" OFF)\n'
        # Insert after the project declaration
        content = re.sub(r'(project\([^\)]+\))', r'\1\n\n' + option_section, content)
    
    # Fix 3: Add precompiled headers option
    if 'option(USE_PRECOMPILED_HEADERS' not in content:
        pch_option = 'option(USE_PRECOMPILED_HEADERS "Use precompiled headers for faster builds" OFF)\n'
        # Insert after SKIP_DEPENDENCIES option
        if 'SKIP_DEPENDENCIES' in content:
            content = re.sub(r'(option\(SKIP_DEPENDENCIES[^\n]+\n)', r'\1' + pch_option, content)
        else:
            # If SKIP_DEPENDENCIES was not added for some reason, add after project
            content = re.sub(r'(project\([^\)]+\))', r'\1\n\n' + pch_option, content)
    
    # Fix 4: Add precompiled header implementation
    if "target_precompile_headers" not in content:
        pch_implementation = '''
# Set up precompiled headers if enabled
if(USE_PRECOMPILED_HEADERS)
    message(STATUS "Using precompiled headers for faster builds")
    target_precompile_headers(SilentForge PRIVATE
        <vector>
        <string>
        <memory>
        <unordered_map>
        <cstdlib>
        <cstring>
        <iostream>
        <algorithm>
        <functional>
        <utility>
        <stdexcept>
        <chrono>
        <array>
        <map>
        <set>
        <thread>
        <mutex>
        <condition_variable>
    )
endif()
'''
        # Add before the end of the file
        content = content + "\n" + pch_implementation
    
    # Write the fixed content
    with open(file_path, 'w') as f:
        f.write(content)
    
    print(f"Successfully fixed {file_path}")
    return True

def main():
    """Main function to find and fix CMakeLists.txt files."""
    # Get the root directory
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    main_cmakelists = os.path.join(root_dir, 'CMakeLists.txt')
    
    if not os.path.exists(main_cmakelists):
        print(f"Error: Could not find {main_cmakelists}")
        return 1
    
    print(f"Fixing main CMakeLists.txt at {main_cmakelists}")
    if fix_cmakelists(main_cmakelists):
        print("All fixes applied successfully!")
        return 0
    else:
        print("Failed to apply all fixes!")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 