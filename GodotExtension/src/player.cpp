#include "player.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Player::Player() {
}

Player::~Player() {
}

void Player::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("change_state", "new_state"),
        &Player::change_state
    );
}

void Player::_ready() {
}

//State Machine: controls which behaviour the player performs
void Player::_physics_process(double delta) {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Check the player's current state every physics frame and run only the code that belongs to that state
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
    is_state_new = false;
}

// Change the player's current state to the given new state
void Player::change_state(int new_state) {
    current_state = static_cast<State>(new_state);
    is_state_new = true;
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

    if (input->is_action_just_pressed("attack")) {
        call_deferred("change_state", static_cast<int>(State::ATTACK));
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
    Vector2 velocity = get_velocity();
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    // Play the attack animation once when entering ATTACK
    if (is_state_new) {animationPlayer->play("attack horizontally");
    }

    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {call_deferred("change_state", static_cast<int>(State::NORMAL));
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