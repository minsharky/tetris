#pragma once

#include "model.hxx"
#include "view.hxx"

#include <ge211.hxx>

class Controller : public ge211::Abstract_game
{
public:
    Controller(int width = 10, int height = 20);

protected:
    void draw(ge211::Sprite_set& set) override;
    View::Dimensions initial_window_dimensions() const override;
    std::string initial_window_title() const override;
    void on_frame(double dt) override;
    void on_key(ge211::Key key) override;
    void on_key_down(ge211::Key key) override;
    void on_key_up(ge211::Key key) override;

private:
    Model model_;
    View view_;
};
