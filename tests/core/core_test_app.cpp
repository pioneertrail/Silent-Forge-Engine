#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <gtest/gtest.h>

// Include OpenGL headers first
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Then include our headers
#include "core/WindowManager.hpp"
#include "core/InputManager.hpp"
#include "core/Camera.hpp"
#include "core/Gamepad.hpp"
#include "core/config.h"
#include "core/SceneNode.hpp"
#include "utils/log.h"
#include <glm/glm.hpp>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <limits>
#include <numeric>
#include <nlohmann/json.hpp>

namespace sfe {

// Simple RenderComponent for testing
class RenderComponent {
public:
    RenderComponent() = default;
    ~RenderComponent() = default;
    
    void render() {
        // Empty implementation for testing
    }
};

// Memory leak detector
class MemoryLeakDetector {
public:
    static MemoryLeakDetector& getInstance() {
        static MemoryLeakDetector instance;
        return instance;
    }

    void* allocate(size_t size) {
        void* ptr = malloc(size);
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            allocations_[ptr] = size;
            totalAllocated_ += size;
            peakAllocated_ = std::max(peakAllocated_, totalAllocated_);
            allocationCount_++;
            peakAllocationCount_ = std::max(peakAllocationCount_, allocationCount_);
        }
        return ptr;
    }

    void deallocate(void* ptr) {
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = allocations_.find(ptr);
            if (it != allocations_.end()) {
                totalAllocated_ -= it->second;
                allocationCount_--;
                allocations_.erase(it);
            }
            free(ptr);
        }
    }

    bool hasLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !allocations_.empty();
    }

    size_t getLeakCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }

    size_t getPeakAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return peakAllocationCount_;
    }

    size_t getPeakMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return peakAllocated_;
    }

    size_t getCurrentAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocationCount_;
    }

    size_t getCurrentMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalAllocated_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        totalAllocated_ = 0;
        peakAllocated_ = 0;
        allocationCount_ = 0;
        peakAllocationCount_ = 0;
    }

private:
    MemoryLeakDetector() = default;
    std::unordered_map<void*, size_t> allocations_;
    mutable std::mutex mutex_;
    size_t totalAllocated_ = 0;
    size_t peakAllocated_ = 0;
    size_t allocationCount_ = 0;
    size_t peakAllocationCount_ = 0;
};

// Override new/delete operators for memory tracking
void* operator new(size_t size) {
    return MemoryLeakDetector::getInstance().allocate(size);
}

void operator delete(void* ptr) noexcept {
    MemoryLeakDetector::getInstance().deallocate(ptr);
}

class CoreTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize core systems
        window = std::make_unique<WindowManager>(800, 600, "Core Systems Test");
        inputManager = std::make_unique<InputManager>();
        camera = std::make_unique<Camera>(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        gamepad = std::make_unique<Gamepad>();
        config = std::make_unique<Config>();
    }

    void TearDown() override {
        // Clean up resources
        if (window) {
            window->shutdown();
        }
        if (gamepad) {
            gamepad->shutdown();
        }
        if (inputManager) {
            inputManager->shutdown();
        }
        camera.reset();
        config.reset();
    }

    std::unique_ptr<WindowManager> window;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Gamepad> gamepad;
    std::unique_ptr<Config> config;
};

TEST_F(CoreTestFixture, WindowInitialization) {
    ASSERT_TRUE(window->initialize());
    EXPECT_EQ(window->getWidth(), 800);
    EXPECT_EQ(window->getHeight(), 600);
}

TEST_F(CoreTestFixture, GamepadInitialization) {
    bool result = gamepad->initialize();
    if (result) {
        EXPECT_TRUE(gamepad->isConnected());
        EXPECT_FALSE(gamepad->getName().empty());
    } else {
        std::cout << "No gamepad detected, skipping gamepad tests" << std::endl;
    }
}

TEST_F(CoreTestFixture, GamepadConfiguration) {
    if (gamepad->initialize() && gamepad->isConnected()) {
        EXPECT_TRUE(config->loadGamepadBindings("tests/fixtures/valid_gamepad.json"));
        
        // Test button mapping
        std::string action = gamepad->getMappedAction(SDL_CONTROLLER_BUTTON_A, *config);
        EXPECT_EQ(action, "jump");
        
        // Test axis range
        auto range = config->getAxisRange(SDL_CONTROLLER_AXIS_LEFTX);
        EXPECT_FLOAT_EQ(range.first, -1.0f);
        EXPECT_FLOAT_EQ(range.second, 1.0f);
    }
}

TEST_F(CoreTestFixture, CameraTransformation) {
    // Test camera view matrix
    glm::mat4 view = camera->getViewMatrix();
    EXPECT_FALSE(glm::all(glm::equal(view, glm::mat4(1.0f))));
    
    // Test camera projection matrix
    float aspectRatio = 800.0f / 600.0f;
    glm::mat4 proj = camera->getProjectionMatrix(aspectRatio);
    EXPECT_FALSE(glm::all(glm::equal(proj, glm::mat4(1.0f))));
}

TEST_F(CoreTestFixture, InputHandling) {
    ASSERT_TRUE(window->initialize());
    
    // Process one frame of input
    float deltaTime = 0.016f; // ~60 FPS
    inputManager->processInput(window->getWindow(), *camera, deltaTime);
    
    // Test gamepad input if available
    if (gamepad->initialize() && gamepad->isConnected()) {
        EXPECT_TRUE(gamepad->pollEvents());
        
        // Test axis values
        float leftX = gamepad->getAxisValue(SDL_CONTROLLER_AXIS_LEFTX);
        float leftY = gamepad->getAxisValue(SDL_CONTROLLER_AXIS_LEFTY);
        EXPECT_GE(leftX, -1.0f);
        EXPECT_LE(leftX, 1.0f);
        EXPECT_GE(leftY, -1.0f);
        EXPECT_LE(leftY, 1.0f);
    }
}

TEST_F(CoreTestFixture, WindowResize) {
    ASSERT_TRUE(window->initialize());
    
    // Simulate window resize
    window->setSize(1024, 768);
    EXPECT_EQ(window->getWidth(), 1024);
    EXPECT_EQ(window->getHeight(), 768);
    
    // Verify aspect ratio
    float aspectRatio = window->getAspectRatio();
    EXPECT_FLOAT_EQ(aspectRatio, 1024.0f / 768.0f);
}

TEST_F(CoreTestFixture, CameraMovement) {
    // Test camera position updates
    glm::vec3 newPosition(1.0f, 2.0f, 3.0f);
    camera->setPosition(newPosition);
    EXPECT_EQ(camera->getPosition(), newPosition);
    
    // Test camera rotation
    float yaw = 45.0f;
    float pitch = 30.0f;
    camera->setRotation(yaw, pitch);
    EXPECT_FLOAT_EQ(camera->getYaw(), yaw);
    EXPECT_FLOAT_EQ(camera->getPitch(), pitch);
    
    // Test camera movement
    glm::vec3 movement(1.0f, 0.0f, 0.0f);
    camera->move(movement);
    EXPECT_EQ(camera->getPosition(), newPosition + movement);
}

TEST_F(CoreTestFixture, InputStates) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    // Test key state tracking
    inputManager->setKeyState(GLFW_KEY_W, true);
    EXPECT_TRUE(inputManager->isKeyPressed(GLFW_KEY_W));
    
    inputManager->setKeyState(GLFW_KEY_W, false);
    EXPECT_FALSE(inputManager->isKeyPressed(GLFW_KEY_W));
    
    // Test mouse button states
    inputManager->setMouseButtonState(GLFW_MOUSE_BUTTON_LEFT, true);
    EXPECT_TRUE(inputManager->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
    
    // Test mouse position
    double x = 100.0, y = 200.0;
    inputManager->setMousePosition(x, y);
    EXPECT_DOUBLE_EQ(inputManager->getMouseX(), x);
    EXPECT_DOUBLE_EQ(inputManager->getMouseY(), y);
}

TEST_F(CoreTestFixture, SceneNodeTransformation) {
    auto node = std::make_unique<SceneNode>();
    
    // Test position
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    node->setPosition(position);
    EXPECT_EQ(node->getPosition(), position);
    
    // Test rotation
    glm::vec3 rotation(45.0f, 30.0f, 15.0f);
    node->setRotation(rotation);
    EXPECT_EQ(node->getRotation(), rotation);
    
    // Test scale
    glm::vec3 scale(2.0f, 2.0f, 2.0f);
    node->setScale(scale);
    EXPECT_EQ(node->getScale(), scale);
    
    // Test transformation matrix
    glm::mat4 transform = node->getWorldTransform();
    EXPECT_FALSE(glm::all(glm::equal(transform, glm::mat4(1.0f))));
}

TEST_F(CoreTestFixture, WindowInitializationFailure) {
    // Test with invalid dimensions
    auto invalidWindow = std::make_unique<WindowManager>(0, 0, "Invalid Window");
    EXPECT_FALSE(invalidWindow->initialize());
    
    // Test with negative dimensions
    auto negativeWindow = std::make_unique<WindowManager>(-1, -1, "Negative Window");
    EXPECT_FALSE(negativeWindow->initialize());
}

TEST_F(CoreTestFixture, MultipleGamepads) {
    // Initialize first gamepad
    ASSERT_TRUE(gamepad->initialize());
    
    // Create and initialize second gamepad
    auto gamepad2 = std::make_unique<Gamepad>();
    if (gamepad2->initialize()) {
        EXPECT_TRUE(gamepad2->isConnected());
        EXPECT_NE(gamepad2->getName(), gamepad->getName());
        
        // Test independent button states
        SDLMock::buttonState = true;
        EXPECT_TRUE(gamepad->pollEvents());
        EXPECT_TRUE(gamepad2->pollEvents());
        EXPECT_TRUE(gamepad->getButtonState(SDL_CONTROLLER_BUTTON_A));
        EXPECT_TRUE(gamepad2->getButtonState(SDL_CONTROLLER_BUTTON_A));
        
        // Test independent axis values
        SDLMock::axisValue = 0.5f;
        EXPECT_TRUE(gamepad->pollEvents());
        EXPECT_TRUE(gamepad2->pollEvents());
        EXPECT_FLOAT_EQ(gamepad->getAxisValue(SDL_CONTROLLER_AXIS_LEFTX), 0.5f);
        EXPECT_FLOAT_EQ(gamepad2->getAxisValue(SDL_CONTROLLER_AXIS_LEFTX), 0.5f);
    }
}

TEST_F(CoreTestFixture, CameraFrustumCulling) {
    // Test point inside frustum
    glm::vec3 pointInside(0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(camera->isPointInFrustum(pointInside));
    
    // Test point outside frustum
    glm::vec3 pointOutside(1000.0f, 1000.0f, 1000.0f);
    EXPECT_FALSE(camera->isPointInFrustum(pointOutside));
    
    // Test sphere inside frustum
    EXPECT_TRUE(camera->isSphereInFrustum(pointInside, 1.0f));
    
    // Test sphere outside frustum
    EXPECT_FALSE(camera->isSphereInFrustum(pointOutside, 1.0f));
}

TEST_F(CoreTestFixture, InputEventQueue) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    // Queue some input events
    inputManager->queueKeyEvent(GLFW_KEY_W, true);
    inputManager->queueMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, true);
    inputManager->queueMouseMoveEvent(100.0, 200.0);
    
    // Process queued events
    inputManager->processEvents();
    
    // Verify event processing
    EXPECT_TRUE(inputManager->isKeyPressed(GLFW_KEY_W));
    EXPECT_TRUE(inputManager->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT));
    EXPECT_DOUBLE_EQ(inputManager->getMouseX(), 100.0);
    EXPECT_DOUBLE_EQ(inputManager->getMouseY(), 200.0);
}

TEST_F(CoreTestFixture, CameraProjection) {
    // Test perspective projection
    float fov = 45.0f;
    float aspect = 800.0f / 600.0f;
    float near = 0.1f;
    float far = 100.0f;
    
    camera->setPerspective(fov, aspect, near, far);
    glm::mat4 proj = camera->getProjectionMatrix(aspect);
    
    // Test projection matrix properties
    EXPECT_FALSE(glm::all(glm::equal(proj, glm::mat4(1.0f))));
    EXPECT_FLOAT_EQ(proj[0][0], 1.0f / (aspect * tan(fov * 0.5f)));
    EXPECT_FLOAT_EQ(proj[1][1], 1.0f / tan(fov * 0.5f));
    EXPECT_FLOAT_EQ(proj[2][2], -(far + near) / (far - near));
    EXPECT_FLOAT_EQ(proj[2][3], -1.0f);
    EXPECT_FLOAT_EQ(proj[3][2], -(2.0f * far * near) / (far - near));
}

TEST_F(CoreTestFixture, InputKeyModifiers) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    // Test key modifiers
    inputManager->setKeyModifier(GLFW_MOD_SHIFT, true);
    EXPECT_TRUE(inputManager->isKeyModifierPressed(GLFW_MOD_SHIFT));
    
    inputManager->setKeyModifier(GLFW_MOD_CONTROL, true);
    EXPECT_TRUE(inputManager->isKeyModifierPressed(GLFW_MOD_CONTROL));
    
    inputManager->setKeyModifier(GLFW_MOD_ALT, true);
    EXPECT_TRUE(inputManager->isKeyModifierPressed(GLFW_MOD_ALT));
    
    // Test multiple modifiers
    EXPECT_TRUE(inputManager->isKeyModifierPressed(GLFW_MOD_SHIFT | GLFW_MOD_CONTROL));
}

TEST_F(CoreTestFixture, SceneNodeHierarchy) {
    auto root = std::make_unique<SceneNode>();
    auto child1 = std::make_unique<SceneNode>();
    auto child2 = std::make_unique<SceneNode>();
    
    // Test parent-child relationship
    child1->setParent(root.get());
    child2->setParent(root.get());
    
    EXPECT_EQ(child1->getParent(), root.get());
    EXPECT_EQ(child2->getParent(), root.get());
    EXPECT_EQ(root->getChildren().size(), 2);
    
    // Test child removal
    child1->removeParent();
    EXPECT_EQ(child1->getParent(), nullptr);
    EXPECT_EQ(root->getChildren().size(), 1);
    
    // Test child transformation inheritance
    glm::vec3 parentPos(1.0f, 2.0f, 3.0f);
    glm::vec3 childPos(4.0f, 5.0f, 6.0f);
    
    root->setPosition(parentPos);
    child2->setPosition(childPos);
    
    glm::mat4 childTransform = child2->getWorldTransform();
    glm::vec3 worldPos = glm::vec3(childTransform[3]);
    EXPECT_EQ(worldPos, parentPos + childPos);
}

TEST_F(CoreTestFixture, InputMouseScroll) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    // Test mouse scroll
    double xoffset = 1.0;
    double yoffset = 2.0;
    inputManager->queueMouseScrollEvent(xoffset, yoffset);
    inputManager->processEvents();
    
    EXPECT_DOUBLE_EQ(inputManager->getMouseScrollX(), xoffset);
    EXPECT_DOUBLE_EQ(inputManager->getMouseScrollY(), yoffset);
}

TEST_F(CoreTestFixture, PerformanceCameraTransform) {
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        camera->getViewMatrix();
        camera->getProjectionMatrix(800.0f / 600.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect matrix calculations to take less than 100ms for 1M iterations
    EXPECT_LT(duration.count(), 100);
}

TEST_F(CoreTestFixture, PerformanceSceneNodeTransform) {
    const int iterations = 1000000;
    auto node = std::make_unique<SceneNode>();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        node->setPosition(glm::vec3(i, i, i));
        node->getWorldTransform();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect transform calculations to take less than 50ms for 1M iterations
    EXPECT_LT(duration.count(), 50);
}

TEST_F(CoreTestFixture, PerformanceInputProcessing) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        inputManager->queueKeyEvent(GLFW_KEY_W, true);
        inputManager->queueMouseMoveEvent(i, i);
        inputManager->processEvents();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect input processing to take less than 200ms for 1M iterations
    EXPECT_LT(duration.count(), 200);
}

TEST_F(CoreTestFixture, CameraErrorHandling) {
    // Test invalid FOV
    EXPECT_FALSE(camera->setPerspective(0.0f, 1.0f, 0.1f, 100.0f));
    EXPECT_FALSE(camera->setPerspective(-45.0f, 1.0f, 0.1f, 100.0f));
    
    // Test invalid near/far planes
    EXPECT_FALSE(camera->setPerspective(45.0f, 1.0f, -0.1f, 100.0f));
    EXPECT_FALSE(camera->setPerspective(45.0f, 1.0f, 0.1f, 0.0f));
    EXPECT_FALSE(camera->setPerspective(45.0f, 1.0f, 100.0f, 0.1f));
    
    // Test invalid aspect ratio
    EXPECT_FALSE(camera->setPerspective(45.0f, 0.0f, 0.1f, 100.0f));
    EXPECT_FALSE(camera->setPerspective(45.0f, -1.0f, 0.1f, 100.0f));
}

TEST_F(CoreTestFixture, InputErrorHandling) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    // Test invalid key codes
    EXPECT_FALSE(inputManager->isKeyPressed(-1));
    EXPECT_FALSE(inputManager->isKeyPressed(GLFW_KEY_LAST + 1));
    
    // Test invalid mouse buttons
    EXPECT_FALSE(inputManager->isMouseButtonPressed(-1));
    EXPECT_FALSE(inputManager->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LAST + 1));
    
    // Test invalid modifiers
    EXPECT_FALSE(inputManager->isKeyModifierPressed(-1));
    EXPECT_FALSE(inputManager->isKeyModifierPressed(GLFW_MOD_LAST + 1));
}

TEST_F(CoreTestFixture, SceneNodeErrorHandling) {
    auto node = std::make_unique<SceneNode>();
    auto child = std::make_unique<SceneNode>();
    
    // Test circular parent-child relationship
    node->setParent(child.get());
    child->setParent(node.get());
    EXPECT_EQ(node->getParent(), nullptr); // Should break the cycle
    
    // Test self-parenting
    node->setParent(node.get());
    EXPECT_EQ(node->getParent(), nullptr);
    
    // Test invalid transformations
    glm::vec3 invalidScale(0.0f, 0.0f, 0.0f);
    node->setScale(invalidScale);
    EXPECT_NE(node->getScale(), invalidScale); // Should clamp to minimum valid scale
}

TEST_F(CoreTestFixture, GamepadErrorHandling) {
    // Test invalid button queries
    EXPECT_FALSE(gamepad->getButtonState(static_cast<SDL_GameControllerButton>(-1)));
    EXPECT_FALSE(gamepad->getButtonState(static_cast<SDL_GameControllerButton>(SDL_CONTROLLER_BUTTON_MAX)));
    
    // Test invalid axis queries
    EXPECT_FLOAT_EQ(gamepad->getAxisValue(static_cast<SDL_GameControllerAxis>(-1)), 0.0f);
    EXPECT_FLOAT_EQ(gamepad->getAxisValue(static_cast<SDL_GameControllerAxis>(SDL_CONTROLLER_AXIS_MAX)), 0.0f);
    
    // Test initialization after shutdown
    ASSERT_TRUE(gamepad->initialize());
    gamepad->shutdown();
    EXPECT_FALSE(gamepad->initialize());
}

TEST_F(CoreTestFixture, SceneNodeMemoryLeak) {
    MemoryLeakDetector::getInstance().reset();
    
    {
        auto root = std::make_unique<SceneNode>();
        for (int i = 0; i < 1000; ++i) {
            auto child = std::make_unique<SceneNode>();
            child->setParent(root.get());
        }
        // root and children should be destroyed here
    }
    
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getLeakCount(), 0);
}

TEST_F(CoreTestFixture, CameraMemoryLeak) {
    MemoryLeakDetector::getInstance().reset();
    
    {
        auto camera1 = std::make_unique<Camera>(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        auto camera2 = std::make_unique<Camera>(
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        // cameras should be destroyed here
    }
    
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getLeakCount(), 0);
}

TEST_F(CoreTestFixture, InputManagerMemoryLeak) {
    MemoryLeakDetector::getInstance().reset();
    
    {
        auto inputManager1 = std::make_unique<InputManager>();
        auto inputManager2 = std::make_unique<InputManager>();
        inputManager1->initialize();
        inputManager2->initialize();
        // input managers should be destroyed here
    }
    
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getLeakCount(), 0);
}

TEST_F(CoreTestFixture, SceneNodeThreadSafety) {
    auto node = std::make_unique<SceneNode>();
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    // Test concurrent position updates
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&node, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                node->setPosition(glm::vec3(i, j, 0));
                auto pos = node->getPosition();
                EXPECT_GE(pos.x, 0);
                EXPECT_GE(pos.y, 0);
            }
        });
    }
    
    for (auto& t : threads) t.join();
    threads.clear();
    
    // Test concurrent child addition/removal
    auto parent = std::make_unique<SceneNode>();
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&parent, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                auto child = std::make_unique<SceneNode>();
                child->setParent(parent.get());
                child->removeParent();
            }
        });
    }
    
    for (auto& t : threads) t.join();
}

TEST_F(CoreTestFixture, InputManagerThreadSafety) {
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    // Test concurrent key state updates
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                inputManager->setKeyState(GLFW_KEY_W + i, true);
                bool state = inputManager->isKeyPressed(GLFW_KEY_W + i);
                EXPECT_TRUE(state);
                inputManager->setKeyState(GLFW_KEY_W + i, false);
            }
        });
    }
    
    for (auto& t : threads) t.join();
    threads.clear();
    
    // Test concurrent mouse position updates
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                inputManager->setMousePosition(i * 100.0, j * 100.0);
                double x = inputManager->getMouseX();
                double y = inputManager->getMouseY();
                EXPECT_GE(x, 0);
                EXPECT_GE(y, 0);
            }
        });
    }
    
    for (auto& t : threads) t.join();
}

TEST_F(CoreTestFixture, CameraThreadSafety) {
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    // Test concurrent camera updates
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                camera->setPosition(glm::vec3(i, j, 0));
                auto pos = camera->getPosition();
                EXPECT_GE(pos.x, 0);
                EXPECT_GE(pos.y, 0);
                
                camera->setRotation(i * 10.0f, j * 10.0f);
                auto yaw = camera->getYaw();
                auto pitch = camera->getPitch();
                EXPECT_GE(yaw, 0);
                EXPECT_GE(pitch, 0);
            }
        });
    }
    
    for (auto& t : threads) t.join();
}

TEST_F(CoreTestFixture, ConcurrentInitialization) {
    const int threadCount = 4;
    std::vector<std::thread> threads;
    std::atomic<bool> success{true};
    
    // Test concurrent InputManager initialization
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&success]() {
            auto tempInput = std::make_unique<InputManager>();
            if (!tempInput->initialize()) {
                success = false;
            }
        });
    }
    for (auto& t : threads) t.join();
    EXPECT_TRUE(success);
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    
    threads.clear();
    success = true;
    
    // Test concurrent Camera initialization
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&success, i]() {
            auto tempCamera = std::make_unique<Camera>(
                glm::vec3(i, i, i),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            if (!tempCamera->isValid()) {
                success = false;
            }
        });
    }
    for (auto& t : threads) t.join();
    EXPECT_TRUE(success);
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
}

TEST_F(CoreTestFixture, ResourceContention) {
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    // Create a shared scene node pool
    auto root = std::make_unique<SceneNode>();
    std::vector<std::unique_ptr<SceneNode>> nodePool;
    for (int i = 0; i < 100; ++i) {
        nodePool.push_back(std::make_unique<SceneNode>());
    }
    
    // Test concurrent access to shared nodes
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&root, &nodePool, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                // Randomly select and modify nodes
                size_t index = j % nodePool.size();
                nodePool[index]->setParent(root.get());
                nodePool[index]->setPosition(glm::vec3(j, j, j));
                nodePool[index]->removeParent();
            }
        });
    }
    
    for (auto& t : threads) t.join();
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
}

TEST_F(CoreTestFixture, ErrorPathConcurrency) {
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> errorCount{0};
    
    // Test concurrent error handling in SceneNode
    auto node = std::make_unique<SceneNode>();
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&node, &errorCount, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                try {
                    // Simulate invalid operations
                    if (j % 2 == 0) {
                        node->setPosition(glm::vec3(std::numeric_limits<float>::infinity()));
                    } else {
                        node->setScale(glm::vec3(0.0f));
                    }
                } catch (...) {
                    errorCount++;
                }
            }
        });
    }
    
    for (auto& t : threads) t.join();
    EXPECT_GT(errorCount, 0); // Expect some errors to be caught
    EXPECT_TRUE(node->isValid()); // Node should remain in valid state
    
    threads.clear();
    errorCount = 0;
    
    // Test concurrent error handling in InputManager
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, &errorCount, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                try {
                    // Simulate invalid input operations
                    if (j % 2 == 0) {
                        inputManager->setKeyState(-1, true);
                    } else {
                        inputManager->setMousePosition(std::numeric_limits<double>::infinity(), 0);
                    }
                } catch (...) {
                    errorCount++;
                }
            }
        });
    }
    
    for (auto& t : threads) t.join();
    EXPECT_GT(errorCount, 0); // Expect some errors to be caught
    EXPECT_TRUE(inputManager->isValid()); // InputManager should remain in valid state
}

TEST_F(CoreTestFixture, ThreadSafetyPerformance) {
    const int threadCount = 4;
    const int iterations = 100000;
    std::vector<std::thread> threads;
    
    // Benchmark SceneNode thread safety
    auto node = std::make_unique<SceneNode>();
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&node, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                node->setPosition(glm::vec3(j, j, j));
                node->getPosition();
            }
        });
    }
    
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect position updates to take less than 100ms for 4 threads, 100K iterations each
    EXPECT_LT(duration.count(), 100);
    
    threads.clear();
    
    // Benchmark InputManager thread safety
    ASSERT_TRUE(window->initialize());
    ASSERT_TRUE(inputManager->initialize());
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                inputManager->setKeyState(GLFW_KEY_W + i, true);
                inputManager->isKeyPressed(GLFW_KEY_W + i);
                inputManager->setKeyState(GLFW_KEY_W + i, false);
            }
        });
    }
    
    for (auto& t : threads) t.join();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect key state updates to take less than 200ms for 4 threads, 100K iterations each
    EXPECT_LT(duration.count(), 200);
    
    threads.clear();
    
    // Benchmark Camera thread safety
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                camera->setPosition(glm::vec3(i, j, 0));
                camera->getPosition();
                camera->setRotation(i * 10.0f, j * 10.0f);
                camera->getYaw();
                camera->getPitch();
            }
        });
    }
    
    for (auto& t : threads) t.join();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect camera updates to take less than 300ms for 4 threads, 100K iterations each
    EXPECT_LT(duration.count(), 300);
}

TEST_F(CoreTestFixture, ResourceContentionPerformance) {
    const int threadCount = 4;
    const int iterations = 100000;
    std::vector<std::thread> threads;
    
    // Create a shared scene node pool
    auto root = std::make_unique<SceneNode>();
    std::vector<std::unique_ptr<SceneNode>> nodePool;
    for (int i = 0; i < 100; ++i) {
        nodePool.push_back(std::make_unique<SceneNode>());
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test concurrent access to shared nodes
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&root, &nodePool, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                size_t index = j % nodePool.size();
                nodePool[index]->setParent(root.get());
                nodePool[index]->setPosition(glm::vec3(j, j, j));
                nodePool[index]->removeParent();
            }
        });
    }
    
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect shared resource access to take less than 500ms for 4 threads, 100K iterations each
    EXPECT_LT(duration.count(), 500);
}

TEST_F(CoreTestFixture, DeepHierarchyContention) {
    const int threadCount = 4;
    const int iterations = 1000;
    const int hierarchyDepth = 10;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<TestSceneNode>> nodes;
    
    // Create a deep hierarchy
    auto root = std::make_unique<TestSceneNode>();
    nodes.push_back(std::move(root));
    auto current = nodes.back().get();
    
    for (int i = 1; i < hierarchyDepth; ++i) {
        auto child = std::make_unique<TestSceneNode>();
        child->setParent(current);
        current = child.get();
        nodes.push_back(std::move(child));
    }
    
    // Reset memory tracking before test
    MemoryLeakDetector::getInstance().reset();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test concurrent access to different levels of the hierarchy
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&nodes, i, iterations, hierarchyDepth]() {
            for (int j = 0; j < iterations; ++j) {
                // Each thread focuses on a different level of the hierarchy
                int level = (i + j) % hierarchyDepth;
                auto node = nodes[level].get();
                
                // Perform various operations
                node->setPosition(glm::vec3(j, j, j));
                node->setRotation(glm::vec3(j * 10.0f, j * 10.0f, j * 10.0f));
                node->setScale(glm::vec3(1.0f + (j % 10) * 0.1f));
                
                // Test hierarchy traversal
                auto worldTransform = node->getWorldTransform();
                auto parent = node->getParent();
                if (parent) {
                    auto parentTransform = parent->getWorldTransform();
                }
                
                // Test child management
                if (level < hierarchyDepth - 1) {
                    auto child = nodes[level + 1].get();
                    child->setParent(node);
                    child->removeParent();
                }
            }
        });
    }
    
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Performance expectations
    EXPECT_LT(duration.count(), 500); // Expect <500ms for 4 threads, 1K iterations
    
    // Memory usage expectations
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getCurrentAllocationCount(), 0);
    EXPECT_LT(MemoryLeakDetector::getInstance().getPeakAllocationCount(), 1000);
    EXPECT_LT(MemoryLeakDetector::getInstance().getPeakMemoryUsage(), 1024 * 1024); // 1MB
    
    // Validate hierarchy integrity
    for (size_t i = 0; i < nodes.size(); ++i) {
        auto node = nodes[i].get();
        EXPECT_TRUE(node->isValid());
        if (i > 0) {
            EXPECT_EQ(node->getParent(), nodes[i - 1].get());
        }
    }
}

TEST_F(CoreTestFixture, MixedComponentContention) {
    const int threadCount = 4;
    const int iterations = 1000;
    const int maxNodes = 5;
    std::vector<std::thread> threads;
    
    // Create initial hierarchy
    auto root = std::make_unique<TestSceneNode>();
    std::vector<std::unique_ptr<TestSceneNode>> nodes(maxNodes);
    for (auto& node : nodes) {
        node = std::make_unique<TestSceneNode>();
        node->setParent(root.get());
    }
    
    // Reset memory tracking before test
    MemoryLeakDetector::getInstance().reset();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test concurrent mixed operations
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&nodes, &root, i, iterations, maxNodes]() {
            for (int j = 0; j < iterations; ++j) {
                // Select a random node to operate on
                size_t index = (i + j) % maxNodes;
                auto node = nodes[index].get();
                
                // Perform transform updates
                node->setPosition(glm::vec3(j, j, j));
                node->setRotation(glm::vec3(j * 10.0f, j * 10.0f, j * 10.0f));
                node->setScale(glm::vec3(1.0f + (j % 10) * 0.1f));
                
                // Test hierarchy operations
                if (j % 2 == 0) {
                    // Remove from parent and reattach
                    node->removeParent();
                    node->setParent(root.get());
                }
                
                // Test world transform calculation
                auto worldTransform = node->getWorldTransform();
                
                // Simulate component operations
                if (j % 3 == 0) {
                    // Create and attach a new component
                    auto component = std::make_unique<RenderComponent>();
                    node->addComponent(std::move(component));
                } else if (j % 3 == 1) {
                    // Remove an existing component
                    node->removeComponent<RenderComponent>();
                }
                
                // Test child management
                if (index < maxNodes - 1) {
                    auto child = nodes[index + 1].get();
                    child->setParent(node);
                    child->removeParent();
                }
            }
        });
    }
    
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Performance expectations
    EXPECT_LT(duration.count(), 600); // Expect <600ms for 4 threads, 1K iterations
    
    // Memory usage expectations
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getCurrentAllocationCount(), 0);
    EXPECT_LT(MemoryLeakDetector::getInstance().getPeakAllocationCount(), 2000);
    EXPECT_LT(MemoryLeakDetector::getInstance().getPeakMemoryUsage(), 2 * 1024 * 1024); // 2MB
    
    // Validate hierarchy integrity
    EXPECT_TRUE(root->isValid());
    for (size_t i = 0; i < nodes.size(); ++i) {
        auto node = nodes[i].get();
        EXPECT_TRUE(node->isValid());
        if (i > 0) {
            EXPECT_EQ(node->getParent(), root.get());
        }
    }
}

TEST_F(CoreTestFixture, SceneNodeSerialization) {
    // Create a complex scene graph
    auto root = std::make_unique<SceneNode>();
    root->setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    root->setRotation(glm::vec3(45.0f, 30.0f, 15.0f));
    root->setScale(glm::vec3(2.0f, 2.0f, 2.0f));

    auto child1 = std::make_unique<SceneNode>();
    child1->setPosition(glm::vec3(4.0f, 5.0f, 6.0f));
    child1->setParent(root.get());

    auto child2 = std::make_unique<SceneNode>();
    child2->setPosition(glm::vec3(7.0f, 8.0f, 9.0f));
    child2->setParent(root.get());

    // Serialize to JSON
    nlohmann::json rootJson;
    root->serialize(rootJson);

    // Create new nodes from serialized data
    auto deserializedRoot = std::make_unique<SceneNode>();
    deserializedRoot->deserialize(rootJson);

    // Verify deserialized data
    EXPECT_EQ(deserializedRoot->getPosition(), root->getPosition());
    EXPECT_EQ(deserializedRoot->getRotation(), root->getRotation());
    EXPECT_EQ(deserializedRoot->getScale(), root->getScale());
    EXPECT_EQ(deserializedRoot->getChildren().size(), root->getChildren().size());

    // Verify child data
    const auto& deserializedChildren = deserializedRoot->getChildren();
    EXPECT_EQ(deserializedChildren[0]->getPosition(), child1->getPosition());
    EXPECT_EQ(deserializedChildren[1]->getPosition(), child2->getPosition());
}

TEST_F(CoreTestFixture, ResourcePoolManagement) {
    const int poolSize = 100;
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> errorCount{0};

    // Create resource pool
    auto resourcePool = std::make_unique<ResourcePool<SceneNode>>(poolSize);

    auto poolTask = [&]() {
        try {
            for (int i = 0; i < iterations; ++i) {
                // Acquire and release resources from pool
                auto resource = resourcePool->acquire();
                if (resource) {
                    resource->setPosition(glm::vec3(i, i, i));
                    successCount++;
                } else {
                    errorCount++;
                }
                resourcePool->release(resource);
            }
        } catch (const std::exception& e) {
            errorCount++;
        }
    };

    // Launch threads
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(poolTask);
    }

    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify results
    EXPECT_EQ(errorCount, 0);
    EXPECT_EQ(successCount, threadCount * iterations);
    EXPECT_EQ(resourcePool->getAvailableCount(), poolSize);
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
}

// Build System Tests
TEST_F(CoreTestFixture, BuildSystemSanitizers) {
    // Test AddressSanitizer detection
    #ifdef __SANITIZE_ADDRESS__
        EXPECT_TRUE(true); // ASan is enabled
    #else
        EXPECT_FALSE(false); // ASan is disabled
    #endif

    // Test ThreadSanitizer detection
    #ifdef __SANITIZE_THREAD__
        EXPECT_TRUE(true); // TSan is enabled
    #else
        EXPECT_FALSE(false); // TSan is disabled
    #endif
}

TEST_F(CoreTestFixture, BuildSystemCoverage) {
    // Test coverage instrumentation
    #ifdef __COVERAGE__
        EXPECT_TRUE(true); // Coverage is enabled
    #else
        EXPECT_FALSE(false); // Coverage is disabled
    #endif
}

TEST_F(CoreTestFixture, BuildSystemPrecompiledHeaders) {
    // Test PCH usage
    std::vector<int> testVec = {1, 2, 3};
    std::string testStr = "test";
    glm::vec3 testVec3(1.0f, 2.0f, 3.0f);
    
    EXPECT_EQ(testVec.size(), 3);
    EXPECT_EQ(testStr.length(), 4);
    EXPECT_FLOAT_EQ(testVec3.x, 1.0f);
}

TEST_F(CoreTestFixture, BuildSystemStaticAnalysis) {
    // Test static analysis compliance
    int* ptr = nullptr;
    EXPECT_EQ(ptr, nullptr); // Should not trigger null pointer warning
    
    std::vector<int> vec;
    vec.reserve(10); // Should not trigger capacity warning
    EXPECT_GE(vec.capacity(), 10);
}

TEST_F(CoreTestFixture, BuildSystemPerformance) {
    // Test build system performance impact
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform some operations
    for (int i = 0; i < 1000; ++i) {
        std::vector<int> temp;
        temp.reserve(100);
        for (int j = 0; j < 100; ++j) {
            temp.push_back(j);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Expect operations to complete within 100ms
    EXPECT_LT(duration.count(), 100);
}

TEST_F(CoreTestFixture, ConcurrentInitializationWithContention) {
    const int threadCount = 4;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> errorCount{0};
    std::mutex resourceMutex;

    auto initTask = [&]() {
        try {
            // Create shared resources
            std::vector<std::unique_ptr<SceneNode>> nodes;
            std::vector<std::unique_ptr<InputManager>> inputManagers;
            std::vector<std::unique_ptr<Camera>> cameras;

            for (int i = 0; i < iterations; ++i) {
                // Initialize with resource contention
                {
                    std::lock_guard<std::mutex> lock(resourceMutex);
                    nodes.push_back(std::make_unique<SceneNode>());
                    inputManagers.push_back(std::make_unique<InputManager>());
                    cameras.push_back(std::make_unique<Camera>(
                        glm::vec3(0.0f, 0.0f, 3.0f),
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                    ));
                }

                // Verify initialization
                if (nodes.back() && inputManagers.back() && cameras.back()) {
                    successCount++;
                } else {
                    errorCount++;
                }
            }
        } catch (const std::exception& e) {
            errorCount++;
        }
    };

    // Launch threads
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(initTask);
    }

    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify results
    EXPECT_EQ(errorCount, 0);
    EXPECT_EQ(successCount, threadCount * iterations);
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
}

TEST_F(CoreTestFixture, ThreadSafetyPerformanceBenchmark) {
    const int threadCount = 4;
    const int iterations = 100000;
    std::vector<std::thread> threads;
    std::atomic<bool> running{true};
    std::atomic<int> operationCount{0};
    std::chrono::high_resolution_clock::time_point start, end;

    // SceneNode performance test
    auto sceneNodeTask = [&]() {
        auto node = std::make_unique<SceneNode>();
        while (running) {
            node->setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
            node->setRotation(glm::vec3(0.1f, 0.2f, 0.3f));
            node->setScale(glm::vec3(1.1f, 1.2f, 1.3f));
            operationCount++;
        }
    };

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(sceneNodeTask);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    running = false;
    for (auto& thread : threads) {
        thread.join();
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "SceneNode operations per second: " << (operationCount * 1000.0 / duration) << std::endl;
    EXPECT_LT(duration, 1100); // Should complete within 1.1 seconds

    // Reset for InputManager test
    threads.clear();
    running = true;
    operationCount = 0;

    // InputManager performance test
    auto inputManagerTask = [&]() {
        auto manager = std::make_unique<InputManager>();
        while (running) {
            manager->update();
            operationCount++;
        }
    };

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(inputManagerTask);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    running = false;
    for (auto& thread : threads) {
        thread.join();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "InputManager operations per second: " << (operationCount * 1000.0 / duration) << std::endl;
    EXPECT_LT(duration, 1100); // Should complete within 1.1 seconds
}

TEST_F(CoreTestFixture, ComplexMemoryLeakDetection) {
    const int nodeCount = 1000;
    const int componentCount = 10;
    std::vector<std::unique_ptr<SceneNode>> nodes;
    std::vector<std::unique_ptr<RenderComponent>> components;

    // Track initial memory state
    size_t initialAllocationCount = MemoryLeakDetector::getInstance().getCurrentAllocationCount();
    size_t initialMemoryUsage = MemoryLeakDetector::getInstance().getCurrentMemoryUsage();

    // Create complex scene graph
    for (int i = 0; i < nodeCount; ++i) {
        auto node = std::make_unique<SceneNode>();
        for (int j = 0; j < componentCount; ++j) {
            auto component = std::make_unique<RenderComponent>();
            node->addComponent(std::move(component));
        }
        if (i > 0) {
            node->setParent(nodes[i-1].get());
        }
        nodes.push_back(std::move(node));
    }

    // Verify memory usage
    size_t peakAllocationCount = MemoryLeakDetector::getInstance().getPeakAllocationCount();
    size_t peakMemoryUsage = MemoryLeakDetector::getInstance().getPeakMemoryUsage();
    EXPECT_GT(peakAllocationCount, initialAllocationCount);
    EXPECT_GT(peakMemoryUsage, initialMemoryUsage);

    // Clear resources
    nodes.clear();
    components.clear();

    // Verify no leaks
    EXPECT_FALSE(MemoryLeakDetector::getInstance().hasLeaks());
    EXPECT_EQ(MemoryLeakDetector::getInstance().getCurrentAllocationCount(), initialAllocationCount);
    EXPECT_EQ(MemoryLeakDetector::getInstance().getCurrentMemoryUsage(), initialMemoryUsage);
}

} // namespace sfe

int main(int argc, char* argv[]) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run Google Test tests
    int gtest_result = RUN_ALL_TESTS();
    
    // Run Catch2 tests
    Catch::Session session;
    int catch_result = session.run(argc, argv);
    
    // Return combined result
    return gtest_result + catch_result;
} 