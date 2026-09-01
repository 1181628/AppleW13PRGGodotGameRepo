#include "enemy1.h"

// Add other includes when needed.
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
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

        case State::WALK:
            process_walk(delta);
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
    // gravity & velocity
    Vector2 velocity = get_velocity();
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }
    set_velocity(velocity);
    move_and_slide();
    
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");  
    
    _turn_direction();

    // Play the attack animation once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("idle");  
    }
    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::WALK));
    }
}

void Enemy1::process_walk(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");    

    // Play the attack animation once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("walk");  
    }
    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::ATTACK));
    }
}

void Enemy1::process_attack(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");    
        // Play the attack animation once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("attack");  
    }
    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::NORMAL));
    }
}

void Enemy1::_turn_direction() {
    Area2D *attackhitbox_area = get_node<Area2D>("AttackHitboxArea");
    Area2D *bodyhitbox_area = get_node<Area2D>("BodyHitboxArea");
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    Node2D *sprite_area = get_node<Node2D>("SpriteArea");
    Node2D *player = get_node<Node2D>("/root/MainScene/Player");

    double direction;

    // Checks whether the Player is on the left side of Enemy1
    if (player->get_global_position().x <
        get_global_position().x) {
        // Face left
        direction = 1;
    } else {
        // Face right
        direction = -1;
    }

    // Applies the changed scale
    Vector2 sprite_scale = sprite_area->get_scale();
    sprite_scale.x = direction;
    sprite_area->set_scale(sprite_scale);

    Vector2 hurtbox_scale = hurtbox_area->get_scale();
    hurtbox_scale.x = direction;
    hurtbox_area->set_scale(hurtbox_scale);

    Vector2 bodyhitbox_scale = bodyhitbox_area->get_scale();
    bodyhitbox_scale.x = direction;
    bodyhitbox_area->set_scale(bodyhitbox_scale);

    Vector2 attackhitbox_scale = attackhitbox_area->get_scale();
    attackhitbox_scale.x = direction;
    attackhitbox_area->set_scale(attackhitbox_scale);
}

void Enemy1::_on_hurtbox_area_entered(Area2D *area) {
    get_node<Timer>("MaterialTimer")->start();
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(false);
}

void Enemy1::_on_material_timer_timeout() {
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(true);
}