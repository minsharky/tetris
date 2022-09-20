#pragma once

#include <ge211.hxx>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Board {
public:
    using Dimensions = ge211::Dims<int>;
    using Position = ge211::Posn<float>;
    using Rectangle = ge211::Rect<int>;

private:
    Dimensions dims_;

public:
    explicit Board(Dimensions dims);

    Dimensions dimensions() const;

    Rectangle all_positions() const;
};

