#include <gtest/gtest.h>
#include <core/SceneNode.hpp>
#include <nlohmann/json.hpp>
#include "TestFixture.hpp"
#include <glm/glm.hpp>
#include <memory>

using namespace SFE;
using namespace SFE::Test;

class SceneNodeTest : public TestFixture {
protected:
    void SetUp() override {
        TestFixture::SetUp();
        rootNode = std::make_unique<SceneNode>("Root");
        childNode = std::make_unique<SceneNode>("Child");
        grandChildNode = std::make_unique<SceneNode>("GrandChild");
    }

    void TearDown() override {
        grandChildNode.reset();
        childNode.reset();
        rootNode.reset();
        TestFixture::TearDown();
    }

    std::unique_ptr<SceneNode> rootNode;
    std::unique_ptr<SceneNode> childNode;
    std::unique_ptr<SceneNode> grandChildNode;
};

// Basic Node Operations
TEST_F(SceneNodeTest, NodeCreation) {
    EXPECT_EQ(rootNode->getName(), "Root");
    EXPECT_TRUE(rootNode->getChildren().empty());
    EXPECT_EQ(rootNode->getParent(), nullptr);
}

TEST_F(SceneNodeTest, AddChild) {
    rootNode->addChild(childNode.get());
    EXPECT_EQ(childNode->getParent(), rootNode.get());
    EXPECT_FALSE(rootNode->getChildren().empty());
    EXPECT_EQ(rootNode->getChildren().front(), childNode.get());
}

TEST_F(SceneNodeTest, RemoveChild) {
    rootNode->addChild(childNode.get());
    rootNode->removeChild(childNode.get());
    EXPECT_EQ(childNode->getParent(), nullptr);
    EXPECT_TRUE(rootNode->getChildren().empty());
}

// Transform Operations
TEST_F(SceneNodeTest, LocalTransform) {
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    rootNode->setLocalPosition(position);
    EXPECT_EQ(rootNode->getLocalPosition(), position);
}

TEST_F(SceneNodeTest, WorldTransform) {
    glm::vec3 parentPos(1.0f, 0.0f, 0.0f);
    glm::vec3 childPos(0.0f, 1.0f, 0.0f);
    
    rootNode->setLocalPosition(parentPos);
    childNode->setLocalPosition(childPos);
    rootNode->addChild(childNode.get());
    
    glm::vec3 expectedWorldPos = parentPos + childPos;
    EXPECT_EQ(childNode->getWorldPosition(), expectedWorldPos);
}

// Hierarchy Operations
TEST_F(SceneNodeTest, DeepHierarchy) {
    rootNode->addChild(childNode.get());
    childNode->addChild(grandChildNode.get());
    
    EXPECT_EQ(grandChildNode->getParent(), childNode.get());
    EXPECT_EQ(childNode->getParent(), rootNode.get());
    EXPECT_EQ(rootNode->getChildren().front(), childNode.get());
    EXPECT_EQ(childNode->getChildren().front(), grandChildNode.get());
}

// Performance Tests
TEST_F(SceneNodeTest, Performance_AddRemoveChildren) {
    const int numChildren = 1000;
    std::vector<std::unique_ptr<SceneNode>> children;
    
    TestPerformance([&]() {
        for (int i = 0; i < numChildren; ++i) {
            auto child = std::make_unique<SceneNode>("Child" + std::to_string(i));
            rootNode->addChild(child.get());
            children.push_back(std::move(child));
        }
        
        for (auto& child : children) {
            rootNode->removeChild(child.get());
        }
        children.clear();
    });
}

// Thread Safety Tests
TEST_F(SceneNodeTest, ThreadSafety_AddRemoveChildren) {
    TestThreadSafety(*rootNode, [](SceneNode& node) {
        auto child = std::make_unique<SceneNode>("ThreadChild");
        node.addChild(child.get());
        node.removeChild(child.get());
    });
}

// Memory Tests
TEST_F(SceneNodeTest, MemoryLeak_NodeDestruction) {
    TestMemoryLeak([&]() {
        auto node = std::make_unique<SceneNode>("TempNode");
        node->addChild(std::make_unique<SceneNode>("ChildNode").get());
    });
}

// Edge Cases
TEST_F(SceneNodeTest, AddNullChild) {
    EXPECT_THROW(rootNode->addChild(nullptr), std::invalid_argument);
}

TEST_F(SceneNodeTest, RemoveNonExistentChild) {
    EXPECT_NO_THROW(rootNode->removeChild(childNode.get()));
}

TEST_F(SceneNodeTest, CircularReference) {
    rootNode->addChild(childNode.get());
    EXPECT_THROW(childNode->addChild(rootNode.get()), std::logic_error);
}

// Basic Property Tests
TEST_F(SceneNodeTest, PositionOperations) {
    // Test basic position setting
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    rootNode->setPosition(pos);
    EXPECT_EQ(rootNode->getPosition(), pos) << "Position was not set correctly";
    
    // Test position translation
    rootNode->translate(glm::vec3(1.0f, 1.0f, 1.0f));
    EXPECT_EQ(rootNode->getPosition(), pos + glm::vec3(1.0f, 1.0f, 1.0f)) << "Translation failed";
    
    // Test position with negative values
    rootNode->setPosition(glm::vec3(-1.0f, -2.0f, -3.0f));
    EXPECT_EQ(rootNode->getPosition(), glm::vec3(-1.0f, -2.0f, -3.0f)) << "Negative position values not handled correctly";
}

TEST_F(SceneNodeTest, RotationOperations) {
    // Test basic rotation setting
    glm::quat rot = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rootNode->setRotation(rot);
    EXPECT_EQ(rootNode->getRotation(), rot) << "Rotation was not set correctly";
    
    // Test rotation combination
    rootNode->rotate(glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    EXPECT_NEAR(glm::angle(rootNode->getRotation()), glm::radians(90.0f), 0.001f) << "Rotation combination failed";
    
    // Test rotation with multiple axes
    rootNode->setRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
    EXPECT_NEAR(glm::angle(rootNode->getRotation()), glm::radians(90.0f), 0.001f) << "Multi-axis rotation failed";
}

TEST_F(SceneNodeTest, ScaleOperations) {
    // Test basic scale setting
    glm::vec3 scale(2.0f, 2.0f, 2.0f);
    rootNode->setScale(scale);
    EXPECT_EQ(rootNode->getScale(), scale) << "Scale was not set correctly";
    
    // Test scale multiplication
    rootNode->scale(glm::vec3(0.5f, 0.5f, 0.5f));
    EXPECT_EQ(rootNode->getScale(), glm::vec3(1.0f, 1.0f, 1.0f)) << "Scale multiplication failed";
    
    // Test non-uniform scaling
    rootNode->setScale(glm::vec3(2.0f, 1.0f, 3.0f));
    EXPECT_EQ(rootNode->getScale(), glm::vec3(2.0f, 1.0f, 3.0f)) << "Non-uniform scaling failed";
}

// Error Handling Tests
TEST_F(SceneNodeTest, InvalidDeserialization) {
    nlohmann::json invalidJson = {
        {"position", "invalid"},
        {"rotation", {}},
        {"scale", {}}
    };
    
    EXPECT_THROW(rootNode->deserialize(invalidJson), std::runtime_error) << "Invalid JSON deserialization should throw";
}

// Serialization Tests
TEST_F(SceneNodeTest, SerializationBasic) {
    rootNode->setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    rootNode->setRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    rootNode->setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    
    nlohmann::json json = rootNode->serialize();
    
    EXPECT_EQ(json["position"][0], 1.0f) << "Position serialization failed";
    EXPECT_EQ(json["position"][1], 2.0f) << "Position serialization failed";
    EXPECT_EQ(json["position"][2], 3.0f) << "Position serialization failed";
    EXPECT_NEAR(json["rotation"]["angle"], 45.0f, 0.001f) << "Rotation serialization failed";
    EXPECT_EQ(json["scale"][0], 2.0f) << "Scale serialization failed";
    EXPECT_EQ(json["scale"][1], 2.0f) << "Scale serialization failed";
    EXPECT_EQ(json["scale"][2], 2.0f) << "Scale serialization failed";
}

TEST_F(SceneNodeTest, HierarchySerialization) {
    // Setup hierarchy
    childNode->setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    grandChildNode->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    rootNode->addChild(childNode.get());
    rootNode->addChild(grandChildNode.get());

    // Serialize
    nlohmann::json json = rootNode->serialize();

    // Verify hierarchy
    EXPECT_EQ(json["children"].size(), 2) << "Root children count incorrect";
    EXPECT_EQ(json["children"][0]["position"][0], 1.0f) << "Child position incorrect";
    EXPECT_EQ(json["children"][1]["position"][0], 0.0f) << "Grandchild position incorrect";
    EXPECT_EQ(json["children"][0]["children"].size(), 1) << "Child children count incorrect";
    EXPECT_EQ(json["children"][0]["children"][0]["position"][1], 1.0f) << "Grandchild position incorrect";
}

// Orbit Behavior Tests
TEST_F(SceneNodeTest, OrbitBehavior) {
    // Set up a node with orbit parameters
    glm::vec3 initialPos(2.0f, 0.0f, 0.0f);
    rootNode->setPosition(initialPos);
    
    // Update with known time values
    float deltaTime = 1.0f;  // 1 second
    float currentTime = 0.0f;
    
    rootNode->update(deltaTime, currentTime);
    
    // After 1 second at 30 degrees/sec, position should have rotated 30 degrees
    glm::vec3 expectedPos = glm::vec3(
        2.0f * cos(glm::radians(30.0f)),
        0.0f,
        2.0f * sin(glm::radians(30.0f))
    );
    
    glm::vec3 actualPos = rootNode->getPosition();
    EXPECT_NEAR(actualPos.x, expectedPos.x, 0.001f) << "X position incorrect after orbit";
    EXPECT_NEAR(actualPos.y, expectedPos.y, 0.001f) << "Y position incorrect after orbit";
    EXPECT_NEAR(actualPos.z, expectedPos.z, 0.001f) << "Z position incorrect after orbit";
}

// Mesh and Texture Tests
TEST_F(SceneNodeTest, MeshAndTextureHandling) {
    // Create a mock texture
    auto mockTexture = std::make_shared<Texture>();
    
    // Test texture assignment
    rootNode->setTexture(mockTexture);
    
    // Test serialization with texture
    nlohmann::json json;
    rootNode->serialize(json);
    EXPECT_TRUE(json.contains("texture")) << "Texture not serialized";
    
    // Test deserialization with texture
    SceneNode newNode;
    newNode.deserialize(json);
}

// Parent-Child Relationship Edge Cases
TEST_F(SceneNodeTest, ParentChildEdgeCases) {
    // Test setting parent to self
    EXPECT_THROW(rootNode->setParent(rootNode.get()), std::invalid_argument) 
        << "Setting parent to self should throw";
    
    // Test changing parent
    SceneNode* originalParent = rootNode.get();
    SceneNode newParent("NewParent");
    
    childNode->setParent(originalParent);
    EXPECT_EQ(childNode->getParent(), originalParent) << "Initial parent not set correctly";
    
    childNode->setParent(&newParent);
    EXPECT_EQ(childNode->getParent(), &newParent) << "Parent not changed correctly";
    EXPECT_TRUE(std::find(originalParent->getChildren().begin(), 
                         originalParent->getChildren().end(), 
                         childNode.get()) == originalParent->getChildren().end())
        << "Child not removed from original parent's children";
}

// Transform Hierarchy Validation
TEST_F(SceneNodeTest, TransformHierarchyValidation) {
    // Set up a hierarchy with known transforms
    rootNode->setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    rootNode->setRotation(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
    
    childNode->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    childNode->setParent(rootNode.get());
    
    // Child's world position should be affected by parent's rotation
    glm::vec3 expectedChildWorldPos = glm::vec3(1.0f, 1.0f, 0.0f);
    
    // Update to apply transforms
    rootNode->update(0.016f, 0.0f);
    childNode->update(0.016f, 0.0f);
    
    // Get actual world position through transformation
    glm::vec3 actualChildWorldPos = childNode->getPosition();
    
    EXPECT_NEAR(actualChildWorldPos.x, expectedChildWorldPos.x, 0.001f) << "Child world position X incorrect";
    EXPECT_NEAR(actualChildWorldPos.y, expectedChildWorldPos.y, 0.001f) << "Child world position Y incorrect";
    EXPECT_NEAR(actualChildWorldPos.z, expectedChildWorldPos.z, 0.001f) << "Child world position Z incorrect";
}

// Update Method Time-Based Behavior
TEST_F(SceneNodeTest, UpdateMethodTimeBehavior) {
    // Test continuous updates
    float deltaTime = 0.016f; // 60 FPS
    float currentTime = 0.0f;
    
    rootNode->setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Update for 1 second (60 frames)
    for(int i = 0; i < 60; i++) {
        rootNode->update(deltaTime, currentTime);
        currentTime += deltaTime;
    }
    
    // After 1 second, should have rotated 30 degrees (orbitSpeed default)
    glm::vec3 expectedPos = glm::vec3(
        cos(glm::radians(30.0f)),
        0.0f,
        sin(glm::radians(30.0f))
    );
    
    glm::vec3 actualPos = rootNode->getPosition();
    EXPECT_NEAR(actualPos.x, expectedPos.x, 0.001f) << "Position X incorrect after 1 second of updates";
    EXPECT_NEAR(actualPos.y, expectedPos.y, 0.001f) << "Position Y incorrect after 1 second of updates";
    EXPECT_NEAR(actualPos.z, expectedPos.z, 0.001f) << "Position Z incorrect after 1 second of updates";
} 