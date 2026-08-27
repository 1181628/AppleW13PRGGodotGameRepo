#include "enemy1.h"

// Add other includes when needed.
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>
// #include <godot_cpp/variant/vector2.hpp>
// #include <godot_cpp/classes/input.hpp>

using namespace godot;


Enemy1::Enemy1() {
}

Enemy1::~Enemy1() {
}

void Enemy1::_bind_methods() {
    ClassDB::bind_method(D_METHOD("change_state", "new_state"), &Enemy1::change_state);
}

void Enemy1::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Enemy1::_on_hurtbox_area_entered));

    Timer *material_timer = get_node<Timer>("MaterialTimer");
    material_timer->connect("timeout", callable_mp(this, &Enemy1::_on_material_timer_timeout));
}

// ======================================== ENEMY1 STATE MACHINE ========================================
// The state machine checks the enemy1's current state and calls only the function belonging to that state
// This tidies and separates the code into parts so that their behaviours do not all run at the same time
void Enemy1::_process(double delta) {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Select the correct behaviour using the enemy1's current state
    switch (current_state) {
        case State::NORMAL:
            process_normal(delta);
            break;

        case State::ATTACK:
            process_attack(delta);
            break;
    }
    is_state_new = false;
}

// Change the enemy1's current state to the given new state
void Enemy1::change_state(int new_state) {
    current_state = static_cast<State>(new_state);
    is_state_new = true;
}

// Runs every rendered frame.
void Enemy1::process_normal(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");    
    animationPlayer->play("idle");     

}

void Enemy1::process_attack(double delta) {
    current_state = State::NORMAL;
}

void Enemy1::_on_hurtbox_area_entered(Area2D *area) {
    get_node<Timer>("MaterialTimer")->start();
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(false);
}

void Enemy1::_on_material_timer_timeout() {
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(true);
}