#include "gameCamera.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <cstdlib>

using namespace godot;

GameCamera::GameCamera() {
}

GameCamera::~GameCamera() {
}

void GameCamera::_bind_methods() {
    ClassDB::bind_method(D_METHOD("room_cleared"), &GameCamera::room_cleared);
}

void GameCamera::_ready() {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Stores the starting position as the centre of the camera's movement range
    cameraStartPosition = get_global_position();
    // Allows other objects to find the camera through its group
    add_to_group("game_camera");
}

void GameCamera::_process(double delta) {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Find the player
    Node2D *player = Object::cast_to<Node2D>(get_tree()->get_first_node_in_group("player"));
    // Stops processing if no valid Player was found.
    if (player == nullptr) {
        return;
    }

    Vector2 cameraPosition = get_global_position();
    Vector2 playerPosition = player->get_global_position();
    float distanceX = player->get_global_position().x - cameraStartPosition.x;

    // Follow slightly, within 10 units of the starting position
    float offsetX = Math::clamp(distanceX * 0.1f, -10.0f, 10.0f);
    float targetX = cameraStartPosition.x + offsetX;
    // Uses delta-based smoothing to keep camera following consistent across frame rates
    float weight = 1.0f - Math::exp(-2.0f * float(delta));

    // Move the camera's horizontal position toward the target
    cameraPosition.x = Math::lerp(cameraPosition.x, targetX, weight);
    // Keep the camera's vertical position fixed at its starting value
    cameraPosition.y = cameraStartPosition.y;

    set_global_position(cameraPosition);

    // Creates a random position between -strength and strength
    double randomX = -strength + (static_cast<double>(rand()) / RAND_MAX) * strength * 2;
    double randomY = -strength + (static_cast<double>(rand()) / RAND_MAX) * strength * 2;
    // Moves the camera by the generated random offset
    set_offset(Vector2(randomX, randomY));
    // Gradually reduces the shake strength until it reaches zero
    strength = Math::move_toward(strength, 0.0, recoverySpeed * delta);
}

// Apply a small camera shake
void GameCamera::camera_shake_small() {
    strength = 1.0;
    recoverySpeed = 20.0;
}

// Apply a stronger camera shake
void GameCamera::camera_shake_big() {
    strength = 3.0;
    recoverySpeed = 20.0;
}

// Briefly slow the game when the player is hurt
void GameCamera::player_hurt() {
    start_timer(0.1);
}

// Apply camera shake and slow motion when a room is cleared
void GameCamera::room_cleared() {
    strength = 10;
    recoverySpeed = 15.0;
    start_timer(0.3, 2.5);
}

// Temporarily change the game's time scale until a timer expires
void GameCamera::start_timer(double time_scale, double duration) {
    // Create a timer to control the duration of the time-scale effect
    Timer *timer = memnew(Timer);
    // Set the timer duration
    timer->set_wait_time(duration);
    // Uses real time so slow motion does not extend its own duration
    timer->set_ignore_time_scale(true);
    // Configure the timer to stop after its first timeout
    timer->set_one_shot(true);
    // Connect the timeout signal to the method that restores normal game speed
    timer->connect("timeout",callable_mp(this, &GameCamera::_on_timer_timeout));
    // Removes the Timer after it finishes
    timer->connect("timeout", Callable(timer, "queue_free"));
    // Add the timer as a child so it can run in the scene tree
    add_child(timer);
    // Apply the requested time scale to the game
    Engine::get_singleton()->set_time_scale(time_scale);

    // Start the timer
    timer->start();
}

// Restore normal game speed when the timer expires
void GameCamera::_on_timer_timeout() {
    Engine::get_singleton()->set_time_scale(1.0);
}