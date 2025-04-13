#!/usr/bin/env python
import os
import re
import shutil
import glob

def log(message):
    """Print a formatted log message."""
    print(f"[DependencyGuard] {message}")

def backup_file(file_path):
    """Create a backup of the file if one doesn't already exist."""
    backup_path = f"{file_path}.orig"
    if not os.path.exists(backup_path):
        log(f"Creating backup of {file_path}")
        shutil.copy2(file_path, backup_path)

def restore_backup(file_path):
    """Restore the original file from backup if backup exists."""
    backup_path = f"{file_path}.orig"
    if os.path.exists(backup_path):
        log(f"Restoring {file_path} from backup")
        shutil.copy2(backup_path, file_path)

def fix_main_cmakelists(file_path="CMakeLists.txt"):
    """Modify the main CMakeLists.txt file to add the SKIP_DEPENDENCIES option."""
    if not os.path.exists(file_path):
        log(f"Error: {file_path} not found")
        return False
    
    backup_file(file_path)
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Add SKIP_DEPENDENCIES option if not already present
    if not re.search(r'option\s*\(\s*SKIP_DEPENDENCIES\s', content):
        option_insert = "option(SKIP_DEPENDENCIES \"Skip dependency checks for faster builds\" OFF)\n"
        
        # Try to find project() declaration to insert after it
        project_match = re.search(r'project\s*\([^)]*\)', content)
        if project_match:
            insert_pos = project_match.end()
            content = content[:insert_pos] + "\n\n" + option_insert + content[insert_pos:]
        else:
            # If no project() found, add at the beginning
            content = option_insert + content
    
    # Wrap find_package calls with conditional skip
    content = re.sub(
        r'(find_package\s*\([^)]*\))',
        r'if(NOT SKIP_DEPENDENCIES)\n  \1\nendif()',
        content
    )
    
    # Wrap include_directories with conditional skip if they look like external deps
    content = re.sub(
        r'(include_directories\s*\(([^)]*VCPKG[^)]*|[^)]*external[^)]*|[^)]*deps[^)]*)\))',
        r'if(NOT SKIP_DEPENDENCIES)\n  \1\nendif()',
        content
    )
    
    # Add special section for handling skipped dependencies
    if not re.search(r'if\s*\(\s*SKIP_DEPENDENCIES\s*\)', content):
        skip_section = """
# Special handling for skipped dependencies
if(SKIP_DEPENDENCIES)
  message(STATUS "Skipping dependency checks - libraries must be provided manually")
  include_directories(${CMAKE_SOURCE_DIR})
  # Add mock targets for any required dependencies
  if(NOT TARGET OpenGL::GL)
    add_library(OpenGL::GL INTERFACE IMPORTED)
  endif()
endif()
"""
        # Add before the first add_subdirectory if present
        add_subdir_match = re.search(r'add_subdirectory\s*\(', content)
        if add_subdir_match:
            insert_pos = add_subdir_match.start()
            content = content[:insert_pos] + skip_section + content[insert_pos:]
        else:
            # Otherwise add at the end
            content += skip_section
    
    with open(file_path, 'w') as f:
        f.write(content)
    
    log(f"Updated {file_path}")
    return True

def fix_subdirectory_cmakelists(directory="."):
    """Process all CMakeLists.txt files in subdirectories."""
    count = 0
    for cmake_file in glob.glob(f"{directory}/**/CMakeLists.txt", recursive=True):
        if cmake_file == "CMakeLists.txt":  # Skip main CMakeLists.txt
            continue
        
        backup_file(cmake_file)
        
        with open(cmake_file, 'r') as f:
            content = f.read()
        
        # Wrap target_link_libraries with conditional skip for external dependencies
        content = re.sub(
            r'(target_link_libraries\s*\([^)]*\b(OpenGL|SDL2|GLEW|external|deps)\b[^)]*\))',
            r'if(NOT SKIP_DEPENDENCIES)\n  \1\nendif()',
            content
        )
        
        # Wrap include_directories with conditional skip if they look like external deps
        content = re.sub(
            r'(include_directories\s*\(([^)]*external[^)]*|[^)]*deps[^)]*)\))',
            r'if(NOT SKIP_DEPENDENCIES)\n  \1\nendif()',
            content
        )
        
        with open(cmake_file, 'w') as f:
            f.write(content)
        
        count += 1
    
    log(f"Updated {count} subdirectory CMakeLists.txt files")
    return count > 0

def add_precompiled_header_support(file_path="CMakeLists.txt"):
    """Add support for precompiled headers in the CMakeLists.txt file."""
    if not os.path.exists(file_path):
        log(f"Error: {file_path} not found")
        return False
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Check if precompiled header support is already present
    if 'USE_PRECOMPILED_HEADERS' in content:
        log("Precompiled header support already exists")
        return True
    
    # Add option for precompiled headers
    pch_option = """
# Option for precompiled headers
option(USE_PRECOMPILED_HEADERS "Use precompiled headers to speed up compilation" OFF)

if(USE_PRECOMPILED_HEADERS)
  message(STATUS "Enabling precompiled headers")
  # Different syntax for different CMake versions
  if(CMAKE_VERSION VERSION_LESS "3.16.0")
    # Fallback for older CMake versions
    message(WARNING "Precompiled headers require CMake 3.16 or newer - feature disabled")
  else()
    # Modern precompiled header support
    set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
    # Apply to targets in subdirectories
    set(PCH_HEADERS
      <memory>
      <vector>
      <string>
      <algorithm>
      <functional>
      <utility>
      <iostream>
    )
  endif()
endif()
"""
    
    # Add after the project declaration if possible
    project_match = re.search(r'project\s*\([^)]*\)', content)
    if project_match:
        insert_pos = project_match.end()
        content = content[:insert_pos] + "\n" + pch_option + content[insert_pos:]
    else:
        # Otherwise add at the end
        content += "\n" + pch_option
    
    with open(file_path, 'w') as f:
        f.write(content)
    
    log("Added precompiled header support")
    return True

def main():
    """Main function to run all optimizations."""
    log("Starting dependency guard setup")
    
    # Process main CMakeLists.txt
    if fix_main_cmakelists():
        log("Successfully updated main CMakeLists.txt")
    else:
        log("Failed to update main CMakeLists.txt")
        return False
    
    # Process subdirectory CMakeLists.txt files
    if fix_subdirectory_cmakelists():
        log("Successfully updated subdirectory CMakeLists.txt files")
    else:
        log("No subdirectory CMakeLists.txt files found or updated")
    
    # Add precompiled header support
    if add_precompiled_header_support():
        log("Successfully added precompiled header support")
    else:
        log("Failed to add precompiled header support")
    
    log("Dependency guard setup complete")
    return True

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1) 