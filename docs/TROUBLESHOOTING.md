# Troubleshooting Guide

## Multi-Agent Issues

### Overlapping File Edits
- **Symptoms**: Merge conflicts when merging PRs from Agent 1 and Agent 2.
- **Solution**:
  1. Assign distinct files/modules to each agent in the issue (e.g., Agent 1: `shadow_map.cpp`, Agent 2: `renderer.cpp`).
  2. Comment in the issue to lock files (e.g., "Agent 1 owns `shadow_map.cpp` until #125").
  3. Merge PRs sequentially, resolving conflicts manually.
- **Prevention**:
  - Define file boundaries in issue subtasks.
  - Use `git diff` to check for overlaps before assigning tasks.

### Inconsistent Code Style
- **Symptoms**: Agent 1 or Agent 2 skips `.clang-format` or deviates from C++ Core Guidelines.
- **Solution**:
  1. Run `clang-format -i src/*` on agent output.
  2. Update agent prompts to emphasize `CONTRIBUTING.md`.
  3. Reject PRs failing format checks in CI.
- **Prevention**:
  - Configure Cursor to auto-apply `.clang-format`.
  - Include style guidelines in every prompt.

### Missing Tests or Documentation
- **Symptoms**: Agent 3 fails to provide tests/docs for Agent 1 or Agent 2's changes.
- **Solution**:
  1. Review PRs for test coverage and doc updates.
  2. Task Agent 3 with backfilling tests/docs in a follow-up PR.
  3. Update Agent 3's prompt to check PR descriptions.
- **Prevention**:
  - Include test/doc requirements in issue subtasks.
  - Use CI to enforce coverage thresholds.

### Prompt Failures
- **Symptoms**: Agent output doesn't match requirements (e.g., Agent 1 skips SIMD optimization).
- **Solution**:
  1. Refine prompt with specific instructions (e.g., "Use SIMD for matrix operations").
  2. Re-run task with updated prompt.
  3. Document successful prompts in `docs/agent_prompts.md`.
- **Prevention**:
  - Test prompts on small tasks first.
  - Maintain a prompt library in `docs/agent_prompts.md`.

### Performance Regressions
- **Symptoms**: Agent 1's changes cause unexpected performance degradation.
- **Solution**:
  1. Run performance tests before and after changes.
  2. Profile critical paths using Tracy.
  3. Revert or optimize problematic changes.
- **Prevention**:
  - Include performance benchmarks in issue templates.
  - Set clear performance targets for each task.

### Cross-Platform Issues
- **Symptoms**: Agent 2's changes work on one platform but fail on others.
- **Solution**:
  1. Test changes on all target platforms.
  2. Use platform-specific CI runners.
  3. Document platform requirements clearly.
- **Prevention**:
  - Include cross-platform testing in CI.
  - Specify platform requirements in issue templates.

### Documentation Gaps
- **Symptoms**: Agent 3's documentation is incomplete or unclear.
- **Solution**:
  1. Review documentation against requirements.
  2. Request specific updates from Agent 3.
  3. Use documentation templates.
- **Prevention**:
  - Include documentation checklists in issues.
  - Set clear documentation standards.

### Integration Failures
- **Symptoms**: Changes from different agents don't work together.
- **Solution**:
  1. Run integration tests before merging.
  2. Coordinate changes through issue comments.
  3. Create integration test plans.
- **Prevention**:
  - Define clear interfaces between components.
  - Include integration testing in CI.

### CI Pipeline Failures
- **Symptoms**: CI checks fail unexpectedly (e.g., build errors, test timeouts, coverage below 80%).
- **Solution**:
  1. Review CI logs to identify the failing step (e.g., `Build`, `Test`, `Generate Coverage`).
  2. For build errors, verify CMake presets and dependencies in `.github/workflows/ci.yml`.
  3. For test failures, check test logs and rerun locally with `ctest --preset=dev --verbose`.
  4. For coverage issues, task Agent 3 with adding tests for uncovered code paths.
  5. For timeouts, optimize tests or increase CI resource limits.
- **Prevention**:
  - Run CI checks locally before pushing (`cmake --build --preset=dev`, `ctest --preset=dev`).
  - Pin dependency versions in CI (e.g., `clang-format-15`, `cmake 3.22`).
  - Monitor CI runtime and optimize long-running tests.
  - Include CI setup instructions in `docs/DEVELOPMENT_WORKFLOW.md`.

## Gamepad Configuration Issues

### Partial Configuration Loading
- **Symptoms**: Some gamepad buttons/axes don't respond despite valid configuration.
- **Cause**: The gamepad doesn't support all configured buttons/axes, leading to skipped bindings.
- **Solution**:
  1. Check logs for warnings like "Button not supported" or "Axis not supported".
  2. Use `Gamepad::isButtonSupported` and `isAxisSupported` to list supported controls.
  3. Update `gamepad.json` to include only supported buttons/axes.
  4. If no bindings load, verify gamepad connection and driver compatibility.
- **Prevention**:
  - Generate configurations dynamically based on detected gamepad capabilities.
  - Test configurations with the target gamepad model before deployment.
  - Document expected bindings in user guides.

### Unsupported Buttons/Axes
- **Symptoms**: Buttons or axes are ignored in configuration.
- **Cause**: Controls not supported by the connected gamepad.
- **Solution**:
  1. Check logs for "Button not supported" or "Axis not supported" warnings.
  2. Verify gamepad capabilities with `Gamepad::isButtonSupported` and `isAxisSupported`.
  3. Update configuration to use only supported controls.
- **Prevention**:
  - Test configuration with target gamepad before deployment.
  - Document supported controls in gamepad documentation.

### Invalid Axis Ranges
- **Symptoms**: Configuration fails to load with "Invalid axis range" error.
- **Cause**: `min` value exceeds `max` value in axis configuration.
- **Solution**:
  1. Check axis range definitions in `gamepad.json`.
  2. Ensure `min` ≤ `max` for all axes.
  3. Use default ranges (-1.0 to 1.0) if unsure.
- **Prevention**:
  - Validate axis ranges during configuration editing.
  - Include range validation in configuration tools.

### Permission Issues
- **Symptoms**: Configuration fails to load with errors like "Failed to open gamepad config".
- **Cause**: Insufficient file or directory permissions on the system.
- **Solution**:
  - **Linux**:
    1. Check permissions with `ls -l gamepad.json`.
    2. Set read permissions: `chmod 644 gamepad.json`.
    3. Verify directory access: `chmod 755 /path/to/config/dir`.
  - **macOS**:
    1. Check permissions with `ls -l gamepad.json`.
    2. Set read permissions: `chmod 644 gamepad.json`.
    3. Ensure user has access to `~/Library/Application Support/SFE`.
  - **Windows**:
    1. Check file properties for read-only status.
    2. Grant read access: `icacls gamepad.json /grant Users:R`.
    3. Verify directory access in `%APPDATA%\SFE`.
  - Ensure the user has sufficient privileges (e.g., not running as restricted user).
  - **Note**: Some commands may require sudo (Linux/macOS) or admin privileges (Windows).
- **Prevention**:
  - Set correct permissions during installation (e.g., 644 for files, 755 for directories).
  - Use platform-standard config paths (e.g., `~/.config/SFE` on Linux).
  - Document permission requirements in setup guides.

### MFi Compliance Warnings
- **Symptoms**: "Config lacks MFi compliance flag" warning on macOS.
- **Cause**: Missing MFi compliance flag in configuration.
- **Solution**:
  1. Add `"mfi_compliant": true` to configuration if using MFi controller.
  2. Set `"mfi_compliant": false` for non-MFi controllers.
  3. Update configuration to match controller type.
- **Prevention**:
  - Include MFi flag in configuration templates.
  - Document MFi requirements for macOS deployment.

### JSON Parsing Errors
- **Symptoms**: "JSON parse error" when loading configuration.
- **Cause**: Malformed JSON in configuration file.
- **Solution**:
  1. Validate JSON syntax using a JSON validator.
  2. Check for missing commas, quotes, or brackets.
  3. Ensure all values are properly formatted.
- **Prevention**:
  - Use JSON schema validation during editing.
  - Include example configurations in documentation.

## Gamepad Issues

### Gamepad Not Detected
- **Symptoms**: No gamepad detected when connected
- **Possible Causes**:
  - SDL2 game controller subsystem not initialized
  - Gamepad not recognized by SDL2
  - Driver issues
- **Solutions**:
  1. Check if SDL2 is properly initialized:
     ```cpp
     if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
         LOG_ERROR("SDL init failed: " + std::string(SDL_GetError()));
     }
     ```
  2. Verify gamepad is recognized:
     ```cpp
     for (int i = 0; i < SDL_NumJoysticks(); ++i) {
         if (SDL_IsGameController(i)) {
             LOG_INFO("Gamepad found at index " + std::to_string(i));
         }
     }
     ```
  3. Check device drivers and USB connection

### Button Mapping Issues
- **Symptoms**: Buttons not responding or mapped incorrectly
- **Possible Causes**:
  - Invalid button configuration
  - Unsupported buttons
  - Configuration file errors
- **Solutions**:
  1. Verify button configuration in JSON:
     ```json
     {
       "buttons": {
         "0": "jump",
         "1": "attack"
       }
     }
     ```
  2. Check button support:
     ```cpp
     if (!gamepad->isButtonSupported(button)) {
         LOG_WARNING("Button not supported");
     }
     ```
  3. Validate configuration file format

### Axis Issues
- **Symptoms**: Analog sticks not working or values incorrect
- **Possible Causes**:
  - Axis not calibrated
  - Deadzone issues
  - Configuration errors
- **Solutions**:
  1. Check axis values:
     ```cpp
     float value = gamepad->getAxisValue(axis);
     LOG_INFO("Axis value: " + std::to_string(value));
     ```
  2. Verify axis configuration:
     ```json
     {
       "axes": {
         "0": {"min": -1.0, "max": 1.0}
       }
     }
     ```
  3. Calibrate gamepad if needed

### Platform-Specific Issues

#### Windows
- Ensure XInput drivers are installed
- Check USB connection and power management settings
- Verify gamepad is recognized in Device Manager

#### Linux
- Check udev rules for gamepad access
- Verify evdev support
- Check permissions for /dev/input devices

#### macOS
- Verify MFi compliance for iOS controllers
- Check USB connection for wired controllers
- Verify Bluetooth pairing for wireless controllers 