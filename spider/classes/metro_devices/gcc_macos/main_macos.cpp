// Minimal main function for macOS build
// This replaces the QNX-specific main.cpp

#include <iostream>
#include <string>

int main() {
    std::cout << "Metro Devices Test - macOS Build" << std::endl;
    std::cout << "This is a minimal build for testing purposes." << std::endl;
    std::cout << "Most QNX-specific functionality has been excluded." << std::endl;
    
    return 0;
}
