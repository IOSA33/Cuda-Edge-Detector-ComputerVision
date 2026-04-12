#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

int main() {
    std::ifstream input("../photos/sample1.jpg", std::ios::binary );
    if (!input.is_open()) {
        std::cout << "Cannot open a photo!" << std::endl;
        return 0;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    std::cout << "Buffer is about " << buffer.size()/1048576 << " MB\n";
}
