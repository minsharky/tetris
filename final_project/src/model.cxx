#include "model.hxx"

using namespace ge211;

Model::Model(int width, int height)
        : block_source(7),
          score(0),
          game_over(false),
          board_({width, height}),
          i_block(Block(i_block_pos, 0)),
          j_block(Block(j_block_pos, 1)),
          l_block(Block(l_block_pos, 2)),
          t_block(Block(t_block_pos, 3)),
          s_block(Block(s_block_pos, 4)),
          o_block(Block(o_block_pos, 5)),
          z_block(Block(z_block_pos, 6)),
          current_block(Block()),
          hold_block(Block())
{
    blocks.push_back(i_block);
    blocks.push_back(j_block);
    blocks.push_back(l_block);
    blocks.push_back(t_block);
    blocks.push_back(s_block);
    blocks.push_back(o_block);
    blocks.push_back(z_block);
}

Block
Model::random_block() {
    return blocks[block_source.next()];
}

bool
Model::is_game_over() {
    // Checks every position of every block in grid_blocks to see if the y
    // value is <= 1. If true, the game is over.
    for (Block block: grid_blocks) {
        for (Position pos: block.get_block_positions()) {
            if (pos.y <= 1) {
                game_over = true;
                return true;
            }
        }
    }
    return false;
}

void
Model::on_frame(double dt)
{
    // Only runs if game is not over.
    if (!game_over) {
        if (current_block.get_live()) {
            // If the current block is live, move it to its next position.
            current_block = current_block.next(dt);
            // Set block to dead if it hits the bottom or is about to
            // intersect another block.
            grid_blocks.pop_back();
            if (current_block.hits_bottom() || current_block.intersect_vertical(grid_blocks)) {
                current_block.set_dead();
            }
            // Take out the previous current block and add in the new one.
            //grid_blocks.pop_back();
            grid_blocks.push_back(current_block);

        } else {
            // Clear and shift down necessary blocks.
            shift_blocks_down(clear());
            // Set current_block to a random one.
            current_block = random_block();
            // If moving would intersect another block, set the block to dead.
            if (current_block.intersect_vertical(grid_blocks)) {
                current_block.set_dead();
            }
            if (is_game_over()) {
                // If game is over, change current block back to previous one
                // i.e. the one at the top of the screen.
                current_block = grid_blocks[grid_blocks.size() - 1];
                // If any position of the current block is occupied by
                // another block, move every position up by 1 so that they
                // are no longer intersecting.
                if (current_block.get_lowest_positions()[0].y >=
                    grid_highest_position()) {
                    current_block.shift_block_up();
                }
                grid_blocks.pop_back();
            }
            grid_blocks.push_back(current_block);
        }
    }
}

void
Model::speed_down()
{
    current_block.speed_down();
}

void
Model::reset_velocity()
{
    current_block.reset_velocity();
}

float Model::grid_highest_position(){
    std::vector<Position> positions_in_path;
    grid_blocks.pop_back();
    // Gets a vector of positions that have the same x value as the lowest
    // positions of the current block.
    for (Block block: grid_blocks) {
        for (Position pos: block.get_block_positions()) {
            for (int i = 0; i < current_block.get_lowest_positions().size(); i++) {
                if (pos.x == current_block.get_lowest_positions()[i].x) {
                    positions_in_path.push_back(pos);
                }
            }
        }
    }
    grid_blocks.push_back(current_block);

    // Out of those positions, gets the position with the lowest y value and
    // returns its y value.
    Position highest_position = Position{0,21};
    for(Position pos: positions_in_path) {
        if (pos.y < highest_position.y) {
            highest_position = pos;
        }
    }

    return highest_position.y;
}

void
Model::hard_drop()
{
    // Sets velocity to the difference between the highest grid block position
    // and lowest current block position to drop in 1 frame.
    float dist = grid_highest_position() - current_block.get_lowest_positions()[0].y;
    std::vector<Position> positions;
    for(Position pos: current_block.get_block_positions()){
        positions.push_back(Position(pos.x, pos.y + (dist -1)));
    }
    current_block.set_block_positions(positions);
}

void
Model::change_hold_block() {
    grid_blocks.pop_back();
    // If hold block is not yet occupied, change it to the current block then
    // randomly generate a new current block.
    if (hold_block == Block()) {
        hold_block = Block(current_block.get_starting_positions(), current_block.get_sprite_num());
        current_block = random_block();
    }
    else {
        // Switch the hold block and current block. The current block
        // generates at its starting positions.
        Block temp = hold_block;
        hold_block = Block(current_block.get_starting_positions(), current_block.get_sprite_num());
        current_block = temp;
    }
    grid_blocks.push_back(current_block);
}

void
Model::rotate()
{
    current_block.rotate();
}

void
Model::move_left()
{
    if (current_block.get_live()) {
        // Take current block out of grid_blocks so that it does not check
        // against its own positions when determining whether it can move.
        grid_blocks.pop_back();
        current_block.move_left(grid_blocks);
        grid_blocks.push_back(current_block);
    }
}

void
Model::move_right()
{
    if (current_block.get_live()) {
        // Take current block out of grid_blocks so that it does not check
        // against its own positions when determining whether it can move.
        grid_blocks.pop_back();
        current_block.move_right(grid_blocks);
        grid_blocks.push_back(current_block);
    }
}

std::vector<Position>
Model::get_occupied_positions() {
    std::vector<Position> occupied_positions;
    for (Block block: grid_blocks) {
        if (!block.get_live()) {
            for (Position pos: block.get_block_positions()) {
                occupied_positions.push_back(pos);
            }
        }
    }
    return occupied_positions;
}

std::vector<int>
Model::clear()
{
    std::vector<int> row_counter(20);
    std::vector<int> rows_to_clear;

    // Sets each row to count of 0.
    for (int i = 0; i < 20; i++) {
        row_counter[i] = 0;
    }

    // Increments 1 in row counter for the y value of each position.
    for (int j = 0; j < get_occupied_positions().size(); j++) {
        row_counter[get_occupied_positions()[j].y - 1]++;
    }

    // If the row value in row counter is 10, add it to rows to clear.
    for (int k = 0; k < row_counter.size(); k++) {
        if (row_counter[k] == 10) {
            rows_to_clear.push_back(k);
        }
    }

    // Checks to see if any of the blocks on the screen is in a row to clear.
    // If so, delete the position from the block.
    std::vector<Block> new_blocks;
    for (Block block: grid_blocks) {
        for (Position pos: block.get_block_positions()) {
            for (int row_y: rows_to_clear) {
                if ((pos.y - 1) == row_y) {
                    block.delete_pos_from_block(pos);
                }
            }
        }
        new_blocks.push_back(block);
    }
    grid_blocks = new_blocks;

    // Add one to score for each cleared row.
    score += rows_to_clear.size();
    return rows_to_clear;
}

void
Model::shift_blocks_down(std::vector<int> cleared_rows) {
    std::vector<Block> new_2;
    std::vector<Position> pos_to_move_down;
    // If the position of a block is above a cleared row, move that position
    // down one for each cleared row that it is above.
    for (Block block: grid_blocks) {
        std::vector<Position> block_pos;
        for (Position pos: block.get_block_positions()) {
            for (int row_y: cleared_rows) {
                if ((pos.y < row_y + 1)) {
                    pos = Position{pos.x, pos.y + 1};
                }
            }
            block_pos.push_back(pos);
        }
        block.set_block_positions(block_pos);
        new_2.push_back(block);
    }
    grid_blocks = new_2;
}

std::ostream&
operator<<(std::ostream& o, std::vector<Block> const& blocks) {
    for (Block block: blocks) {
        o << "block: ";
        for (Position pos: block.get_block_positions()) {
            o << pos << " ";
        }
        o << "\n";
    }
    return o;
}