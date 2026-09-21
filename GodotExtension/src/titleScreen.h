#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <godot_cpp/classes/control.hpp>

namespace godot {

class TitleScreen : public Control {
    GDCLASS(TitleScreen, Control);

private:
    // Handles each menu action through a separate button callback
    void on_new_game_pressed();
    void on_load_game_pressed();
    void on_exit_game_pressed();
    void on_past_records_pressed();

protected:
    static void _bind_methods();

public:
    TitleScreen();
    ~TitleScreen();

    void _ready() override;
    void _exit_tree() override;
};

}

#endif
