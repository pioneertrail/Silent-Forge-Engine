#!/bin/bash

# Silent Forge Engine Agent Setup Script
# This script helps initialize and configure AI agents in Cursor for the Silent Forge Engine project.

# Configuration
REPO_URL="https://github.com/pioneertrail/Silent-Forge-Engine"
DOCS_DIR="docs"
AGENT_PROMPTS="$DOCS_DIR/agent_prompts.md"

# Function to display usage
usage() {
    echo "Usage: $0 <agent_number> [task_issue]"
    echo "  agent_number: 1 (Core Systems), 2 (Auxiliary Systems), or 3 (Testing/Docs)"
    echo "  task_issue: Optional GitHub issue number for the task"
    exit 1
}

# Function to get agent role
get_agent_role() {
    case $1 in
        1) echo "Core Systems Development" ;;
        2) echo "Auxiliary Systems and Features" ;;
        3) echo "Testing and Documentation" ;;
        *) echo "Invalid agent number" ; usage ;;
    esac
}

# Function to get branch prefix
get_branch_prefix() {
    case $1 in
        1) echo "feature" ;;
        2) echo "feature" ;;
        3) echo "test" ;;
    esac
}

# Function to extract prompt from agent_prompts.md
extract_prompt() {
    local agent_num=$1
    local task_issue=$2
    
    # Read the prompt template
    local prompt_template=$(sed -n "/### Agent $agent_num/,/```/p" "$AGENT_PROMPTS" | sed '$d')
    
    # Replace placeholders if task issue is provided
    if [ ! -z "$task_issue" ]; then
        prompt_template=$(echo "$prompt_template" | sed "s/#126/#$task_issue/g")
    fi
    
    echo "$prompt_template"
}

# Main script
if [ $# -lt 1 ]; then
    usage
fi

AGENT_NUM=$1
TASK_ISSUE=$2

# Validate agent number
if ! [[ "$AGENT_NUM" =~ ^[1-3]$ ]]; then
    echo "Error: Agent number must be 1, 2, or 3"
    usage
fi

# Get agent role and branch prefix
AGENT_ROLE=$(get_agent_role "$AGENT_NUM")
BRANCH_PREFIX=$(get_branch_prefix "$AGENT_NUM")

# Extract and display the prompt
echo "=== Agent $AGENT_NUM ($AGENT_ROLE) Setup ==="
echo
echo "1. Configure Cursor:"
echo "   - Name: Agent $AGENT_NUM"
echo "   - Role: $AGENT_ROLE"
echo
echo "2. Copy the following prompt to Cursor:"
echo
extract_prompt "$AGENT_NUM" "$TASK_ISSUE"
echo
echo "3. Create branch:"
echo "   git checkout -b $BRANCH_PREFIX/[feature]-agent$AGENT_NUM"
echo
echo "4. Start development following the workflow in $DOCS_DIR/DEVELOPMENT_WORKFLOW.md"
echo
echo "5. Submit PR when ready, referencing issue #$TASK_ISSUE"
echo
echo "Note: Grok-Code and Grok-QA will review your work."
echo "See $DOCS_DIR/TROUBLESHOOTING.md for common issues." 