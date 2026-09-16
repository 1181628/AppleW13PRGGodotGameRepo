#include "gameCamera.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/callable.hpp>

#include <cstdlib>

using namespace godot;

GameCamera::GameCamera() {
}

GameCamera::~GameCamera() {
}

void GameCamera::_bind_methods() {
}

void GameCamera::_ready() {
    cameraStartPosition = get_global_position();
}

void GameCamera::_process(double delta) {
    // Find the player
    Node2D *player = Object::cast_to<Node2D>(get_tree()->get_first_node_in_group("player"));
    Vector2 cameraPosition = get_global_position();
    Vector2 playerPosition = player->get_global_position();
    float distanceX = player->get_global_position().x - cameraStartPosition.x;

    // Follow slightly, within 10 units of the starting position
    float offsetX = Math::clamp(distanceX * 0.1f, -10.0f, 10.0f);
    float targetX = cameraStartPosition.x + offsetX;
    float weight = 1.0f - Math::exp(-2.0f * float(delta));

    cameraPosition.x = Math::lerp(cameraPosition.x, targetX, weight);
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

// Sets the shake strength to a small amount
void GameCamera::camera_shake_small() {
    strength = 1.0;
    recoverySpeed = 20.0;
}

// Sets the shake strength to a larger amount
void GameCamera::camera_shake_big() {
    strength = 3.0;
    recoverySpeed = 20.0;
}

// Sets the shake strength to a very larger amount
void GameCamera::camera_shake_verybig() {
    strength = 5.0;
    recoverySpeed = 20.0;
}

void GameCamera::start_timer(double time_scale) {
    Timer *timer = memnew(Timer);
    timer->set_wait_time(0.03);
    timer->set_one_shot(true);
    timer->connect("timeout",callable_mp(this, &GameCamera::_on_timer_timeout));
    add_child(timer);
    Engine::get_singleton()->set_time_scale(time_scale);

    timer->start();
}

void GameCamera::_on_timer_timeout() {
    Engine::get_singleton()->set_time_scale(1.0);
}

void GameCamera::player_hurt() {
    strength = 5;
    start_timer(0.1);
}

void GameCamera::room_cleared() {
    strength = 15;
    start_timer(0.7);
}