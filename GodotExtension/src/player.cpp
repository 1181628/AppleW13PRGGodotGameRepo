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
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    switch (current_state) {
        case State::NORMAL:
            process_normal(delta);
            break;

        case State::DASH:
            process_dash(delta);
            break;

        case State::ATTACK:
            process_attack(delta);
            break;

        case State::ATTACK_UP:
            process_attack_up(delta);
            break;

        case State::ATTACK_DOWN:
            process_attack_down(delta);
            break;
    }
}

void Player::process_normal(double delta) {
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
    if (input->is_action_just_pressed("ui_accept")) {
        moveVector_y = -1;
    }
    double moveVector_x = input->get_axis("ui_left", "ui_right");

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
    
    // limit left right speed
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
    // update animations on the character
    _update_animation();
}

void Player::process_dash(double delta) {
    current_state = State::NORMAL;
}

void Player::process_attack(double delta) {
    Input * input = Input::get_singleton();
    Vector2 velocity = get_velocity();
    
    if (animation_player != nullptr) {
        animation_player->play("attack horizontally");
    }

    set_velocity(velocity);
    move_and_slide();
}

void Player::process_attack_up(double delta) {
    current_state = State::NORMAL;
}

void Player::process_attack_down(double delta) {
    current_state = State::NORMAL;
}

void Player::_update_animation() {
    AnimationPlayer *animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    animationPlayer->play("idle");  
}