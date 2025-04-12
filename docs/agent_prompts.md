# Agent-Specific Prompts

This document contains the standard prompts for each agent in the Silent Forge Engine project.

## Agent 1: Core Systems Development

```markdown
As Agent 1 (Core Systems Development), implement [feature] for the Silent Forge Engine following these guidelines:

1. **Code Standards**:
   - Follow C++ Core Guidelines strictly
   - Use modern C++17 features
   - Optimize for performance
   - Document performance implications
   - Use SIMD where applicable

2. **Architecture**:
   - Follow patterns in docs/ARCHITECTURE.md
   - Maintain clean separation of concerns
   - Ensure thread safety where needed
   - Consider memory layout and cache efficiency

3. **Documentation**:
   - Use Doxygen-style comments
   - Document performance characteristics
   - Include usage examples
   - Note thread safety requirements

4. **Testing**:
   - Provide testable interfaces
   - Document edge cases
   - Note performance benchmarks
   - Specify memory requirements

5. **Branch and Commit**:
   - Use branch: feature/[feature]-agent1
   - Follow commit message format:
     ```
     feat(scope): description

     Detailed description of changes.

     Agent: Agent 1
     Closes #[issue]
     ```

Example prompt for shadow mapping:
"As Agent 1, implement shadow mapping for the Silent Forge Engine's rendering system. Focus on performance optimization and memory efficiency. Use depth textures and implement PCF filtering. Document performance implications and memory requirements. Follow the architecture in docs/ARCHITECTURE.md."
```

## Agent 2: Auxiliary Systems and Features

```markdown
As Agent 2 (Auxiliary Systems and Features), implement [feature] for the Silent Forge Engine following these guidelines:

1. **Code Standards**:
   - Focus on maintainability
   - Ensure cross-platform compatibility
   - Implement clean interfaces
   - Document user-facing features
   - Follow SOLID principles

2. **Architecture**:
   - Integrate with existing systems
   - Use dependency injection
   - Implement event-driven design
   - Consider extensibility

3. **Documentation**:
   - Document user-facing APIs
   - Include usage examples
   - Note platform-specific behavior
   - Document configuration options

4. **Testing**:
   - Write integration tests
   - Test cross-platform behavior
   - Verify user interactions
   - Check error handling

5. **Branch and Commit**:
   - Use branch: feature/[feature]-agent2
   - Follow commit message format:
     ```
     feat(scope): description

     Detailed description of changes.

     Agent: Agent 2
     Closes #[issue]
     ```

Example prompt for input system:
"As Agent 2, implement gamepad support for the Silent Forge Engine's input system. Ensure cross-platform compatibility and provide a clean API for gamepad input. Document platform-specific behavior and configuration options. Follow the architecture in docs/ARCHITECTURE.md."
```

## Agent 3: Testing and Documentation

```markdown
As Agent 3 (Testing and Documentation), implement [feature] for the Silent Forge Engine following these guidelines:

1. **Code Standards**:
   - Ensure test coverage > 80%
   - Follow documentation standards
   - Maintain clear examples
   - Track known issues
   - Use consistent formatting

2. **Architecture**:
   - Document system interactions
   - Update architecture docs
   - Maintain API documentation
   - Track dependencies

3. **Documentation**:
   - Update CHANGELOG.md
   - Maintain troubleshooting guides
   - Document test cases
   - Provide usage examples

4. **Testing**:
   - Write unit tests
   - Create integration tests
   - Document test cases
   - Verify coverage

5. **Branch and Commit**:
   - Use branches: test/[feature]-agent3, docs/[feature]-agent3
   - Follow commit message format:
     ```
     test(scope): description
     or
     docs(scope): description

     Detailed description of changes.

     Agent: Agent 3
     Closes #[issue]
     ```

Example prompt for documentation:
"As Agent 3, create comprehensive documentation for the shadow mapping feature. Write unit tests for the ShadowMap class, update the architecture documentation with shadow mapping details, and add troubleshooting entries for common issues. Ensure test coverage exceeds 80%."
```

## Best Practices for Using Prompts

1. **Customization**:
   - Tailor prompts to specific features
   - Include relevant issue numbers
   - Reference specific files or modules
   - Set clear acceptance criteria

2. **Context**:
   - Provide architectural context
   - Reference related features
   - Note dependencies
   - Specify performance requirements

3. **Quality Control**:
   - Review prompt effectiveness
   - Update based on results
   - Document successful patterns
   - Track common issues

4. **Integration**:
   - Coordinate with other agents
   - Reference related tasks
   - Note dependencies
   - Track progress 