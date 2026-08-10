#include "player.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>

using namespace godot;

Player::Player() {
}

Player::~Player() {
}

void Player::_bind_methods() {
}

void Player::_ready() {
}


void Player::_physics_process(double delta) {
    // Access Godot’s input system
    Input * input = Input::get_singleton();
    // Get the player’s current velocity
    Vector2 velocity = get_velocity();

    // gravity
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }

    // get movement
    double moveVector_y = 0;
    if (input->is_action_just_pressed("jump")) {
        moveVector_y = -1;
    }
    double moveVector_x = input->get_axis("move_left", "move_right");

    // jump
    if (moveVector_y == -1 && is_on_floor()) {
        velocity.y = jumpSpeed * moveVector_y;
    }

    // move left and right
    if (moveVector_x != 0 ) {
        velocity.x += moveVector_x * horizontal_acceleration * delta;
    } else {
        velocity.x = velocity.x / 2;
    }
    
    // 
    if (velocity.x < -maxHorizontalSpeed) {
        velocity.x = -maxHorizontalSpeed;
    }
    if (velocity.x > maxHorizontalSpeed) {
        velocity.x = maxHorizontalSpeed;
    }

    // Store modified velocity
    set_velocity(velocity);
    // Move the character
    move_and_slide();
    _update_animation();
}

void Player::_update_animation() {
    AnimationPlayer *animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    animationPlayer->play("idle");  

}