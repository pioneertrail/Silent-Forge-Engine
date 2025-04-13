#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

namespace SFE {
namespace Test {

class TestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void TearDown() override {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        EXPECT_LT(duration.count(), 1000) << "Test took too long to execute";
    }

    template<typename T>
    void TestThreadSafety(T& object, const std::function<void(T&)>& operation, int numThreads = 4, int numOperations = 10000) {
        std::vector<std::thread> threads;
        std::atomic<bool> success(true);
        std::atomic<int> errorCount(0);

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&]() {
                try {
                    for (int j = 0; j < numOperations; ++j) {
                        operation(object);
                    }
                } catch (const std::exception& e) {
                    success = false;
                    errorCount++;
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        EXPECT_TRUE(success) << "Thread safety test failed with " << errorCount << " errors";
    }

    template<typename T>
    void TestPerformance(const std::function<void()>& operation, int numOperations = 100000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < numOperations; ++i) {
            operation();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        EXPECT_LT(duration.count(), 500) << "Performance test took too long";
    }

    void TestMemoryLeak(const std::function<void()>& operation) {
        {
            operation();
        }
        // MemoryLeakDetector will verify no leaks occurred
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
};

} // namespace Test
} // namespace SFE 