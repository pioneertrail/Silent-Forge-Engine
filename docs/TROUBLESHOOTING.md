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