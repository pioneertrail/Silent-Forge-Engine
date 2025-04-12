# Development Workflow

This document outlines the development workflow for the Silent Forge Engine project.

## Daily Development Process

1. **Start of Day**
   - Pull latest changes from main branch
   - Review any new issues or pull requests
   - Update local dependencies if needed

2. **Feature Development**
   - Create feature branch from main
   - Implement changes following coding standards
   - Write tests for new functionality
   - Update documentation
   - Run local tests and checks

3. **Code Review Process**
   - Self-review changes
   - Run static analysis tools
   - Check for performance impacts
   - Verify documentation updates

4. **End of Day**
   - Commit changes with descriptive messages
   - Push to remote repository
   - Update issue tracking
   - Plan next day's tasks

## Branch Management

### Branch Types
- `main` - Production-ready code
- `develop` - Integration branch for features
- `feature/*` - New features
- `bugfix/*` - Bug fixes
- `hotfix/*` - Critical fixes for main

### Branch Workflow
1. Create branch from appropriate base
2. Develop feature/fix
3. Run tests and checks
4. Create pull request
5. Address review comments
6. Merge when approved

## Testing Strategy

### Unit Testing
- Write tests for new components
- Maintain test coverage > 80%
- Run tests before commits
- Use test-driven development where appropriate

### Integration Testing
- Test component interactions
- Verify system behavior
- Check performance impacts
- Validate resource management

### Performance Testing
- Profile critical paths
- Check memory usage
- Monitor frame times
- Verify optimization effectiveness

## Code Quality

### Static Analysis
- Run clang-tidy regularly
- Address all warnings
- Follow coding standards
- Document complex logic

### Code Review Checklist
- [ ] Code follows style guide
- [ ] Tests are present and passing
- [ ] Documentation is updated
- [ ] Performance impact considered
- [ ] Security implications reviewed
- [ ] Error handling is appropriate

## Documentation

### Required Documentation
- API documentation
- Architecture decisions
- Performance considerations
- Usage examples
- Troubleshooting guides

### Documentation Updates
- Update with code changes
- Keep examples current
- Document known issues
- Maintain changelog

## Release Process

1. **Preparation**
   - Update version numbers
   - Update changelog
   - Run full test suite
   - Verify documentation

2. **Release**
   - Create release branch
   - Build release artifacts
   - Create release notes
   - Tag release version

3. **Post-Release**
   - Merge to main
   - Update develop branch
   - Announce release
   - Monitor for issues

## Tools and Automation

### Development Tools
- IDE: Visual Studio Code
- Build System: CMake
- Testing: Google Test
- Profiling: Tracy
- Documentation: Doxygen

### Automation
- CI/CD: GitHub Actions
- Code Formatting: clang-format
- Static Analysis: clang-tidy
- Testing: Automated test suite
- Documentation: Automated builds

## Best Practices

1. **Code Organization**
   - Keep functions small and focused
   - Use meaningful names
   - Follow SOLID principles
   - Document complex logic

2. **Version Control**
   - Commit frequently
   - Write clear commit messages
   - Keep commits focused
   - Use feature branches

3. **Testing**
   - Write tests first
   - Test edge cases
   - Maintain test coverage
   - Automate testing

4. **Documentation**
   - Document as you code
   - Keep docs up to date
   - Include examples
   - Explain why, not just what

## Multi-Agent Development

The Silent Forge Engine uses three AI agents (Agent 1, Agent 2, Agent 3) in Cursor to accelerate development while maintaining code quality and consistency.

### Agent Roles and Responsibilities

1. **Agent 1: Core Systems Development**
   - **Primary Focus**: Core engine components and performance-critical systems
   - **Key Responsibilities**:
     - Implement and maintain rendering system
     - Develop ECS architecture
     - Optimize performance-critical code
     - Handle physics and collision systems
   - **Example Tasks**:
     - Implement shadow mapping in `src/rendering/shadow_map.cpp`
     - Optimize scene graph traversal
     - Develop particle system
   - **Code Standards**:
     - Follow C++ Core Guidelines strictly
     - Optimize for performance
     - Use SIMD where applicable
     - Document performance implications

2. **Agent 2: Auxiliary Systems and Features**
   - **Primary Focus**: Supporting systems and user-facing features
   - **Key Responsibilities**:
     - Implement input handling
     - Manage resources and assets
     - Develop audio system
     - Create editor tools
   - **Example Tasks**:
     - Add gamepad support
     - Implement resource caching
     - Create level editor
   - **Code Standards**:
     - Focus on maintainability
     - Ensure cross-platform compatibility
     - Implement clean interfaces
     - Document user-facing features

3. **Agent 3: Testing and Documentation**
   - **Primary Focus**: Quality assurance and documentation
   - **Key Responsibilities**:
     - Write unit and integration tests
     - Maintain project documentation
     - Update changelog
     - Create troubleshooting guides
   - **Example Tasks**:
     - Write tests for new features
     - Update architecture documentation
     - Document API changes
   - **Code Standards**:
     - Ensure test coverage > 80%
     - Follow documentation standards
     - Maintain clear examples
     - Track known issues

### Task Management

1. **Issue Creation**
   - Use GitHub issue templates
   - Assign specific tasks to each agent
   - Include acceptance criteria
   - Set clear deadlines

2. **Branch Strategy**
   - Agent 1: `feature/<feature>-agent1`
   - Agent 2: `feature/<feature>-agent2`
   - Agent 3: `test/<feature>-agent3`, `docs/<feature>-agent3`
   - Example: `feature/shadow-map-agent1`

3. **Commit Messages**
   - Follow conventional commits
   - Include agent identifier
   - Example:
     ```
     feat(rendering): add shadow mapping

     Implemented shadow map generation with depth textures.
     Integrated with material system.

     Agent: Agent 1
     Closes #125
     ```

### Code Review Process

1. **Agent-Specific Checks**
   - **Agent 1**:
     - Performance impact
     - Memory usage
     - Algorithm efficiency
     - Core system integration
   - **Agent 2**:
     - User experience
     - Cross-platform compatibility
     - Resource management
     - Feature completeness
   - **Agent 3**:
     - Test coverage
     - Documentation completeness
     - Example quality
     - Troubleshooting entries

2. **PR Template**
   ```markdown
   ## Description
   [Brief description of changes]

   ## Changes
   - [List of changes]

   ## Agent Assignments
   - Agent 1: [Core system tasks]
   - Agent 2: [Auxiliary system tasks]
   - Agent 3: [Testing and documentation tasks]

   ## Checklist
   - [ ] Code follows CONTRIBUTING.md
   - [ ] Tests included (Agent 3)
   - [ ] Documentation updated (Agent 3)
   - [ ] Performance impact assessed (Agent 1)
   - [ ] Cross-platform compatibility verified (Agent 2)
   ```

### Documentation Management

1. **Agent 3 Responsibilities**
   - Update `CHANGELOG.md`:
     ```markdown
     ## [Unreleased]
     ### Added
     - Shadow mapping in renderer (#125 by Agent 1, tests by Agent 3)
     - Gamepad support in input system (#126 by Agent 2, docs by Agent 3)
     ```
   - Maintain `docs/TROUBLESHOOTING.md`
   - Update `docs/ARCHITECTURE.md`

2. **Code Documentation**
   - Agent 1: Performance implications
   - Agent 2: User-facing features
   - Agent 3: API documentation

### Best Practices

1. **Communication**
   - Use detailed prompts
   - Document agent decisions
   - Track dependencies
   - Log conflicts

2. **Quality Assurance**
   - Enforce code standards
   - Maintain test coverage
   - Profile performance
   - Review documentation

3. **Conflict Prevention**
   - Assign distinct modules
   - Use file locking
   - Coordinate through issues
   - Document boundaries

4. **Performance Monitoring**
   - Track metrics
   - Profile changes
   - Monitor memory
   - Check coverage

### Example Workflow

1. **Feature Implementation**
   ```markdown
   Issue: [FEATURE] Add Shadow Mapping (#125)
   
   Subtasks:
   - Agent 1: Implement shadow map generation
   - Agent 2: Integrate with renderer
   - Agent 3: Write tests and docs
   ```

2. **Development Process**
   - Each agent creates branch:
     - `feature/shadow-map-agent1`
     - `feature/renderer-shadow-agent2`
     - `test/docs-shadow-agent3`
   - Submit separate PRs
   - Run CI checks
   - Review and merge

3. **Documentation Update**
   - Update architecture docs
   - Add API documentation
   - Update changelog
   - Create troubleshooting entries

### Troubleshooting

1. **Common Issues**
   - Code style inconsistencies
   - Documentation gaps
   - Test coverage issues
   - Performance regressions

2. **Resolution Steps**
   - Refine agent prompts
   - Update documentation
   - Add missing tests
   - Profile and optimize

3. **Prevention**
   - Clear task definitions
   - Strict code reviews
   - Automated checks
   - Regular audits 