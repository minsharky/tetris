#include "board.hxx"

using namespace ge211;

Board::Board(Dimensions dims)
        : dims_(dims)
{}

Board::Dimensions Board::dimensions() const
{
    return dims_;
}

Board::Rectangle Board::all_positions() const
{
    return Rectangle::from_top_left(the_origin, dims_);
}