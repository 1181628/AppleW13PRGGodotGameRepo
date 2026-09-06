#include "enemy1.h"
#include "playerStatus.h"

// Add other includes when needed.
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/collision_polygon2d.hpp>

using namespace godot;

Enemy1::Enemy1() {
}

Enemy1::~Enemy1() {
}

void Enemy1::_bind_methods() {
    ClassDB::bind_method(D_METHOD("change_state", "new_state"), &Enemy1::change_state);
}

void Enemy1::_ready() {
    // Connects Enemy1's hurtbox signal
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Enemy1::_on_hurtbox_area_entered));
    // Connects the material timer signal
    Timer *material_timer = get_node<Timer>("MaterialTimer");
    material_timer->connect("timeout", callable_mp(this, &Enemy1::_on_material_timer_timeout));
}

// ================================== ENEMY1 STATE MACHINE ===================================
void Enemy1::_process(double delta) {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Find Player's position
    match_player_position();

    //  Runs the behaviour belonging to the current state
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

        case State::DIE:
            process_die(delta);
            break;
    }
    // After this physics frame, the current state is no longer new
    is_state_new = false;
}

// Change the enemy1's current state to the given new state
void Enemy1::change_state(int new_state) {
    current_state = static_cast<State>(new_state);
    is_state_new = true;
}

// ================================== NORMAL STATE ===================================
void Enemy1::process_normal(double delta) {
    // Remove this line when delta is used
    // There will be a gravity effect added to Enemy1 but not now
    (void)delta;

    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");  

    // Stops all movement while Enemy1 is idle
    Vector2 velocity = get_velocity();
    velocity.x = 0;
    velocity.y = 0;    
    set_velocity(velocity);
    move_and_slide();
    
    _turn_direction();

    // Plays idle once when entering NORMAL
    if (is_state_new) {
        animationPlayer->play("idle");  
    }
    // Selects the next state after idle finishes
    if (!animationPlayer->is_playing()) {
        // Calculates the horizontal distance between Enemy1 and Player
        double distance = std::abs(playerPosition.x - get_global_position().x);
        // Attack when the Player is less than 80 pixels away else move closer
        if (distance < attackRange) {
            call_deferred("change_state", static_cast<int>(State::ATTACK));
        } else {
            call_deferred("change_state", static_cast<int>(State::WALK));
        }
    }
}

// ================================== WALK STATE ===================================
void Enemy1::process_walk(double delta) {
    (void)delta;
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");   
    Vector2 velocity = get_velocity();
    _turn_direction();

    // Plays walk once when entering WALK
    if (is_state_new) {
        // Move right when the Player is on the right
        if (playerPosition.x > get_global_position().x) {
            velocity.x = walkSpeed;
        }
        // Move left when the Player is on the left
        else {
            velocity.x = -walkSpeed;
        }
        // Removes the previous vertical velocity
        velocity.y = 0;
        animationPlayer->play("walk");  
    }
    set_velocity(velocity);
    move_and_slide();

    // Attacks when close enough to the Player
    if (std::abs(playerPosition.x - get_global_position().x) < attackRange) {
        call_deferred("change_state", static_cast<int>(State::ATTACK));
    }
}

// ================================== ATTACK STATE ===================================
void Enemy1::process_attack(double delta) {
    (void)delta;
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");  
    Vector2 velocity = get_velocity();  

    // Stops all movement during the attack
    velocity.x = 0;
    velocity.y = 0;
    set_velocity(velocity);
    move_and_slide();

    // Plays attack once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("attack");  
    }
    // Returns to NORMAL after the attack finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::NORMAL));
    }
}

// ================================== DIE STATE ===================================
void Enemy1::process_die(double delta) {
    (void)delta;
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");  

    // Plays die once when entering DIE
    if (is_state_new) {
        // Disables all collision shapes
        get_node<CollisionPolygon2D>("CollisionPolygon2D")->set_disabled(true);
        get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox")->set_disabled(true);
        get_node<CollisionPolygon2D>("BodyHitboxArea/BodyHitbox")->set_disabled(true);
        get_node<CollisionPolygon2D>("AttackHitboxArea/AttackHitbox")->set_disabled(true);

        animationPlayer->play("death");  
    }
}

void Enemy1::_turn_direction() {
    Area2D *attackhitbox_area = get_node<Area2D>("AttackHitboxArea");
    Area2D *bodyhitbox_area = get_node<Area2D>("BodyHitboxArea");
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    Node2D *sprite_area = get_node<Node2D>("SpriteArea");

    double direction;

    // Checks whether the Player is on the left side of Enemy1
    if (playerPosition.x < get_global_position().x) {
        // Face left
        direction = 1;
    }
    else {
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

void Enemy1::match_player_position() {
    TypedArray<Node> players = get_tree()->get_nodes_in_group("player");

    if (players.size() > 0) {
        Node2D *player = Object::cast_to<Node2D>(players[0]);
        // Store the Player's global position
        if (player) {
            playerPosition = player->get_global_position();
        }
    }
}

// Runs when another Area2D enters the Enemy1's hurtbox
void Enemy1::_on_hurtbox_area_entered(Area2D *area) {
    get_node<Timer>("MaterialTimer")->start();
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(false);
    StringName areaName = area->get_name();

    PlayerStatus *player_status = get_node<PlayerStatus>("/root/PlayerStatusData");

    // Only deal damage if the entering area is the Player's Attack1 hitbox
    if (areaName == StringName("Attack1")) {
        Health -= player_status->attackDamage;
    }

    // Change state to DIE when Enemy1 has no health remaining
    if (Health <= 0) {
        call_deferred("change_state", static_cast<int>(State::DIE));
    }

}

// called automatically when Enemy1's MaterialTimer finishes
void Enemy1::_on_material_timer_timeout() {
    get_node<Sprite2D>("SpriteArea/Sprite2D")->set_use_parent_material(true);
}