#ifndef PLAYERSTATUSBAR_H
#define PLAYERSTATUSBAR_H

#include <godot_cpp/classes/canvas_layer.hpp>

namespace godot {

class PlayerStatusBar : public CanvasLayer {
    GDCLASS(PlayerStatusBar, CanvasLayer);

private:

protected:
    static void _bind_methods();

public:
    PlayerStatusBar();
    ~PlayerStatusBar();

    void _ready() override;

    void refresh_player_status();
};

}

#endif