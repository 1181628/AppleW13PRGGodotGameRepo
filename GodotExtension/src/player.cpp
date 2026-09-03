#include "player.h"
#include "playerStatus.h"
#include "interactable.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/collision_polygon2d.hpp>

using namespace godot;

Player::Player() {
}

Player::~Player() {
}

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("change_state", "new_state"), &Player::change_state);
}

void Player::_ready() {
    // Adds the Player to the "player" group so enemies can find it
    add_to_group("player");

    Timer *invincibility_timer = get_node<Timer>("InvincibilityTimer");
    invincibility_timer->connect("timeout", callable_mp(this, &Player::_on_invincibility_timer_timeout));

    Timer *flash_timer = get_node<Timer>("FlashTimer");
    flash_timer->connect("timeout", callable_mp(this, &Player::_on_flash_timer_timeout));
    
    // Connects Player's hurtbox signal
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Player::_on_hurtbox_area_entered));
    // Connects Player's attack1 signal
    Area2D *attack1_area = get_node<Area2D>("Attack1");
    attack1_area->connect("area_entered", callable_mp(this, &Player::_on_attack1_area_entered));
}  


// =================================== PLAYER STATE MACHINE ===================================
void Player::_physics_process(double delta) {     
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    //  Runs the behaviour belonging to the current state
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

        case State::HURT:
            process_hurt(delta);
            break;

        case State::DIE:
            process_die(delta);
            break;
    }
    // After this physics frame, the current state is no longer new
    is_state_new = false;
}

// Change the player's current state to the given new state
void Player::change_state(int new_state) {
    current_state = static_cast<State>(new_state);
    is_state_new = true;
}

// ================================== NORMAL STATE ===================================
void Player::process_normal(double delta) {
    // Access Godot’s input system
    Input * input = Input::get_singleton();
    // Get the player’s current velocity
    Vector2 velocity = get_velocity();

    // Gravity
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }

    // Get movement
    double moveVector_y = 0;
    if (input->is_action_just_pressed("ui_accept")) {
        moveVector_y = -1;
    }
    double moveVector_x = input->get_axis("ui_left", "ui_right");

    // Jump
    if (moveVector_y == -1 && is_on_floor()) {
        velocity.y = jumpSpeed * moveVector_y;
    }

    // Move left and right
    if (moveVector_x != 0 ) {
        velocity.x += moveVector_x * horizontalAcceleration * delta;
    } else {
        velocity.x = velocity.x / 2;
    }
    
    // Limit left right speed
    if (velocity.x < -maxHorizontalSpeed) {
        velocity.x = -maxHorizontalSpeed;
    }
    if (velocity.x > maxHorizontalSpeed) {
        velocity.x = maxHorizontalSpeed;
    }

    // Change the state to ATTACK when attack was pressed
    if (input->is_action_just_pressed("attack")) {
        call_deferred("change_state", static_cast<int>(State::ATTACK));
    }

    // Store modified velocity
    set_velocity(velocity);
    // Move the character
    move_and_slide();
    // Update animations on the character
    _update_animation();
    // Update turning direction
    _turn_direction();
}

// Placeholder for the future DASH behaviour
void Player::process_dash(double delta) {
    current_state = State::NORMAL;
}

// ================================== ATTACK STATE ===================================
void Player::process_attack(double delta) {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    // Play the attack animation once when entering ATTACK
    if (is_state_new) {
        animationPlayer->play("attack horizontally");
    }

    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::NORMAL));
    }
    // Gravity but Player can jump/move during the same time
    apply_gravity_movement(delta);
}

// Placeholder for a future upward attack behaviour
void Player::process_attack_up(double delta) {
    current_state = State::NORMAL;
}

// Placeholder for a future downward attack behaviour
void Player::process_attack_down(double delta) {
    current_state = State::NORMAL;
}

// ================================== HURT STATE ===================================
void Player::process_hurt(double delta) {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    Vector2 velocity = get_velocity();

    // Play the hurt animation once when entering HURT
    if (is_state_new) {
        // Finds the child nodes and stores them in pointers
        Sprite2D * sprite = get_node<Sprite2D>("Sprite2D");
        Area2D *attack1_area = get_node<Area2D>("Attack1");
        Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");

        // Flip the sprite/attack1/hurtbox area to face the direction opposite to right
        if (hurtDirection == "right") {
            // Knockback horizontal velocity
            velocity.x = 35;

            sprite->set_flip_h(true);

            Vector2 attack1_scale =attack1_area->get_scale();
            attack1_scale.x = -1;
            attack1_area->set_scale(attack1_scale);

            Vector2 hurtbox_scale = hurtbox_area->get_scale();
            hurtbox_scale.x = -1;
            hurtbox_area->set_scale(hurtbox_scale);
        }
        // Flip the sprite/attack1/hurtbox area to face the direction opposite to left
        if (hurtDirection == "left") {
            velocity.x = -35;

            sprite->set_flip_h(false);

            Vector2 attack1_scale = attack1_area->get_scale();
            attack1_scale.x = 1;
            attack1_area->set_scale(attack1_scale);

            Vector2 hurtbox_scale = hurtbox_area->get_scale();
            hurtbox_scale.x = 1;
            hurtbox_area->set_scale(hurtbox_scale);
        }
        // Knockback vertical velocity
        velocity.y = -100;
        animationPlayer->play("getHit");
    }

    // Applies gravity during the knockback
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }
    // Saves and applies the knockback velocity
    set_velocity(velocity);
    // Move the character under the knockback velocity
    move_and_slide();

    // Return to NORMAL when the attack animation finishes
    if (!animationPlayer->is_playing()) {
        call_deferred("change_state", static_cast<int>(State::NORMAL));
    }
}

// ================================== DIE STATE ===================================
void Player::process_die(double delta) {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    Vector2 velocity = get_velocity();

    // Applies gravity during the death
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }
    set_velocity(velocity);
    move_and_slide();

    // Play the death animation once when entering DIE
    if (is_state_new) {
        // Disable the hurtbox shape area after death
        get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox")->set_disabled(true);
        // Stop the invincibility timer
        get_node<Timer>("InvincibilityTimer")->stop();
        animationPlayer->play("death");
    }
}

// Selects an animation based on movement and floor status
void Player::_update_animation() {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    Input *input = Input::get_singleton();
    Vector2 velocity = get_velocity();
    double moveVector_x = input->get_axis("ui_left", "ui_right");

    // Checks the Player's movement condition and selects a suitable animation
    if (!is_on_floor()) {
        if (velocity.y < 0) {
            // Jump
            animationPlayer->play("idle");
        }
        if (velocity.y > 0) {
            // Fall
            animationPlayer->play("idle");
        }
    }
    else if (moveVector_x != 0) {
        // Run
        animationPlayer->play("idle"); 
    }
    else {
        // Idle
        animationPlayer->play("idle");         
    }
}   

// Applies gravity, jumping and horizontal movement
void Player::apply_gravity_movement(double delta) {
    Input * input = Input::get_singleton();
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
        velocity.x += moveVector_x * horizontalAcceleration * delta;
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

// Start the Player's invincibility frames
void Player::start_invincibility() {
    isInvincible = true;

    CollisionPolygon2D *hurtbox =get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox");
    // Disables the Player's hurtbox temporarily
    hurtbox->set_deferred("disabled", true);

    // Starts the invincibility duration
    get_node<Timer>("InvincibilityTimer")->start();
    // Starts the flashing effect
    get_node<Timer>("FlashTimer")->start();
}

// Switches the Player between visible and invisible
void Player::_on_flash_timer_timeout() {
    Sprite2D *sprite = get_node<Sprite2D>("Sprite2D");
    sprite->set_visible(!sprite->is_visible());
}

// Ends the Player's invincibility frames
void Player::_on_invincibility_timer_timeout() {
    isInvincible = false;

    CollisionPolygon2D *hurtbox =get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox");
    Sprite2D *sprite = get_node<Sprite2D>("Sprite2D");

    // Stops the flashing effect
    get_node<Timer>("FlashTimer")->stop();

    // Makes sure the Player is visible
    sprite->set_visible(true);
    // Enables the Player's hurtbox again
    hurtbox->set_deferred("disabled", false);

    // Only enables the hurtbox while the Player is alive
    if (current_state != State::DIE) {
        get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox")->set_disabled(false);
    }

}

// Turns the Player and its collision areas towards the movement direction
void Player::_turn_direction() {
    Input *input = Input::get_singleton();
    double moveVector_x = input->get_axis("ui_left", "ui_right");
    Sprite2D * sprite = get_node<Sprite2D>("Sprite2D");
    Area2D *attack1_area = get_node<Area2D>("Attack1");
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");

    // When the Player moves, flip the sprite/attack1/hurtbox collision area to face the direction it was moving toward
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

// Runs when another Area2D enters the Player's hurtbox
void Player::_on_hurtbox_area_entered(Area2D *area) {
    // Ignores damage during the invincibility frames
    if (isInvincible) {
        return;
    }

    PlayerStatus *player_status = get_node<PlayerStatus>("/root/Status");
    // Reduces the Player's health
    player_status->playerStatusValue -= 1;
    // Updates the health bar animation
    player_status->refresh_player_status();

    // Checks whether the attack came from which side
    if (area->get_global_position().x > get_global_position().x) {
        hurtDirection = "left";
    }
    else {
        hurtDirection = "right";
    }

    // Change state to DIE if Player has no more health otherwise change state to HURT
    if (player_status->playerStatusValue <= 0) {
        call_deferred("change_state", static_cast<int>(State::DIE));
    }
    else {
        // Starts the invincibility frames
        start_invincibility();
        call_deferred("change_state", static_cast<int>(State::HURT));
    }
}

// Runs when the Player's attack touches another Area2D
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

    // Applies the changed position
    set_global_position(player_position);
}

void Player::_unhandled_input(const Ref<InputEvent> &event) {
    // Interacts with the nearby object when the interact key is pressed
    if (
        event->is_action_pressed("interact") &&
        interactingWith != nullptr
    ) {
        interactingWith->interact();
    }
}