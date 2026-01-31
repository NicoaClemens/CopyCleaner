// ast_common.hpp
// Declares/Implements: Pos, Span, Node

#pragma once

#include <cstddef>

/// @brief Position in source code. Has operators `==` and `!=` to compare two Pos
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

/// @brief Span between two positions
struct Span {
    Pos p1;
    Pos p2;
};

/// @brief Generic AST node with value and source location
/// @tparam T Type of the contained node
template <typename T>
struct Node {
    T value;
    Span s;
};
