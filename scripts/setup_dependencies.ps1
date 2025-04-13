# Silent Forge Engine Dependency Setup Script
# This script verifies and installs required dependencies for the Silent Forge Engine.

# Configuration
$DEPENDENCIES = @{
    "cmake" = @{
        Command = "cmake --version"
        Install = "choco install cmake -y"
        Version = "3.22.0"
        Path = "C:\Program Files\CMake\bin"
        VersionPattern = "cmake version (\d+\.\d+\.\d+)"
    }
    "ninja" = @{
        Command = "ninja --version"
        Install = "choco install ninja -y"
        Version = "1.11.0"
        Path = "C:\ProgramData\chocolatey\bin"
        VersionPattern = "(\d+\.\d+\.\d+)"
    }
    "clang-format" = @{
        Command = "clang-format --version"
        Install = "choco install llvm -y"
        Version = "15.0.0"
        Path = "C:\Program Files\LLVM\bin"
        VersionPattern = "clang-format version (\d+\.\d+\.\d+)"
    }
    "clang-tidy" = @{
        Command = "clang-tidy --version"
        Install = "choco install llvm -y"
        Version = "15.0.0"
        Path = "C:\Program Files\LLVM\bin"
        VersionPattern = "LLVM version (\d+\.\d+\.\d+)"
    }
    "lcov" = @{
        Command = "lcov --version"
        Install = "choco install lcov -y"
        Version = "1.16.0"
        Path = "C:\ProgramData\chocolatey\bin"
        VersionPattern = "LCOV version (\d+\.\d+\.\d+)"
    }
}

# Function to check if Chocolatey is installed
function Test-Chocolatey {
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
        Write-Host "Installing Chocolatey package manager..."
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
        if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
            Write-Host "[FAIL] Failed to install Chocolatey"
            exit 1
        }
    }
    Write-Host "[PASS] Chocolatey is installed"
}

# Function to add to PATH
function Add-ToPath {
    param([string]$Path)
    
    if (-not ($env:Path -split ';' -contains $Path)) {
        $env:Path = "$Path;$env:Path"
        [Environment]::SetEnvironmentVariable("Path", $env:Path, [EnvironmentVariableTarget]::User)
        Write-Host "Added $Path to PATH"
    }
}

# Function to compare versions
function Compare-Versions {
    param(
        [string]$Version1,
        [string]$Version2
    )
    
    $v1 = [version]$Version1
    $v2 = [version]$Version2
    
    if ($v1 -gt $v2) { return 1 }
    if ($v1 -lt $v2) { return -1 }
    return 0
}

# Function to get installed version
function Get-InstalledVersion {
    param(
        [string]$Command,
        [string]$Pattern
    )
    
    try {
        $output = Invoke-Expression $Command 2>&1
        if ($output -match $Pattern) {
            return $matches[1]
        }
    } catch {
        return $null
    }
    return $null
}

# Function to install LLVM
function Install-LLVM {
    Write-Host "Installing LLVM..."
    $installerPath = Join-Path $env:TEMP "LLVM-15.0.0-win64.exe"
    $installerUrl = "https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.0/LLVM-15.0.0-win64.exe"
    
    try {
        # Check for existing LLVM installation
        $existingVersion = Get-InstalledVersion "clang-format --version" $DEPENDENCIES["clang-format"].VersionPattern
        if ($existingVersion) {
            $comparison = Compare-Versions $existingVersion $DEPENDENCIES["clang-format"].Version
            if ($comparison -ge 0) {
                Write-Host "[INFO] LLVM version $existingVersion is already installed and meets requirements"
                Add-ToPath "C:\Program Files\LLVM\bin"
                return $true
            } else {
                Write-Host "[INFO] Upgrading LLVM from version $existingVersion to $($DEPENDENCIES["clang-format"].Version)"
            }
        }
        
        # Download installer
        Write-Host "Downloading LLVM installer..."
        Invoke-WebRequest -Uri $installerUrl -OutFile $installerPath
        
        # Run installer silently
        Write-Host "Running LLVM installer..."
        $process = Start-Process -FilePath $installerPath -ArgumentList "/S" -Wait -PassThru
        
        if ($process.ExitCode -eq 0) {
            Write-Host "[PASS] LLVM installed successfully"
            Add-ToPath "C:\Program Files\LLVM\bin"
            return $true
        } else {
            Write-Host "[FAIL] LLVM installer failed with exit code $($process.ExitCode)"
            return $false
        }
    } catch {
        Write-Host "[FAIL] Error installing LLVM: $_"
        return $false
    } finally {
        # Clean up installer
        if (Test-Path $installerPath) {
            Remove-Item $installerPath -Force
        }
    }
}

# Function to verify dependency
function Test-Dependency {
    param(
        [string]$Name,
        [hashtable]$Config
    )
    
    Write-Host "`nChecking $Name..."
    
    # Add to PATH if specified
    if ($Config.Path) {
        Add-ToPath $Config.Path
    }
    
    # Check if command exists
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if (-not $command) {
        Write-Host "[FAIL] $Name is not installed or not in PATH"
        return $false
    }
    
    # Check version
    $installedVersion = Get-InstalledVersion $Config.Command $Config.VersionPattern
    if ($installedVersion) {
        $comparison = Compare-Versions $installedVersion $Config.Version
        if ($comparison -ge 0) {
            Write-Host "[PASS] $Name version $installedVersion is installed (meets minimum requirement $($Config.Version))"
            return $true
        } else {
            Write-Host "[FAIL] $Name version $installedVersion is installed but does not meet minimum requirement $($Config.Version)"
            return $false
        }
    } else {
        Write-Host "[FAIL] Could not determine $Name version"
        return $false
    }
}

# Function to install dependency
function Install-Dependency {
    param(
        [string]$Name,
        [hashtable]$Config
    )
    
    Write-Host "Installing $Name..."
    try {
        if ($Name -in @("clang-format", "clang-tidy")) {
            return Install-LLVM
        }
        
        # Check for existing installation
        $existingVersion = Get-InstalledVersion $Config.Command $Config.VersionPattern
        if ($existingVersion) {
            $comparison = Compare-Versions $existingVersion $Config.Version
            if ($comparison -ge 0) {
                Write-Host "[INFO] $Name version $existingVersion is already installed and meets requirements"
                return $true
            } else {
                Write-Host "[INFO] Upgrading $Name from version $existingVersion to $($Config.Version)"
            }
        }
        
        Invoke-Expression $Config.Install
        if ($Config.Path) {
            Add-ToPath $Config.Path
        }
        if (Test-Dependency $Name $Config) {
            Write-Host "[PASS] Successfully installed $Name"
            return $true
        } else {
            Write-Host "[FAIL] Failed to install $Name"
            return $false
        }
    } catch {
        Write-Host "[FAIL] Error installing $Name`: $_"
        return $false
    }
}

# Main script
Write-Host "=== Silent Forge Engine Dependency Setup ==="
Write-Host "This script will verify and install required dependencies."

# Check for Chocolatey
Test-Chocolatey

# Verify and install dependencies
$allDependenciesInstalled = $true
foreach ($dep in $DEPENDENCIES.GetEnumerator()) {
    if (-not (Test-Dependency $dep.Key $dep.Value)) {
        if (Install-Dependency $dep.Key $dep.Value) {
            Write-Host "[PASS] $($dep.Key) is now installed"
        } else {
            $allDependenciesInstalled = $false
            Write-Host "[FAIL] Failed to install $($dep.Key)"
        }
    }
}

# Display final results
Write-Host "`n=== Setup Results ==="
if ($allDependenciesInstalled) {
    Write-Host "[PASS] All dependencies are installed and verified"
    Write-Host "`nNext Steps:"
    Write-Host "1. Run the test script: .\scripts\test_agent_setup.ps1"
    Write-Host "2. Verify CMake configuration: cmake --preset=dev"
} else {
    Write-Host "[FAIL] Some dependencies failed to install"
    Write-Host "`nPlease check the error messages above and try again."
    Write-Host "You may need to run this script as administrator."
}

Write-Host "`nNote: If you prefer manual installation, see CONTRIBUTING.md for details." 