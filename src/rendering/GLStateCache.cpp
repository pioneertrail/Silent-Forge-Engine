#include "rendering/GLStateCache.hpp"
#include <iostream>
#include <sstream>

namespace SFE {
namespace Rendering {

GLStateCache::GLStateCache() {
    reset();
}

GLStateCache& GLStateCache::getInstance() {
    static GLStateCache instance;
    return instance;
}

void GLStateCache::setLogCallback(LogCallback callback) {
    m_logCallback = std::move(callback);
}

void GLStateCache::reset() {
    m_boundVAO = 0;
    m_boundBuffers.clear();
    m_boundTextures.fill(0);
    m_currentProgram = 0;
    m_enabledCaps.clear();
    m_blendFunc.sfactor = GL_ONE;
    m_blendFunc.dfactor = GL_ZERO;
    m_depthFunc = GL_LESS;
    log("GLStateCache reset");
}

bool GLStateCache::bindVAO(GLuint vao) {
    if (m_boundVAO == vao) {
        return false;
    }
    
    glBindVertexArray(vao);
    m_boundVAO = vao;
    
    std::stringstream ss;
    ss << "Bound VAO " << vao;
    log(ss.str());
    return true;
}

bool GLStateCache::bindBuffer(GLenum target, GLuint buffer) {
    auto it = m_boundBuffers.find(target);
    if (it != m_boundBuffers.end() && it->second == buffer) {
        return false;
    }
    
    glBindBuffer(target, buffer);
    m_boundBuffers[target] = buffer;
    
    std::stringstream ss;
    ss << "Bound buffer " << buffer << " to target " << target;
    log(ss.str());
    return true;
}

bool GLStateCache::bindTexture(GLenum target, GLuint unit, GLuint texture) {
    if (unit >= m_boundTextures.size()) {
        log("Texture unit out of bounds");
        return false;
    }
    
    if (m_boundTextures[unit] == texture) {
        return false;
    }
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture);
    m_boundTextures[unit] = texture;
    
    std::stringstream ss;
    ss << "Bound texture " << texture << " to unit " << unit;
    log(ss.str());
    return true;
}

bool GLStateCache::useProgram(GLuint program) {
    if (m_currentProgram == program) {
        return false;
    }
    
    glUseProgram(program);
    m_currentProgram = program;
    
    std::stringstream ss;
    ss << "Using program " << program;
    log(ss.str());
    return true;
}

bool GLStateCache::setEnabled(GLenum cap, bool enabled) {
    auto it = m_enabledCaps.find(cap);
    if (it != m_enabledCaps.end() && it->second == enabled) {
        return false;
    }
    
    if (enabled) {
        glEnable(cap);
    } else {
        glDisable(cap);
    }
    
    m_enabledCaps[cap] = enabled;
    
    std::stringstream ss;
    ss << (enabled ? "Enabled " : "Disabled ") << cap;
    log(ss.str());
    return true;
}

bool GLStateCache::setBlendFunc(GLenum sfactor, GLenum dfactor) {
    if (m_blendFunc.sfactor == sfactor && m_blendFunc.dfactor == dfactor) {
        return false;
    }
    
    glBlendFunc(sfactor, dfactor);
    m_blendFunc.sfactor = sfactor;
    m_blendFunc.dfactor = dfactor;
    
    std::stringstream ss;
    ss << "Set blend func source=" << sfactor << ", dest=" << dfactor;
    log(ss.str());
    return true;
}

bool GLStateCache::setDepthFunc(GLenum func) {
    if (m_depthFunc == func) {
        return false;
    }
    
    glDepthFunc(func);
    m_depthFunc = func;
    
    std::stringstream ss;
    ss << "Set depth function " << func;
    log(ss.str());
    return true;
}

void GLStateCache::log(std::string_view message) const {
    if (m_logCallback) {
        m_logCallback(message);
    }
}

} // namespace Rendering
} // namespace SFE 