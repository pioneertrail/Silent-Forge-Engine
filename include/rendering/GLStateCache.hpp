#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <array>
#include <functional>
#include <string_view>

namespace SFE {
namespace Rendering {

/**
 * @brief Caches OpenGL state to avoid redundant state changes
 * 
 * This class tracks common OpenGL state changes and avoids redundant API calls
 * when the requested state is already active.
 */
class GLStateCache {
public:
    using LogCallback = std::function<void(std::string_view)>;

    /**
     * @brief Get the singleton instance
     * @return Reference to the GLStateCache singleton
     */
    static GLStateCache& getInstance();

    /**
     * @brief Set a callback for logging state changes
     * @param callback Function to call for logging
     */
    void setLogCallback(LogCallback callback);

    /**
     * @brief Reset all cached state (call when context changes)
     */
    void reset();

    /**
     * @brief Bind a vertex array object
     * @param vao VAO to bind
     * @return true if a state change was made
     */
    bool bindVAO(GLuint vao);

    /**
     * @brief Bind a buffer to a target
     * @param target Buffer target (GL_ARRAY_BUFFER, etc.)
     * @param buffer Buffer to bind
     * @return true if a state change was made
     */
    bool bindBuffer(GLenum target, GLuint buffer);

    /**
     * @brief Bind a texture to a target and unit
     * @param target Texture target (GL_TEXTURE_2D, etc.)
     * @param unit Texture unit to use
     * @param texture Texture to bind
     * @return true if a state change was made
     */
    bool bindTexture(GLenum target, GLuint unit, GLuint texture);

    /**
     * @brief Use a shader program
     * @param program Program to use
     * @return true if a state change was made
     */
    bool useProgram(GLuint program);

    /**
     * @brief Enable or disable a capability
     * @param cap Capability to change
     * @param enabled Whether to enable or disable
     * @return true if a state change was made
     */
    bool setEnabled(GLenum cap, bool enabled);

    /**
     * @brief Set blend function
     * @param sfactor Source factor
     * @param dfactor Destination factor
     * @return true if a state change was made
     */
    bool setBlendFunc(GLenum sfactor, GLenum dfactor);

    /**
     * @brief Set depth function
     * @param func Depth function to use
     * @return true if a state change was made
     */
    bool setDepthFunc(GLenum func);

private:
    GLStateCache();
    ~GLStateCache() = default;
    
    // Delete copy and move operations
    GLStateCache(const GLStateCache&) = delete;
    GLStateCache& operator=(const GLStateCache&) = delete;
    GLStateCache(GLStateCache&&) = delete;
    GLStateCache& operator=(GLStateCache&&) = delete;

    // Logging
    void log(std::string_view message) const;

    // State tracking
    GLuint m_boundVAO{0};
    std::unordered_map<GLenum, GLuint> m_boundBuffers;
    std::array<GLuint, 32> m_boundTextures{};  // Assuming up to 32 texture units
    GLuint m_currentProgram{0};
    std::unordered_map<GLenum, bool> m_enabledCaps;
    struct {
        GLenum sfactor{GL_ONE};
        GLenum dfactor{GL_ZERO};
    } m_blendFunc;
    GLenum m_depthFunc{GL_LESS};

    // Logging callback
    LogCallback m_logCallback;
};

} // namespace Rendering
} // namespace SFE 