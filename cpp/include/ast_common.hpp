// ast_common.hpp
// Declares/Implements: Pos, Span, Node

#pragma once

#include <cstddef>

struct Pos {
    std::size_t line;
    std::size_t column;

    bool operator==(const Pos& other) const {
        return (line == other.line) && (column == other.column);
    }
    bool operator!=(const Pos& other) const {
        return !operator==(other);
    }
};

struct Span {
    Pos p1;
    Pos p2;
};

template <typename T>
struct Node {
    T value;
    Span s;
};
