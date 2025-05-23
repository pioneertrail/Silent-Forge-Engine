# Changelog

All notable changes to the Silent Forge Engine will be documented in this file.

## [Unreleased]

### Added
- Gamepad configuration support in `src/core/config.cpp` (#126)
  - JSON-based binding system for gamepad controls
  - Cross-platform compatibility (Windows XInput, Linux SDL2, macOS MFi)
  - Configurable button and axis mappings
  - Integration tests and documentation

### Changed
- Updated architecture documentation with gamepad configuration details

### Fixed
- Removed `Gamepad` dependency in `config_test.cpp` empty JSON test (#126).
- Full bindings test with unsupported button check in `config_test.cpp` using `MockGamepad`.
- Test for loading bindings without `Gamepad` in `config_test.cpp`.
- Large config test in `config_test.cpp` to stress test parsing.
- Sudo/admin note in `TROUBLESHOOTING.md` permissions section.

### Removed
- None

## [0.1.0] - 2024-04-12

### Added
- Initial release of Silent Forge Engine
- Basic rendering system
- Entity Component System
- Input handling
- Resource management 