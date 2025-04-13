#include <iostream>
#include <vector>
#include <string>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create a large vector to demonstrate some work
    std::vector<std::string> items;
    for (int i = 0; i < 100000; ++i) {
        items.push_back("Item " + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Created " << items.size() << " items in " 
              << duration.count() << "ms" << std::endl;
    
    return 0;
} 