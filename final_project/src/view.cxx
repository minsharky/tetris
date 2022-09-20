#include "view.hxx"

using Color = ge211::Color;

static int const grid_size = 30;
static Color const grid_color{0,0,0};
static Color const i_color{106,159,200};
static Color const j_color{17,108,164};
static Color const l_color{196,77, 8};
static Color const t_color{174,122,241};
static Color const s_color{134,203,85};
static Color const o_color{246,174,45};
static Color const z_color{197,40,61};

View::View(Model const& model)
        : model_(model),
          grid_sprite(Dimensions(grid_size - 2, grid_size - 2), grid_color),

          i_sprite(Dimensions(grid_size - 2, grid_size - 2), i_color),
          j_sprite(Dimensions(grid_size - 2, grid_size - 2), j_color),
          l_sprite(Dimensions(grid_size - 2, grid_size - 2), l_color),
          t_sprite(Dimensions(grid_size - 2, grid_size - 2), t_color),
          s_sprite(Dimensions(grid_size - 2, grid_size - 2), s_color),
          o_sprite(Dimensions(grid_size - 2, grid_size - 2), o_color),
          z_sprite(Dimensions(grid_size - 2, grid_size - 2), z_color),

          hold_box(Dimensions(grid_size * 4, grid_size * 4), grid_color)
{}

void
View::draw(ge211::Sprite_set& set)
{
    for (ge211::Posn<int> pos: model_.board()) {
        set.add_sprite(grid_sprite, board_to_screen(pos.into<float>()).into<int>());
    }

    set.add_sprite(hold_box, ge211::Posn<int>(grid_size * model_.get_board_width() + 15,60));
    for (Position pos: model_.get_hold_block().get_block_positions()) {
        add_block_sprite(set, model_.get_hold_block().get_sprite_num(),
                         Position(board_to_screen(pos).x + grid_size * model_.get_board_width() + 15, board_to_screen(pos).y + 60), 1);
    }

    ge211::Font sans30 {"sans.ttf", 24};
    ge211::Text_sprite::Builder score_builder(sans30);

    score_builder << "Score: " << model_.get_score();
    score_sprite.reconfigure(score_builder);
    set.add_sprite(score_sprite, ge211::Posn<int>(grid_size * model_
            .get_board_width() + 18,10));

    for (Block block: model_.get_grid_blocks()) {
        for (Position pos: block.get_block_positions()) {
            add_block_sprite(set, block.get_sprite_num(), board_to_screen
                    (Position {pos.x + 3,pos.y - 1}), 1);
        }
    }
}

View::Dimensions
View::initial_window_dimensions() const
{
    return grid_size * model_.board().dimensions() + Dimensions(150,0);
}

std::string
View::initial_window_title() const
{
    return "Tetris";
}

View::Position
View::board_to_screen(Model::Position logical) const {
    return Position(logical.x * grid_size + 1, logical.y * grid_size + 1);
}

void
View::add_block_sprite(ge211::Sprite_set& set, int num, Position pos, int
z) {
    if (num == 0) {
        set.add_sprite(i_sprite, pos.into<int>(), z);
    }
    else if (num == 1) {
        set.add_sprite(j_sprite, pos.into<int>(), z);
    }
    else if (num == 2) {
        set.add_sprite(l_sprite, pos.into<int>(), z);
    }
    else if (num == 3) {
        set.add_sprite(t_sprite, pos.into<int>(), z);
    }
    else if (num == 4) {
        set.add_sprite(s_sprite, pos.into<int>(), z);
    }
    else if (num == 5) {
        set.add_sprite(o_sprite, pos.into<int>(), z);
    }
    else if (num == 6) {
        set.add_sprite(z_sprite, pos.into<int>(), z);
    }
}