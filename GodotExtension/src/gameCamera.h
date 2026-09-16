#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class GameCamera : public Camera2D {
    GDCLASS(GameCamera, Camera2D)

private:
	double strength;
    double recoverySpeed;
    Vector2 cameraStartPosition;

protected:
    static void _bind_methods();

public:
    GameCamera();
    ~GameCamera();

    void _ready() override;
    void _process(double delta) override;

    void camera_shake_small();
    void camera_shake_big();
    void camera_shake_verybig();

    void start_timer(double time_scale);
    void _on_timer_timeout();
    void player_hurt();
    void room_cleared();
};

}

#endif