#include <iostream> 
#include <vector> 
#include <string> 
#include <algorithm> 
#include <chrono> 
 
int main() { 
    std::vector<std::string> items; 
    for (int i = 0; i < 100000; ++i) { 
        items.push_back("Item " + std::to_string(i)); 
    } 
    std::cout << "Created " << items.size() << " items" << std::endl; 
    return 0; 
} 
