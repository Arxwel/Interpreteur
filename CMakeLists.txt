cmake_minimum_required(VERSION 3.8)
project(untitled1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ArbreAbstrait.cpp Interpreteur.cpp Lecteur.cpp Symbole.cpp SymboleValue.cpp TableSymboles.cpp)
add_executable(untitled1 ${SOURCE_FILES})