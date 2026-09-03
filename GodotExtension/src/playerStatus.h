#ifndef PLAYERSTATUS_H
#define PLAYERSTATUS_H

#include <godot_cpp/classes/canvas_layer.hpp>

namespace godot {

class PlayerStatus : public CanvasLayer {
    GDCLASS(PlayerStatus, CanvasLayer);

private:

protected:
    static void _bind_methods();

public:
    PlayerStatus();
    ~PlayerStatus();

    int playerStatusValue = 5;

    void _ready() override;

    void refresh_player_status();
};

}

#endif