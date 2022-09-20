#pragma once

#include "model.hxx"

class View
{
public:
    using Dimensions = ge211::Dims<int>;
    using Position = ge211::Posn<float>;
    using Rectangle = ge211::Rect<float>;

    explicit View(Model const& model);

    void draw(ge211::Sprite_set& set);

    Dimensions initial_window_dimensions() const;

    std::string initial_window_title() const;

    View::Position board_to_screen(Model::Position logical) const;

    void add_block_sprite(ge211::Sprite_set& set, int num, Position pos,
                          int z);

private:
    Model const& model_;
    ge211::Rectangle_sprite const grid_sprite;
    ge211::Text_sprite score_sprite;
    ge211:: Rectangle_sprite const i_sprite;
    ge211:: Rectangle_sprite const j_sprite;
    ge211:: Rectangle_sprite const l_sprite;
    ge211:: Rectangle_sprite const t_sprite;
    ge211:: Rectangle_sprite const s_sprite;
    ge211:: Rectangle_sprite const o_sprite;
    ge211:: Rectangle_sprite const z_sprite;

    ge211:: Rectangle_sprite const hold_box;
};
