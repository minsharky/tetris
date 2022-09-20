#include "model.hxx"
#include "block.hxx"
#include <catch.hxx>

using namespace ge211;

struct Test_access
{
    Model& model;
    Board& board();
    explicit Test_access(Model&);
    Block& current_block();
    Block& hold_block();
    Block& i_block();
    Block& o_block();
    std::vector<Block>& grid_blocks();
    bool game_over();
    int score();
};

TEST_CASE("WHOLE GAME COMPLETION & HARD DROP") {
    Model m(10, 20);
    Test_access access(m);
    double const dt = 0.4;
    CHECK(access.current_block() == Block());
    m.block_source.stub_with(5);
    m.on_frame(dt);
    CHECK(access.current_block() == access.o_block());
    CHECK(access.current_block().get_live());
    CHECK(!access.game_over());
    m.on_frame(dt);
    // Checks to see if the block moved 24 frames (3 grid positions, 90px)
    // Specification 3: block moves down at 7.5 grids / sec
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{1,4} || pos == Position{1,5} || pos ==Position{2,4} || pos == Position{2,5});
        CHECK(check);
    }
    m.hard_drop();

    bool hard = false;
    for (Position pos: access.current_block().get_block_positions()) {
        if (pos.y == 20) {
            hard = true;
        }
    }
    CHECK(hard);

    // Must call on_frame(0) twice after hard dropping a block
    // 1st on_frame(0) makes the current_block dead and pushes it into grid_blocks
    //     remember that because dt == 0, current_block will not move to the next block
    // 2nd on_frame(0) makes current_block be reset to the starting o_block and sets it alive again at starting positions

    m.on_frame(0);
    CHECK(!access.current_block().get_live());

    m.on_frame(0);
    CHECK(access.current_block() == access.o_block());
    CHECK(access.current_block().get_live());

    // Checks that it drops on top of the previous o block.
    m.hard_drop();

    m.on_frame(0);
    bool harder = false;
    for (Position pos: access.current_block().get_block_positions()) {
        if (pos.y == 18) {
            harder = true;
        }
    }
    CHECK(harder);

    m.on_frame(0);

    // After dropping the o_block 8 more times, the 8th o_block will hit the top of the grid and call game_over
    for(int i = 0; i < 8; i++){
        m.hard_drop();

        // 1st on_frame(0) makes the current_block dead and pushes it into grid_blocks
        m.on_frame(0);
        // 2nd on_frame(0) makes current_block be reset to the starting o_block and sets it alive again at starting positions
        m.on_frame(0);
    }

    // Checks that there is a block at the top.
    bool at_top = false;
    for (Block block: access.grid_blocks()) {
        for (Position pos: block.get_block_positions()) {
            if (pos.y <= 1) {
                at_top = true;
            }
        }
    }
    CHECK(at_top);
    CHECK(access.game_over());
}

TEST_CASE("ROTATE AND CLEAR") {
    Model m(10, 20);
    Test_access access(m);
    double const dt = 0.4;
    CHECK(access.current_block() == Block());
    m.block_source.stub_with(0);
    m.on_frame(dt);
    CHECK(access.current_block() == access.i_block());
    CHECK(access.current_block().get_live());
    CHECK(!access.game_over());

    m.on_frame(0);

    // move current_block to the left twice and drop it to the ground
    m.move_left();
    m.move_left();
    m.hard_drop();
    m.on_frame(0);

    // check that there is an i_block at Pos(-2, 20) Pos(-1, 20) Pos(0, 20) Pos(1, 20)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{-2,20} || pos == Position{-1,20} || pos ==Position{0,20} || pos == Position{1,20});
        CHECK(check);
    }

    m.on_frame(0);

    // generate four more i_blocks and stack them on top of the first i_block
    for(int i = 0; i < 4; i++){
        m.move_left();
        m.move_left();
        m.hard_drop();
        m.on_frame(0);
        m.on_frame(0);
    }

    // generate an i_block and move it to the right twice and drop it on the ground
    m.move_right();
    m.move_right();
    m.hard_drop();
    m.on_frame(0);

    // check that there is an i_block at Pos(2, 20) Pos(3, 20) Pos(4, 20) Pos(5, 20)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{2,20} || pos == Position{3,20} || pos ==Position{4,20} || pos == Position{5,20});
        CHECK(check);
    }

    m.on_frame(0);

    // generate 4 i_blocks and stack it on top of the i_block on the right
    for(int i = 0; i < 4; i++){
        m.move_right();
        m.move_right();
        m.hard_drop();
        m.on_frame(0);
        m.on_frame(0);
    }
    m.on_frame(0.4);

    //rotate i_block
    m.rotate();

    // std::vector<Position> i_block_pos starting positions = {{0,1}, {1,1}, {2,1}, {3,1}};
    // std::vector<Position> i_block_pos  after rotating = {{2,0}, {2,1}, {2,2}, {2,3}};
    // Position main = block_positions[0] = position 1: x: 0, y: 4
    // actual_pos.x = starting_positions[counter].x;
    // therefore, all x positions in current_block == 2

    // actual_pos.y = starting_positions[counter].y + main.y -3;
    // {{2,0 + 4 -3}, {2,1 + 4 -3}, {2,2 + 4 -3}, {2,3 + 4 -3}}

    // resulting new current_block positions{{2,1}, {2,2}, {2,3}, {2,4}}
    // check that the i_block positions are equal to  Pos(2, 1) Pos(2, 2) Pos(2, 3) Pos(2, 4)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{2,1} || pos == Position{2,2} || pos ==Position{2,3} || pos == Position{2,4});
        CHECK(check);
    }

    //move the rotated block to the right 4 times
    m.move_right();
    m.move_right();
    m.move_right();
    m.move_right();

    // check that the i_block positions are equal to  Pos(6, 1) Pos(6, 2) Pos(6, 3) Pos(6, 4)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{6,1} || pos == Position{6,2} || pos ==Position{6,3} || pos == Position{6,4});
        CHECK(check);
    }

    m.hard_drop();
    m.on_frame(0);

    // check that the i_block positions are equal to  Pos(20, 0) Pos(20, 1) Pos(20, 2) Pos(20, 3)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{6,17} || pos == Position{6,18} || pos ==Position{6,19} || pos == Position{6,20});
        CHECK(check);
    }
    m.on_frame(0);
    m.on_frame(dt);

    // i_block positions are equal to  Pos(2, 1) Pos(2, 2) Pos(2, 3) Pos(2, 4) after one rotate
    m.rotate();

    m.rotate();
    // std::vector<Position> i_block_pos  starting positions = {{2,0}, {2,1}, {2,2}, {2,3}};
    // std::vector<Position> i_block_pos after rotating = {{3,2}, {2,2}, {1,2}, {0,2}};
    // Position main = block_positions[0] = position 1: x: 2, y: 1
    // actual_pos.x = starting_positions[counter].x;
    // {{3,}, {2,}, {1,}, {0,}}

    // actual_pos.y = starting_positions[counter].y + main.y -3;
    // {{3,2 + 1 -3}, {2, 2 + 1 -3}, {1,2 + 1 -3}, {0,2 + 1 -3}}
    // resulting new current_block positions{{3,0}, {2,0}, {1,0}, {0,0}}
    // check that the i_block positions are equal to  Pos(2, 1) Pos(2, 2) Pos(2, 3) Pos(2, 4)

    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{3,0} || pos == Position{2,0} || pos ==Position{1,0} || pos == Position{0,0});
        CHECK(check);
    }

    m.rotate();
    // std::vector<Position> i_block_pos  starting positions =  {{3,2}, {2,2}, {1,2}, {0,2}};
    // std::vector<Position> i_block_pos after rotating = {{1,3}, {1,2}, {1,1}, {1,0}};
    // Position main = block_positions[0] = position 1: x: 3, y: 0
    // actual_pos.x = starting_positions[counter].x;
    // {{1,}, {1,}, {1,}, {1,}}

    // actual_pos.y = starting_positions[counter].y + main.y -3;
    // {{1,3 -3}, {1,2 -3}, {1,1 -3}, {1,0-3}}
    // resulting new current_block positions{{1,0}, {1,-1}, {1,-2}, {1,-3}}
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{1,0} || pos == Position{1,-1} || pos ==
                                                                       Position{1,-2} || pos == Position{1,-3});
        CHECK(check);
    }

    // move the rotated block to the left 4 times
    m.move_left();
    m.move_left();
    m.move_left();
    m.move_left();
    // check that the i_block positions are equal to  Pos(0, 0) Pos(0, -1) Pos(0, -2) Pos(0, -3)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{-3,0} || pos == Position{-3,-1} || pos
                                                                         == Position{-3,-2} || pos == Position{-3,-3});
        CHECK(check);
    }


    m.hard_drop();
    m.on_frame(0);
    //  check that the i_block positions are equal to  Pos(-3, 20) Pos(-3, 19) Pos(-3, 18) Pos(-3, 17)
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{-3,20} || pos == Position{-3,19} || pos== Position{-3,18} || pos == Position{-3,17});
        CHECK(check);
    }
    m.on_frame(0);
    m.on_frame(0);
    access.grid_blocks().pop_back();

     //check that the rows are cleared by going through every position on the grid and checking to see that each positions is on the ground
        for (Block blocks: access.grid_blocks()) {
                for(Position pos: blocks.get_block_positions()){
                    bool check = (pos.y == 20);
                    CHECK(check);
                }
        }

    //check that the rows are cleared
    CHECK(access.score() == 4);

}

TEST_CASE("CLEARING & SCORE") {
    Model m(10, 20);
    Test_access access(m);
    m.block_source.stub_with(0);
    // Sets current block to i block.
    m.on_frame(0);
    // Moves current block
    m.on_frame(0.13333333);
    bool p;
    for (Position pos: access.current_block().get_block_positions()) {
        p = (pos == Position{0,2} || pos == Position{1,2} || pos ==
                                                             Position{2,2} || pos == Position{3,2});
    }
    CHECK(p);
    for (int i = 0; i < 3; i++) {
        m.move_left();
    }
    m.on_frame(2.4);
    bool b1;
    for (Position pos: access.current_block().get_block_positions()) {
        b1 = (pos == Position{-3,20} || pos == Position{-2,20} ||
              pos == Position{-1,20} || pos == Position{0,20});
        CHECK(b1);
    }

    // Sets current block to dead.
    m.on_frame(0);
    // Generates new i block at top.
    m.on_frame(0);
    for (int i = 0; i < 3; i++) {
        m.move_left();
    }
    m.hard_drop();
    // Sets current block to dead.
    m.on_frame(0);
    // Generates new i-block at top.
    m.on_frame(0);

    // Drops two more i-blocks on the left side.
    for (int count = 0; count < 2; count++) {
        for (int left = 0; left < 3; left++) {
            m.move_left();
        }
        m.hard_drop();
        m.on_frame(0);
        m.on_frame(0);
    }

    // Drops two i-blocks 1 over to the right.
    for (int c2 = 0; c2 < 2; c2++) {
        m.move_right();
        m.hard_drop();
        m.on_frame(0);
        m.on_frame(0);
    }

    // Drops another i-block 1 over to the right.
    m.move_right();
    m.hard_drop();
    m.on_frame(0);

    // Changes to o-block.
    m.block_source.stub_with(5);

    // Generates o-block at top.
    m.on_frame(0);

    // Moves block to right edge
    for (int i = 0; i < 5; i++) {
        m.move_right();
    }

    for (Position pos: access.current_block().get_block_positions()) {
        bool same = (pos == Position{5, 1} || pos == Position{6, 1}||
                     pos == Position{5, 2} || pos == Position{6, 2});
        CHECK(same);
    }

    // Trying to move past right edge does not do anything.
    m.move_right();

    for (Position pos: access.current_block().get_block_positions()) {
        bool same2 = (pos == Position{5, 1} || pos == Position{6, 1}||
                      pos == Position{5, 2} || pos == Position{6, 2});
        CHECK(same2);
    }

    m.hard_drop();
    m.on_frame(0);
    m.on_frame(0);

    CHECK(access.score() == 2);
}

TEST_CASE("HOLD BLOCK") {
    Model m(10,20);
    Test_access access(m);
    m.block_source.stub_with({0, 5});

    // Generate i-block.
    m.on_frame(0);

    m.on_frame(0.4);

    CHECK(access.hold_block() == Block());
    m.change_hold_block();

    // Checks that when hold block is empty, current block is set to a new
    // random block (o-block in this case since random is stubbed)
    CHECK(access.hold_block() == access.i_block());
    CHECK(access.current_block() == access.o_block());

    // Checks that hold block and current block switch.
    m.change_hold_block();
    CHECK(access.hold_block() == access.o_block());
    CHECK(access.current_block() == access.i_block());

    // Checks that rotating will change the orientation of the block when it
    // is un-held by changing the starting positions of that block.
    m.on_frame(0.4);
    m.rotate();
    m.change_hold_block();
    //std::cout << access.hold_block() << "\n";
    for (Position pos: access.hold_block().get_starting_positions()) {
        bool hold = (pos == Position{2,0} || pos == Position{2,1} || pos ==
                                                                     Position{2,2} || pos == Position{2,3});
        CHECK(hold);
    }
    CHECK(access.current_block() == access.o_block());

    // Checks that it originates at the proper starting position when
    // previously rotated.
    m.change_hold_block();
    for (Position pos: access.current_block().get_block_positions()) {
        bool start = (pos == Position{2,0} || pos == Position{2,1} || pos ==
                                                                      Position{2,2} || pos == Position{2,3});
        CHECK(start);
    }
    CHECK(access.hold_block() == access.o_block());
}

TEST_CASE("SPEED_DOWN AND GRID_BLOCKS") {
    Model m(10, 20);
    Test_access access(m);
    double const dt = 0.4;
    CHECK(access.current_block() == Block());
    m.block_source.stub_with(0);
    m.on_frame(dt);
    CHECK(access.current_block() == access.i_block());
    CHECK(access.current_block().get_live());
    CHECK(!access.game_over());

    //check that the current_block's initial velocity is 7.5
    CHECK(access.current_block().get_velocity() == 7.5);
    m.speed_down();
    //check that the current_block's new velocity is 15
    CHECK(access.current_block().get_velocity() == 15);

    //drop the current_block to the ground
    m.hard_drop();
    m.on_frame(0);
    //check that the block is on the ground
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{0,20} || pos == Position{1,20} || pos == Position{2,20} || pos == Position{3,20});
        CHECK(check);
    }
    m.on_frame(0);

    CHECK(access.current_block().get_velocity() == 7.5);
    m.speed_down();
    CHECK(access.current_block().get_velocity() == 15);

    //move the new current_block down to where it rests on top of the first block
    m.on_frame(1.25);

    //check that the new block is on top of the last block
    for (Position pos: access.current_block().get_block_positions()) {
        bool check = (pos == Position{0,19} || pos == Position{1,19} || pos == Position{2,19} || pos == Position{3,19});
        CHECK(check);
    }
    m.on_frame(0);
    access.grid_blocks().pop_back();

    //check that every block in the grid is dead
    for (Block blocks: access.grid_blocks()) {
        bool check = !blocks.get_live();
        CHECK(check);
    }

}

Test_access::Test_access(Model& model)
        : model(model)
{ }

Board&
Test_access::board() {
    return model.board_;
}

Block&
Test_access::current_block()
{
    return model.current_block;
}

Block&
Test_access::hold_block()
{
    return model.hold_block;
}

Block&
Test_access::i_block() {
    return model.i_block;
}

Block&
Test_access::o_block() {
    return model.o_block;
}

std::vector<Block>&
Test_access:: grid_blocks() {
    return model.grid_blocks;
}

bool
Test_access::game_over()
{
    return model.game_over;
}

int
Test_access::score() {
    return model.score;
}