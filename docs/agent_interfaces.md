# Agent Interfaces

This document details the interfaces between agents in the Silent Forge Engine project, including purpose, usage, and affected agents.

## Core Systems (Agent 1)

### Gamepad Interface
- **Purpose**: Handles gamepad input and configuration
- **Location**: `include/core/Gamepad.hpp`, `src/core/Gamepad.cpp`
- **Dependencies**: SDL2 (v2.0.14+), `core/config.h`
- **Affected Agents**: Agent 2 (Config), Agent 3 (Tests)

#### Methods
```cpp
/**
 * @brief Check if a button is supported by the gamepad
 * @param button SDL_GameControllerButton to check
 * @return bool True if supported, false otherwise
 */
bool isButtonSupported(SDL_GameControllerButton button) const;

/**
 * @brief Get the action mapped to a button using Config
 * @param button Button to check (SDL_GameControllerButton)
 * @param config Config instance with button mappings
 * @return std::string Action name if pressed, else empty
 */
std::string getMappedAction(SDL_GameControllerButton button, const Config& config) const;

/**
 * @brief Poll gamepad state
 * @return bool True if successful, false if uninitialized
 * @note Targets <0.1ms per call
 */
bool pollEvents();
```

#### Usage Example
```cpp
Gamepad gamepad;
Config config;

if (gamepad.initialize()) {
    config.loadGamepadBindings("gamepad.json");
    
    while (gamepad.pollEvents()) {
        if (gamepad.isButtonSupported(SDL_CONTROLLER_BUTTON_A)) {
            std::string action = gamepad.getMappedAction(
                SDL_CONTROLLER_BUTTON_A, config);
            // Handle action
        }
    }
}
```

#### Performance
- Polling: <0.1ms target
- Memory: O(1) per gamepad
- Dependencies: SDL2 gamepad API

### Renderer Interface
- **Purpose**: Manages rendering pipeline
- **Location**: `include/core/Renderer.hpp`, `src/core/Renderer.cpp`
- **Dependencies**: OpenGL, GLFW
- **Affected Agents**: Agent 3 (Tests)

#### Methods
```cpp
/**
 * @brief Initialize the renderer
 * @param width Window width
 * @param height Window height
 * @return bool True if successful
 */
bool initialize(int width, int height);

/**
 * @brief Render a frame
 * @param scene Scene to render
 * @return bool True if successful
 */
bool renderFrame(const Scene& scene);
```

#### Usage Example
```cpp
Renderer renderer;
Scene scene;

if (renderer.initialize(1280, 720)) {
    while (true) {
        renderer.renderFrame(scene);
    }
}
```

#### Performance
- Frame Time: <16ms target
- Memory: O(n) for scene objects
- Dependencies: OpenGL 4.5+

## Auxiliary Systems (Agent 2)

### Config Interface
- **Purpose**: Manages configuration and settings
- **Location**: `include/core/config.h`, `src/core/config.cpp`
- **Dependencies**: nlohmann/json
- **Affected Agents**: Agent 1 (Gamepad), Agent 3 (Tests)

#### Methods
```cpp
/**
 * @brief Load gamepad bindings from JSON
 * @param filename Path to bindings file
 * @return bool True if successful
 */
bool loadGamepadBindings(const std::string& filename);

/**
 * @brief Get action mapped to button
 * @param button Button to check
 * @return std::string Action name if mapped, else empty
 */
std::string getButtonAction(SDL_GameControllerButton button) const;
```

#### Usage Example
```cpp
Config config;
Gamepad gamepad;

if (config.loadGamepadBindings("gamepad.json")) {
    std::string action = config.getButtonAction(SDL_CONTROLLER_BUTTON_A);
    // Use action
}
```

#### Configuration Format
```json
{
    "buttons": {
        "0": "jump",
        "1": "attack"
    },
    "axes": {
        "0": {"min": -1.0, "max": 1.0}
    }
}
```

### Logger Interface
- **Purpose**: Handles logging and debugging
- **Location**: `include/utils/log.h`, `src/utils/log.cpp`
- **Dependencies**: spdlog
- **Affected Agents**: All agents

#### Methods
```cpp
/**
 * @brief Log an error message
 * @param message Error message
 * @param ... Format arguments
 */
void LOG_ERROR(const char* message, ...);

/**
 * @brief Log an info message
 * @param message Info message
 * @param ... Format arguments
 */
void LOG_INFO(const char* message, ...);
```

#### Usage Example
```cpp
LOG_INFO("Gamepad connected: %s", gamepad.getName().c_str());
LOG_ERROR("Failed to load config: %s", error.c_str());
```

## Testing System (Agent 3)

### TestRunner Interface
- **Purpose**: Executes and reports tests
- **Location**: `tests/TestRunner.hpp`, `tests/TestRunner.cpp`
- **Dependencies**: Catch2
- **Affected Agents**: All agents

#### Methods
```cpp
/**
 * @brief Run all tests
 * @return int Number of failed tests
 */
int runAllTests();

/**
 * @brief Run specific test suite
 * @param suite Test suite name
 * @return int Number of failed tests
 */
int runTestSuite(const std::string& suite);
```

#### Usage Example
```cpp
TestRunner runner;
int failed = runner.runAllTests();
if (failed > 0) {
    LOG_ERROR("%d tests failed", failed);
}
```

#### Test Format
```cpp
TEST_CASE("Gamepad initialization", "[Gamepad]") {
    Gamepad gamepad;
    REQUIRE(gamepad.initialize());
    REQUIRE(gamepad.isConnected());
}
```

## Documentation System (Agent 4)

### Documentation Interface
- **Purpose**: Manages project documentation
- **Location**: `docs/`
- **Dependencies**: None
- **Affected Agents**: All agents

#### Requirements
- All interfaces must be documented
- Include usage examples
- List affected agents
- Note performance implications

#### Documentation Format
```markdown
## Component Name
- **Purpose**: Brief description
- **Location**: File paths
- **Dependencies**: Required libraries
- **Affected Agents**: Other agents

### Methods
[Method documentation]

### Usage Example
[Code example]

### Performance
[Performance details]
```

## Best Practices

### 1. Interface Design
- Keep interfaces minimal and focused
- Document all public methods
- Use clear naming conventions
- Follow project coding standards

### 2. Documentation
- Update documentation before making changes
- Include examples and use cases
- Document breaking changes
- Maintain changelog

### 3. Testing
- Write unit tests for all interfaces
- Maintain test coverage >80%
- Document test cases
- Follow test guidelines

### 4. Communication
- Document all interfaces
- Tag affected agents
- Coordinate through user
- Follow protocol

### 5. Cross-Checking and Violation Handling

#### Agent Responsibilities
- **Agent 1 (Core Systems)**
  - Verify interface documentation completeness
  - Check performance metrics adherence
  - Validate cross-platform compatibility
  - Report violations to user

- **Agent 2 (Auxiliary Systems)**
  - Verify configuration format compliance
  - Check logging standards
  - Validate error handling
  - Report violations to user

- **Agent 3 (Testing and QA)**
  - Verify test coverage requirements
  - Check test documentation
  - Validate performance benchmarks
  - Report violations to user

- **Agent 4 (Documentation and Architecture)**
  - Verify documentation standards
  - Check cross-references
  - Validate format consistency
  - Report violations to user

#### Role-Based Violation Handling
1. **Detection**
   - Agents review work based on their current roles
   - Use automated tools:
     - Code: clang-format, clang-tidy
     - Testing: lcov, Catch2
     - Documentation: markdownlint, vale
     - Performance: gprof, perf
   - Document all findings with agent identity

2. **Reporting**
   - Create issue with `violation` label
   - Reference relevant documentation
   - Tag responsible agent by identity
   - Include suggested fixes
   - **Escalation**: If unresolved after 48 hours, user arbitrates via issue comments

3. **Resolution**
   - Violating agent (by identity) addresses issues
   - Other agents verify fixes based on roles
   - User approves final changes

#### Role Overlap Policy
- **Coordination**
  - Agents coordinate overlapping roles via user
  - Document shared responsibilities in PRs
  - Update role boundaries in `agent_interfaces.md`

- **Common Overlaps**
  - Testing & Documentation (Agent 3 & 4)
  - Performance & Testing (Agent 1 & 3)
  - Configuration & Core (Agent 2 & 1)

- **Resolution**
  - Primary responsibility assigned by user
  - Secondary role provides review
  - Both agents sign off on changes

#### Role History
```markdown
## Role Change History
- 2025-04-12: Initial role assignments defined
- 2025-04-12: Agent 4 added API Design role
- 2025-04-12: Agent 3 added Performance Testing role
```

#### Role Change Example
```markdown
## PR: Agent 1 Role Change
- **Old Role**: Core Systems
- **New Role**: Core Systems, API Design
- **Tasks Handed Off**: None
- **Updated Docs**: `agent_interfaces.md`
- **Tagged**: Agent 4 (Documentation and Architecture)
- **Reason**: Support Gamepad API documentation
```

## Agent System

### Agent Identity and Roles
Each agent has a permanent identity (Agent 1-4) but can take on different roles as needed. This allows for role flexibility while maintaining consistent agent identity and responsibility tracking.

#### Agent Identities
- **Agent 1**: Primary identity for core system development
- **Agent 2**: Primary identity for auxiliary system development
- **Agent 3**: Primary identity for testing and quality assurance
- **Agent 4**: Primary identity for documentation and architecture

#### Dynamic Roles
Roles are assigned based on current task requirements and can change over time. Each agent maintains their primary identity while potentially taking on additional roles.

Example Role Assignments:
```markdown
Current Roles:
- Agent 1: Core Systems, Performance Optimization
- Agent 2: Auxiliary Systems, Configuration Management
- Agent 3: Testing, Quality Assurance, Performance Testing
- Agent 4: Documentation, Architecture, API Design
```

### Cross-Checking and Violation Handling

#### Agent Responsibilities
- **Agent 1 (Core Systems)**
  - Verify interface documentation completeness
  - Check performance metrics adherence
  - Validate cross-platform compatibility
  - Report violations to user

- **Agent 2 (Auxiliary Systems)**
  - Verify configuration format compliance
  - Check logging standards
  - Validate error handling
  - Report violations to user

- **Agent 3 (Testing and QA)**
  - Verify test coverage requirements
  - Check test documentation
  - Validate performance benchmarks
  - Report violations to user

- **Agent 4 (Documentation and Architecture)**
  - Verify documentation standards
  - Check cross-references
  - Validate format consistency
  - Report violations to user

#### Role-Based Violation Handling
1. **Detection**
   - Agents review work based on their current roles
   - Use automated tools where possible
   - Document all findings with agent identity

2. **Reporting**
   - Create issue with `violation` label
   - Reference relevant documentation
   - Tag responsible agent by identity
   - Include suggested fixes

3. **Resolution**
   - Violating agent (by identity) addresses issues
   - Other agents verify fixes based on roles
   - User approves final changes

#### Example Violation Report
```markdown
## Interface Violation Report
- **Component**: Gamepad
- **Violation**: Missing performance documentation
- **Reference**: `docs/agent_interfaces.md#performance`
- **Suggested Fix**: Add performance metrics section
- **Responsible Agent**: Agent 1 (Core Systems)
- **Reported By**: Agent 3 (Testing and QA)
```

#### Role Change Protocol
1. **Notification**
   - Agent announces role change in PR description
   - Updates relevant documentation
   - Tags affected agents

2. **Documentation**
   - Update role assignments in `agent_interfaces.md`
   - Maintain change history
   - Ensure continuity of responsibilities

3. **Transition**
   - Complete current tasks before role change
   - Hand off responsibilities to new role holder
   - Update automated checks and tools 