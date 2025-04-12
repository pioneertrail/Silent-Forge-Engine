# Contributing to Silent Forge Engine

This document outlines the development practices and guidelines for the Silent Forge Engine project.

## Development Workflow

### Branch Naming Convention
- `feature/` - New features or enhancements
- `bugfix/` - Bug fixes
- `refactor/` - Code refactoring
- `docs/` - Documentation updates
- `test/` - Test-related changes

### Commit Message Format
```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test-related changes
- `chore`: Maintenance tasks

Example:
```
feat(rendering): add PBR material support

- Implemented physically-based rendering materials
- Added material property editor
- Updated shader system to support PBR

Closes #123
```

## Code Style

### C++ Guidelines
- Follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- Use modern C++17 features where appropriate
- Keep functions small and focused
- Document public interfaces with Doxygen-style comments

### Documentation
- Document all public APIs
- Include examples for complex functionality
- Keep documentation up-to-date with code changes
- Use clear and concise language

## Testing
- Write unit tests for new features
- Maintain test coverage above 80%
- Run tests before submitting changes
- Document test cases and expected behavior

## Review Process
1. Create a feature branch
2. Implement changes
3. Run tests and verify functionality
4. Update documentation
5. Submit pull request
6. Address any feedback
7. Merge when approved

## Tools and Setup
- Use the provided `.clang-format` for code formatting
- Run `clang-tidy` for static analysis
- Use the provided CMake presets for building
- Follow the project's coding standards

## Getting Help
- Check the documentation
- Review existing issues
- Ask in the project discussions
- Contact the maintainer 