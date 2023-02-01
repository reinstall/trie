cmake_minimum_required(VERSION 3.17)
project(trie)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_library(reinstall
        src/reinstall/trie/trie.h)