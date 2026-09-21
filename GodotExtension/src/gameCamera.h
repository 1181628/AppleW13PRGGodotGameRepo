#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

class GameCamera : public Camera2D {
    GDCLASS(GameCamera, Camera2D)

private:
    // Store the camera's starting position and shake settings
    Vector2 cameraStartPosition;
    double strength = 0.0;
    double recoverySpeed = 20.0;

protected:
    static void _bind_methods();

public:
    GameCamera();
    ~GameCamera();

    // Set up the camera and update following and shaking
    void _ready() override;
    void _process(double delta) override;

    // Apply different shake strengths
    void camera_shake_small();
    void camera_shake_big();

    // Trigger effects for gameplay events
    void player_hurt();
    void room_cleared();

    // Temporarily change game speed and restore it afterwards
    void start_timer(double time_scale, double duration = 0.1);
    void _on_timer_timeout();
};

}

#endif