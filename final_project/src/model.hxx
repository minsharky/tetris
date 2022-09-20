#pragma once

#include <ge211.hxx>
#include "board.hxx"
#include "block.hxx"
#include <iostream>
#include <vector>

class Model
{
public:

    using Position = Board::Position;

    using Rectangle = Board::Rectangle;

    explicit Model(int width = 10, int height = 20);

    ge211::Random_source<int> block_source;

    Rectangle board() const {
        return board_.all_positions();
    }

    int get_board_width() const {
        return board_.dimensions().width;
    }

    int get_score() const {
        return score;
    }

    Block get_current_block() const {
        return current_block;
    }

    std::vector<Block> get_grid_blocks() const{
        return grid_blocks;
    }

    Block get_hold_block() const{
        return hold_block;
    }

    // Generates random block.
    Block random_block();

    // Sets game_over to true if there is a block at the top of screen.
    bool is_game_over();

    // Runs through one frame.
    void on_frame(double dt);

    // Increases velocity of block.
    void speed_down();

    // Sets velocity of block back to original velocity.
    void reset_velocity();

    // Returns the y-value of the highest block on the screen.
    float grid_highest_position();

    // Auto drops block until it intersects another block or reaches bottom
    // of grid.
    void hard_drop();

    // Changes the block being held.
    void change_hold_block();

    // Rotates block.
    void rotate();

    // Moves the block left, if possible.
    void move_left();

    // Moves the block right, if possible.
    void move_right();

    // Gets all positions occupied by a dead block.
    std::vector<Position> get_occupied_positions();

    // Vector of ints that correspond to the y value of the rows that were
    // cleared.
    std::vector<int> clear();

    // Shifts every block above a cleared row down.
    void shift_blocks_down(std::vector<int> row);

    Block get_current_block(){
        return current_block;
    }

    friend struct Test_access;

private:
    int score;
    bool game_over;
    Board board_;

    std::vector<Position> i_block_pos = {{0,1}, {1,1}, {2,1}, {3,1}};
    std::vector<Position> j_block_pos = {{0,1}, {1,1}, {2,1}, {2,2}};
    std::vector<Position> l_block_pos = {{0,1}, {1,1}, {2,1}, {0,2}};
    std::vector<Position> t_block_pos = {{0,1}, {1,1}, {2,1}, {1,2}};
    std::vector<Position> s_block_pos = {{0,2}, {1,1}, {1,2}, {2,1}};
    std::vector<Position> o_block_pos = {{1,1}, {1,2}, {2,1}, {2,2}};
    std::vector<Position> z_block_pos = {{0,1}, {1,1}, {1,2}, {2,2}};

    Block i_block;
    Block j_block;
    Block l_block;
    Block t_block;
    Block s_block;
    Block o_block;
    Block z_block;

    // 7 different types of blocks.
    std::vector<Block> blocks;

    // Current block in motion.
    Block current_block;

    // Current block held.
    Block hold_block;

    // All blocks on grid, including current.
    std::vector<Block> grid_blocks;
};

std::ostream& operator<<(std::ostream& o, std::vector<Block> const& blocks);