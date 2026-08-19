#include "enemy1.h"

// Add other includes when needed.
#include <godot_cpp/classes/animation_player.hpp>
// #include <godot_cpp/variant/vector2.hpp>
// #include <godot_cpp/variant/utility_functions.hpp>
// #include <godot_cpp/classes/input.hpp>

using namespace godot;


Enemy1::Enemy1() {
}

Enemy1::~Enemy1() {
}

void Enemy1::_bind_methods() {
}

void Enemy1::_ready() {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    animationPlayer->play("idle");      
}


// Runs every rendered frame.
void Enemy1::_process(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    // Add frame-based behaviour here.
}


// Use this instead of _process() for movement and physics.
// It must also be declared in the .h file.
// void NewClass::_physics_process(double delta) {
//     (void)delta;
//
//     // Add movement, gravity, and physics behaviour here.
// }


// Define custom functions here.
// void NewClass::take_damage(int damage) {
// }
//
// void NewClass::attack() {
// }
//
// bool NewClass::is_alive() const {
//     return true;
// }