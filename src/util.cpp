//
// Created by Kristóf Hetényi on 2022. 12. 06..
//
#include <string>
#include "sstream"
#include <vector>

std::vector<std::string> splitStr(const std::string& str, char c) {
    std::vector<std::string> args;
    std::stringstream stream(str);
    std::string segment;

    while(getline(stream, segment, c))
    {
        args.push_back(segment);
    }

    return args;
}
