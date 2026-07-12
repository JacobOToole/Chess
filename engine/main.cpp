//
// Created by jacob on 7/13/2026.
//

#include <iostream>
#include <string>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name my_engine\n";
            std::cout << "id author Jacob\n";
            std::cout << "uciok\n" << std::flush;
        } else if (line == "isready") {
            std::cout << "readyok\n" << std::flush;
        } else if (line == "quit") {
            return 0;
        }
    }
    return 0;
}
