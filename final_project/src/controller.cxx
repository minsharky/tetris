#include "controller.hxx"

using Color = ge211::Color;

Controller::Controller(int width, int height)
        : model_(width, height),
          view_(model_)
{}

void
Controller::draw(ge211::Sprite_set& set)
{
    background_color = Color{69,69,69};
    view_.draw(set);
}

View::Dimensions
Controller::initial_window_dimensions() const
{
    return view_.initial_window_dimensions();
}

std::string
Controller::initial_window_title() const
{
    return view_.initial_window_title();
}

void
Controller::on_frame(double dt) {
    model_.on_frame(dt);
}

void
Controller::on_key(ge211::Key key) {

    if (key == ge211::Key::code('q')) {
        quit();
    }

    if (key == ge211::Key::left()) {
        model_.move_left();
    }

    if (key == ge211::Key::right()) {
        model_.move_right();
    }

    if (key == ge211::Key::up()) {
        model_.rotate();
    }

    if (key == ge211::Key::code(' ')) {
        model_.hard_drop();
    }

    if (key == ge211::Key::shift()) {
        model_.change_hold_block();
    }
}

void
Controller::on_key_down(ge211::Key key)
{
    if (key == ge211::Key::down()) {
        model_.speed_down();
    }
}

void
Controller::on_key_up(ge211::Key key)
{
    if (key == ge211::Key::down()) {
        model_.reset_velocity();
    }
}