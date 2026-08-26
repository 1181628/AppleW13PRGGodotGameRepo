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
}

void Enemy1::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");    
    animationPlayer->play("idle");      

    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Enemy1::_on_hurtbox_area_entered));

    Timer *material_timer = get_node<Timer>("MaterialTimer");
    material_timer->connect("timeout", callable_mp(this, &Enemy1::_on_material_timer_timeout));
}


// Runs every rendered frame.
void Enemy1::_process(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    // Add frame-based behaviour here.
}

void Enemy1::_on_hurtbox_area_entered(Area2D *area) {
    get_node<Timer>("MaterialTimer")->start();
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(false);
}

void Enemy1::_on_material_timer_timeout() {
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(true);
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