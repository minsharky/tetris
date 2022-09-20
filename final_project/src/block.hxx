#pragma once

#include <ge211.hxx>
#include <iostream>

using Position = ge211::Posn<float>;
using Velocity = ge211::Dims<float>;

struct Block{

public:
    Block();
    Block(std::vector<Position> position, int num);

    bool get_live() const {
        return live;
    }

    // Sets velocity to v.
    void set_velocity(Velocity v) {
        velocity = v;
    }

    float get_velocity() {
        return velocity.height;
    }

    // Sets velocity back to original velocity.
    void reset_velocity() {
        velocity.height = 7.5;
    }

    std::vector<Position> get_starting_positions() const{
        return starting_positions;
    }

    std::vector<Position> get_block_positions() const{
        return block_positions;
    }

    int get_sprite_num() const{
        return sprite_num;
    }

    // Gets the next Block.
    Block next(double dt);

    // Makes block static.
    void set_dead();

    // Determines if block hits bottom of grid.
    bool hits_bottom();

    // Determines if block intersects another block vertically.
    bool intersect_vertical(std::vector<Block> blocks);

    // Determines if block intersects another block on left side.
    bool intersect_left(std::vector<Block> blocks);

    // Moves block 1 to the left, if it does not reach the left border and
    // will not intersect another block.
    void move_left(std::vector<Block> blocks);

    // Determines if block intersects another block on right side.
    bool intersect_right(std::vector<Block> blocks);

    // Moves block 1 to the right, if it does not reach the right border and
    // will not intersect another block.
    void move_right(std::vector<Block> blocks);

    // Rotates the block by 90 degrees.
    void rotate();

    // Sets the velocity of the block to 0.25
    void speed_down();

    // Gets the lowest position of the block.
    std::vector<Position> get_lowest_positions();

    // Deletes a position from block_positions.
    void delete_pos_from_block(Position pos);

    // Sets block_positions to a vector of Positions.
    void set_block_positions(std::vector<Position> pos);

    // Moves every position in a block up by one.
    void shift_block_up();

private:
    bool live;
    Velocity velocity;
    std::vector<Position> starting_positions;
    std::vector<Position> block_positions;
    int sprite_num;
};

bool operator==(Block const&, Block const&);
bool operator!=(Block const&, Block const&);
std::ostream& operator<<(std::ostream& o, Block const&);