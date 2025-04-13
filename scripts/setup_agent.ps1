# Silent Forge Engine Agent Setup Script for PowerShell
# This script initializes and configures AI agents in Cursor for the Silent Forge Engine project.

# Configuration
$REPO_URL = "https://github.com/pioneertrail/Silent-Forge-Engine"
$DOCS_DIR = "docs"
$AGENT_PROMPTS = Join-Path $DOCS_DIR "agent_prompts.md"
$DEPENDENCIES = @("cmake", "ninja", "clang-format", "clang-tidy", "lcov")

# Function to display usage
function Show-Usage {
    Write-Host "Usage: $($MyInvocation.MyCommand.Name) <agent_number> [task_issue] [--ci]"
    Write-Host "  agent_number: 1 (Core Systems), 2 (Auxiliary Systems), or 3 (Testing/Docs)"
    Write-Host "  task_issue: Optional GitHub issue number for the task"
    Write-Host "  --ci: Optional flag to run local CI checks"
    exit 1
}

# Function to get agent role
function Get-AgentRole {
    param([int]$AgentNum)
    
    switch ($AgentNum) {
        1 { return "Core Systems Development" }
        2 { return "Auxiliary Systems and Features" }
        3 { return "Testing and Documentation" }
        default { 
            Write-Host "Error: Invalid agent number"
            Show-Usage
        }
    }
}

# Function to get branch prefix
function Get-BranchPrefix {
    param([int]$AgentNum)
    
    switch ($AgentNum) {
        1 { return "feature" }
        2 { return "feature" }
        3 { return "test" }
    }
}

# Function to check dependencies
function Test-Dependencies {
    Write-Host "Checking dependencies..."
    foreach ($dep in $DEPENDENCIES) {
        if (-not (Get-Command $dep -ErrorAction SilentlyContinue)) {
            Write-Host "Error: $dep is not installed. Please install it."
            exit 1
        }
        Write-Host "  - $dep`: OK"
    }
    Write-Host
}

# Function to run local CI checks
function Invoke-LocalCI {
    Write-Host "Running local CI checks..."
    
    Write-Host "  - Configuring build..."
    cmake --preset=dev
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake configuration failed"
        exit 1
    }
    
    Write-Host "  - Building..."
    cmake --build --preset=dev
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed"
        exit 1
    }
    
    Write-Host "  - Running tests..."
    ctest --preset=dev
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Tests failed"
        exit 1
    }
    
    Write-Host "  - Checking format..."
    Get-ChildItem -Path "src" -Recurse -Include "*.cpp","*.h" | ForEach-Object {
        clang-format --dry-run --Werror $_.FullName
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Formatting check failed for $($_.Name)"
            exit 1
        }
    }
    
    Write-Host "Local CI checks passed."
    Write-Host
}

# Function to extract prompt from agent_prompts.md
function Get-AgentPrompt {
    param(
        [int]$AgentNum,
        [string]$TaskIssue
    )
    
    if (-not (Test-Path $AGENT_PROMPTS)) {
        Write-Host "Error: $AGENT_PROMPTS not found"
        exit 1
    }
    
    $content = Get-Content $AGENT_PROMPTS -Raw
    $pattern = "(?s)### Agent $AgentNum.*?```"
    $prompt = [regex]::Match($content, $pattern).Value
    
    if (-not $prompt) {
        Write-Host "Error: Could not find prompt for Agent $AgentNum"
        exit 1
    }
    
    $prompt = $prompt -replace "```$", ""
    
    if ($TaskIssue) {
        $prompt = $prompt -replace "#126", "#$TaskIssue"
    }
    
    return $prompt
}

# Main script
if ($args.Count -lt 1) {
    Show-Usage
}

$AgentNum = [int]$args[0]
$TaskIssue = $args[1]
$RunCI = $args[2] -eq "--ci"

# Validate agent number
if ($AgentNum -notin 1..3) {
    Write-Host "Error: Agent number must be 1, 2, or 3"
    Show-Usage
}

# Get agent role and branch prefix
$AgentRole = Get-AgentRole $AgentNum
$BranchPrefix = Get-BranchPrefix $AgentNum

# Check dependencies
Test-Dependencies

# Display setup instructions
Write-Host "=== Agent $AgentNum ($AgentRole) Setup ==="
Write-Host
Write-Host "1. Configure Cursor:"
Write-Host "   - Name: Agent $AgentNum"
Write-Host "   - Role: $AgentRole"
Write-Host
Write-Host "2. Copy the following prompt to Cursor:"
Write-Host
Get-AgentPrompt $AgentNum $TaskIssue
Write-Host
Write-Host "3. Create branch:"
Write-Host "   git checkout -b $BranchPrefix/[feature]-agent$AgentNum"
Write-Host
Write-Host "4. Run local CI checks before pushing:"
Write-Host "   .\scripts\setup_agent.ps1 $AgentNum $TaskIssue --ci"
Write-Host
Write-Host "5. Start development following $DOCS_DIR\DEVELOPMENT_WORKFLOW.md"
Write-Host
Write-Host "6. Submit PR when ready, referencing issue #$TaskIssue"
Write-Host
Write-Host "Note: Grok-Code and Grok-QA will review your work."
Write-Host "See $DOCS_DIR\TROUBLESHOOTING.md for common issues."

# Optional CI check
if ($RunCI) {
    Invoke-LocalCI
} 