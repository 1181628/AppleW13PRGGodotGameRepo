#ifndef PAUSE_SCREEN_H
#define PAUSE_SCREEN_H

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>

namespace godot {

class PauseScreen : public CanvasLayer {
    GDCLASS(PauseScreen, CanvasLayer);

private:
    void pause_game();
    void resume_game();
    void on_main_menu_pressed();
    void on_quit_game_pressed();

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _unhandled_input(const Ref<InputEvent> &event) override;
};

}

#endif