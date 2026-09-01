#include "player.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace godot;

Player::Player() {
}

Player::~Player() {
}

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("change_state", "new_state"), &Player::change_state);
}

void Player::_ready() {
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Player::_on_hurtbox_area_entered));

    Area2D *attack1_area = get_node<Area2D>("Attack1");
    attack1_area->connect("area_entered", callable_mp(this, &Player::_on_attack1_area_entered));
}


// ======================================== PLAYER STATE MACHINE ========================================
// The state machine checks the player's current state and calls only the function belonging to that state
// This tidies and separates the code into parts so that their behaviours do not all run at the same time
void Player::_physics_process(double delta) {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Select the correct behaviour using the player's current state
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

void Player::apply_gravity_movement(double delta) {
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
    // update turning direction
    _turn_direction();
}

void Player::process_dash(double delta) {
    current_state = State::NORMAL;
}

void Player::process_attack(double delta) {
    Vector2 velocity = get_velocity();
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    // Play the attack animation once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("attack horizontally");
    }

    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::NORMAL));
    }

    apply_gravity_movement(delta);
}

void Player::process_attack_up(double delta) {
    current_state = State::NORMAL;
}

void Player::process_attack_down(double delta) {
    current_state = State::NORMAL;
}

void Player::_update_animation() {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    Input *input = Input::get_singleton();
    Vector2 velocity = get_velocity();
    double moveVector_x = input->get_axis("ui_left", "ui_right");

    if (!is_on_floor()) {
        if (velocity.y < 0) {
            animationPlayer->play("idle");
        }
        if (velocity.y > 0) {
            animationPlayer->play("idle");
        }
    }
    else if (moveVector_x != 0) {
        animationPlayer->play("idle"); 
    }
    else {
        animationPlayer->play("idle");         
    }
}

void Player::_turn_direction() {
    Input *input = Input::get_singleton();
    double moveVector_x = input->get_axis("ui_left", "ui_right");
    Sprite2D * sprite = get_node<Sprite2D>("Sprite2D");
    Area2D *attack1_area = get_node<Area2D>("Attack1");
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");

    // When the Player moves, flip the sprite to face the direction it was moving toward
    if (moveVector_x < 0) {
        sprite->set_flip_h(true);

        Vector2 attack1_scale =attack1_area->get_scale();
        attack1_scale.x = -1;
        attack1_area->set_scale(attack1_scale);

        Vector2 hurtbox_scale = hurtbox_area->get_scale();
        hurtbox_scale.x = -1;
        hurtbox_area->set_scale(hurtbox_scale);

    }
    if (moveVector_x > 0) {
        sprite->set_flip_h(false);

        Vector2 attack1_scale = attack1_area->get_scale();
        attack1_scale.x = 1;
        attack1_area->set_scale(attack1_scale);

        Vector2 hurtbox_scale = hurtbox_area->get_scale();
        hurtbox_scale.x = 1;
        hurtbox_area->set_scale(hurtbox_scale);
    }
}

void Player::_on_hurtbox_area_entered(Area2D *area) {
    // Reduce player health here
}

void Player::_on_attack1_area_entered(Area2D *area) {
    Sprite2D *sprite = get_node<Sprite2D>("Sprite2D");
    Vector2 player_position = get_global_position();

    if (!sprite->is_flipped_h()) {
        // Facing to the right, the player moves backward to the left
        player_position.x -= 2;
    } else {
        // Facing to the left, the player moves backward to the right
        player_position.x += 2;
    }

    set_global_position(player_position);
}