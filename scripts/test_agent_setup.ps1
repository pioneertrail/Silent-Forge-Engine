# Silent Forge Engine Agent Setup Test Script
# This script tests the agent setup process and verifies all components work correctly.

# Configuration
$REPO_URL = "https://github.com/pioneertrail/Silent-Forge-Engine"
$DOCS_DIR = "docs"
$AGENT_PROMPTS = Join-Path $DOCS_DIR "agent_prompts.md"
$SETUP_SCRIPT = Join-Path $PSScriptRoot "setup_agent.ps1"

# Test Results
$TestResults = @{
    "Script Exists" = $false
    "Docs Directory" = $false
    "Agent Prompts" = $false
    "Prompt Format" = $false
    "Dependencies" = $false
    "CI Commands" = $false
}

# Function to display test results
function Show-TestResults {
    Write-Host "`n=== Agent Setup Test Results ==="
    foreach ($test in $TestResults.GetEnumerator()) {
        $status = if ($test.Value) { "[PASS]" } else { "[FAIL]" }
        Write-Host "$status $($test.Name)"
    }
    Write-Host "`nNote: Some tests may require manual verification."
}

# Test 1: Verify setup script exists
if (Test-Path $SETUP_SCRIPT) {
    $TestResults["Script Exists"] = $true
    Write-Host "[PASS] Setup script found at $SETUP_SCRIPT"
} else {
    Write-Host "[FAIL] Setup script not found at $SETUP_SCRIPT"
}

# Test 2: Verify docs directory exists
if (Test-Path $DOCS_DIR) {
    $TestResults["Docs Directory"] = $true
    Write-Host "[PASS] Docs directory found at $DOCS_DIR"
} else {
    Write-Host "[FAIL] Docs directory not found at $DOCS_DIR"
}

# Test 3: Verify agent prompts file exists
if (Test-Path $AGENT_PROMPTS) {
    $TestResults["Agent Prompts"] = $true
    Write-Host "[PASS] Agent prompts file found at $AGENT_PROMPTS"
} else {
    Write-Host "[FAIL] Agent prompts file not found at $AGENT_PROMPTS"
}

# Test 4: Verify prompt format
if ($TestResults["Agent Prompts"]) {
    $content = Get-Content $AGENT_PROMPTS -Raw
    $promptFormatValid = $true
    
    # Check for all required agent sections
    1..3 | ForEach-Object {
        $pattern = "### Agent $_"
        if (-not ($content -match $pattern)) {
            $promptFormatValid = $false
            Write-Host "[FAIL] Missing prompt section for Agent $_"
        }
    }
    
    if ($promptFormatValid) {
        $TestResults["Prompt Format"] = $true
        Write-Host "[PASS] Prompt format is valid"
    }
}

# Test 5: Verify dependencies
$dependencies = @("cmake", "ninja", "clang-format", "clang-tidy", "lcov")
$allDependenciesFound = $true

foreach ($dep in $dependencies) {
    if (Get-Command $dep -ErrorAction SilentlyContinue) {
        Write-Host "[PASS] $dep is installed"
    } else {
        $allDependenciesFound = $false
        Write-Host "[FAIL] $dep is not installed"
    }
}

if ($allDependenciesFound) {
    $TestResults["Dependencies"] = $true
    Write-Host "[PASS] All dependencies are installed"
}

# Test 6: Verify CI commands
$ciCommands = @(
    @{Command = "cmake --preset=dev"; Description = "CMake configuration"},
    @{Command = "cmake --build --preset=dev"; Description = "CMake build"},
    @{Command = "ctest --preset=dev"; Description = "CTest"},
    @{Command = "clang-format --version"; Description = "Clang-format"}
)

$allCiCommandsValid = $true
foreach ($cmd in $ciCommands) {
    try {
        $output = Invoke-Expression $cmd.Command 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[PASS] $($cmd.Description) command works"
        } else {
            $allCiCommandsValid = $false
            Write-Host "[FAIL] $($cmd.Description) command failed"
        }
    } catch {
        $allCiCommandsValid = $false
        Write-Host "[FAIL] $($cmd.Description) command failed: $_"
    }
}

if ($allCiCommandsValid) {
    $TestResults["CI Commands"] = $true
    Write-Host "[PASS] All CI commands are working"
}

# Display final results
Show-TestResults

# Provide next steps
Write-Host "`nNext Steps:"
if ($TestResults.Values -contains $false) {
    Write-Host "1. Fix the failed tests above"
    Write-Host "2. Run this test script again"
} else {
    Write-Host "1. Test agent setup with: .\scripts\setup_agent.ps1 1 126"
    Write-Host "2. Verify the prompt is correctly extracted"
    Write-Host "3. Run CI checks with: .\scripts\setup_agent.ps1 1 126 --ci"
}

Write-Host "`nNote: This test script does not verify the actual agent behavior in Cursor."
Write-Host "      For that, use the onboarding test issue template." 