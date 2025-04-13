# Contributing to Silent Forge Engine

## Overview

This document outlines the contribution guidelines for the Silent Forge Engine project, including agent roles, communication protocols, and development standards.

## Agent Communication

### Limitations
- Agents cannot directly communicate with each other
- All coordination must be facilitated through the user
- Interface changes must be documented in `docs/agent_interfaces.md`

### Protocol
1. **Interface Documentation**:
   - Document all interfaces in `docs/agent_interfaces.md`
   - Include purpose, scope, and dependencies
   - Provide usage examples
   - List affected agents

2. **Change Notification**:
   - Update documentation before changes
   - Create PR with clear description
   - Tag affected agents in comments
   - User coordinates review and feedback

3. **Dependency Management**:
   - Declare dependencies in:
     - `CMakeLists.txt`
     - `vcpkg.json`
     - `docs/DEPENDENCIES.md`
   - Coordinate updates through user

4. **Testing Coordination**:
   - Agent 3 defines requirements
   - Other agents provide test interfaces
   - User coordinates test runs

## Code Standards

### C++ Standards
- Use C++17 features
- Follow C++ Core Guidelines
- Use RAII principles
- Implement move semantics
- Avoid raw pointers

### Naming Conventions
- Classes: PascalCase
- Functions: camelCase
- Variables: snake_case
- Constants: UPPER_SNAKE_CASE
- Namespaces: lowercase

### Documentation
- Use Doxygen-style comments
- Document all public interfaces
- Include usage examples
- Note performance implications

### Testing
- Write unit tests for all code
- Maintain >80% coverage
- Test edge cases
- Document test cases

## Version Control

### Branch Structure
- `main`: Production-ready code
- `develop`: Integration branch
- `feature/*`: Feature branches
- `docs/*`: Documentation branches
- `test/*`: Test branches

### Commit Messages
- Format: `<type>(<scope>): <description>`
- Types: feat, fix, docs, test, refactor
- Scope: core, utils, tests, docs
- Description: Clear, concise summary

### PR Process
1. Create branch from `develop`
2. Make changes
3. Update documentation
4. Run tests
5. Create PR
6. Tag affected agents
7. User coordinates review
8. Merge after approval

## Agent Signatures

### Requirements
- Every message must end with signature
- Include role in signature
- Use consistent format
- Never omit signature

### Format
```markdown
-Agent <number> (<role>)
```

### Examples
```markdown
-Agent 1 (Core Systems)
-Agent 2 (Auxiliary Systems)
-Agent 3 (Testing and QA)
-Agent 4 (Documentation and Architecture)
```

## Best Practices

### 1. Communication
- Document all interfaces
- Tag affected agents
- Coordinate through user
- Follow protocol

### 2. Version Control
- Use feature branches
- Follow conventions
- Coordinate merges
- Resolve conflicts

### 3. Testing
- Write unit tests
- Maintain coverage
- Document cases
- Follow guidelines

### 4. Documentation
- Update before changes
- Include examples
- Document breaks
- Maintain changelog

### 5. Signatures
- End every message
- Include role
- Use format
- Never omit

## Example Workflow

1. Agent 1 needs to modify `Gamepad`:
   ```markdown
   ## Changes to Gamepad Interface
   - Added: `getMappedAction` method
   - Modified: `pollEvents` performance target
   - Affected Agents: Agent 2 (Config), Agent 3 (Tests)
   ```

2. User coordinates changes:
   - Shares interface updates
   - Collects feedback
   - Manages timeline

3. Agents implement changes:
   - Agent 1 updates `Gamepad`
   - Agent 2 updates `Config`
   - Agent 3 adds tests
   - Agent 4 updates docs

4. User manages integration:
   - Reviews PRs
   - Coordinates testing
   - Handles deployment 