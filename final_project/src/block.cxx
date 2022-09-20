#include "block.hxx"

Block::Block(std::vector<Position> position, int num)
        : live(true),
          velocity(Velocity({0, 7.5})),
          starting_positions(position),
          block_positions(position),
          sprite_num(num)
{};

Block::Block()
        : live(false),
          velocity(),
          starting_positions(),
          block_positions(),
          sprite_num()
{};

Block
Block::next(double dt)
{
    Block result(*this);
    for (Position& pos: result.block_positions) {
        pos.y += velocity.height * dt;
    }
    return result;
}

void
Block::set_dead()
{
    live = false;
    velocity = {0,0};
}

bool
Block::hits_bottom() {
    for (Position pos: block_positions) {
        if (pos.y > 19.9) {
            velocity.height =0.5;
        }
        if (pos.y >= 20) {
            for(Position &pos2: block_positions){
                pos2.y = (int)pos2.y;
            }
            return true;
        }
    }
    return false;
}

bool
Block::intersect_vertical(std::vector<Block> blocks) {
    for (Position pos: block_positions) {
        for (Block block: blocks) {
            for (Position p2: block.block_positions) {
                if ((pos.y + 1 >= p2.y) &&(pos.y <= p2.y) && (pos.x == p2.x)) {
                    for (int i = 0; i < 4; i++) {
                        block_positions[i].y = (int) block_positions[i].y;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<Position>
Block::get_lowest_positions() {
    std::vector<Position> lowest;
    lowest.push_back(block_positions[0]);
    for (Position pos: block_positions) {
        if (pos.y > lowest[0].y) {
            lowest.clear();
            lowest.push_back(pos);
        }

        else if (pos.y == lowest[0].y) {
            lowest.push_back(pos);
        }
    }
    return lowest;
}

bool
Block::intersect_left(std::vector<Block> blocks) {
    for (Position pos: block_positions) {
        for (Block block: blocks) {
            for (Position p2: block.block_positions) {
                if (pos.x - 1 == p2.x && (pos.y < p2.y + 1) && (pos.y > p2.y - 1)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool
Block::intersect_right(std::vector<Block> blocks) {
    for (Position pos: block_positions) {
        for (Block block: blocks) {
            for (Position p2: block.block_positions) {
                if (pos.x == p2.x - 1 && (pos.y < p2.y + 1) && (pos.y > p2.y - 1)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void
Block::move_left(std::vector<Block> blocks)
{
    Position leftest = block_positions[0];
    for (Position pos: block_positions) {
        if (pos.x < leftest.x) {
            leftest = pos;
        }
    }

    // Move left only if doing so would not go past the left side of the
    // screen or intersect a block on the left side.
    if (leftest.x >= -2 && !intersect_left(blocks)) {
        for (Position& pos: block_positions) {
            pos.x -= 1;
        }
    }
}

void
Block::move_right(std::vector<Block> blocks)
{
    Position rightest = block_positions[0];
    for (Position pos: block_positions) {
        if (pos.x > rightest.x) {
            rightest = pos;
        }
    }

    // Move right only if doing so would not go past the right side of the
    // screen or intersect a block on the right side.
    if (rightest.x <= 5 && !intersect_right(blocks)) {
        for (Position& pos: block_positions) {
            pos.x += 1;
        }
    }
}

void
Block::rotate()
{
    for(Position& start_pos: starting_positions) {
        if (start_pos == Position(0, 0)) {
            start_pos = Position(0, 3);
        } else if (start_pos == Position(0, 3)) {
            start_pos = Position(3, 3);
        } else if (start_pos == Position(3, 3)) {
            start_pos = Position(3, 0);
        } else if (start_pos == Position(3, 0)) {
            start_pos = Position(0, 0);
        } else if (start_pos == Position(0, 1)) {
            start_pos = Position(2, 0);
        } else if (start_pos == Position(2, 0)) {
            start_pos = Position(3, 2);
        } else if (start_pos == Position(3, 2)) {
            start_pos = Position(1, 3);
        } else if (start_pos == Position(1, 3)) {
            start_pos = Position(0, 1);
        } else if (start_pos == Position(0, 2)) {
            start_pos = Position(1, 0);
        } else if (start_pos == Position(1, 0)) {
            start_pos = Position(3, 1);
        } else if (start_pos == Position(3, 1)) {
            start_pos = Position(2, 3);
        } else if (start_pos == Position(2, 3)) {
            start_pos = Position(0, 2);
        } else if (start_pos == Position(1, 1)) {
            start_pos = Position(2, 1);
        } else if (start_pos == Position(2, 1)) {
            start_pos = Position(2, 2);
        } else if (start_pos == Position(2, 2)) {
            start_pos = Position(1, 2);
        } else if (start_pos == Position(1, 2)) {
            start_pos = Position(1, 1);
        }
    }
    Position main = block_positions[0];
    int counter = 0;
    for(Position& actual_pos: block_positions){
        actual_pos.x = starting_positions[counter].x;
        actual_pos.y = starting_positions[counter].y + main.y - 3;
        counter++;
    }
}

void
Block::speed_down()
{
    velocity.height = 15;
}


void
Block::delete_pos_from_block(Position pos)
{
    std::vector<Position> keep;
    for (Position block_pos: block_positions) {
        if (block_pos != pos) {
            keep.push_back(block_pos);
        }
    }
    block_positions = keep;
}

void
Block::set_block_positions(std::vector<Position> pos)
{
    block_positions.clear();
    for (Position p: pos) {
        block_positions.push_back(p);
    }
}

void
Block::shift_block_up() {
    for (Position& pos: block_positions) {
        pos.y -= 1;
    }
}

bool
operator== (Block const& lhs, Block const& rhs) {
    // If one of the blocks is dead, but the other is not, return false.
    if (lhs.get_live() != rhs.get_live()) {
        return false;
    } else if (lhs.get_block_positions().size() == 0 && rhs.get_block_positions().size() == 0){
        // If they are both empty (i.e. dead), return true;
        return true;
    } else {
        // If every position in the blocks are the same, return true <3
        for (int i = 0; i < 4; i++){
            if (lhs.get_block_positions()[i] != rhs.get_block_positions()[i]) {
                return false;
            }
        }
    }
    return true;
}

bool
operator!=(Block const& lhs, Block const& rhs) {
    return !(lhs == rhs);
}

std::ostream&
operator<<(std::ostream& o, Block const& block) {
    int counter = 1;
    for (Position pos: block.get_block_positions()) {
        o << "position " << counter << ": x: " << pos.x << ", y: " <<
          pos.y << "\n";
        counter++;
    }
    o << "sprite num: " << block.get_sprite_num() << "\n";
    return o;
}