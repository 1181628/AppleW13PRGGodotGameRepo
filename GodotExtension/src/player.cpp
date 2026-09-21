#include "player.h"
#include "playerStatus.h"
#include "playerStatusBar.h"
#include "interactable.h"
#include "gameCamera.h"
#include "saveManager.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/collision_polygon2d.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

using namespace godot;

Player::Player() {
}

Player::~Player() {
}

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("change_state", "new_state"), &Player::change_state);
}

void Player::_ready() {
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Adds the Player to the "player" group so enemies can find it
    add_to_group("player");

    // Connects the timer that ends the Player's invincibility
    Timer *invincibility_timer = get_node<Timer>("InvincibilityTimer");
    invincibility_timer->connect("timeout", callable_mp(this, &Player::_on_invincibility_timer_timeout));

    // Connects the timer that controls the flashing effect
    Timer *flash_timer = get_node<Timer>("FlashTimer");
    flash_timer->connect("timeout", callable_mp(this, &Player::_on_flash_timer_timeout));
    
    // Connects Player's hurtbox signal
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");
    hurtbox_area->connect("area_entered", callable_mp(this, &Player::_on_hurtbox_area_entered));
    // Connects Player's attack signal
    Area2D *attack_area = get_node<Area2D>("Attack");
    attack_area->connect("area_entered", callable_mp(this, &Player::_on_attack_area_entered));
}  


// =================================== PLAYER STATE MACHINE ===================================
void Player::_physics_process(double delta) {     
    // Stop the function running before the game starts
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Updates movement values from the Player's current stats
    PlayerStatus *player_status = get_node<PlayerStatus>("/root/PlayerStatusData");
    maxHorizontalSpeed = player_status->maxHorizontalSpeed;
    jumpHeight = player_status->jumpHeight;

    //  Runs the behaviour belonging to the current state
    switch (current_state) {
        case State::NORMAL:
            process_normal(delta);
            break;

        case State::ATTACK:
            process_attack(delta);
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
        velocity.y = jumpHeight * moveVector_y;
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
    // Allows jumping and movement while the attack animation plays
    apply_gravity_movement(delta);
}

// ================================== HURT STATE ===================================
void Player::process_hurt(double delta) {
    AnimationPlayer * animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    Vector2 velocity = get_velocity();

    // Play the hurt animation once when entering HURT
    if (is_state_new) {
        // Finds the child nodes and stores them in pointers
        Sprite2D * sprite = get_node<Sprite2D>("Sprite2D");
        Area2D *attack_area = get_node<Area2D>("Attack");
        Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");

        // Flip the sprite/attack/hurtbox area to face the direction opposite to right
        if (hurtDirection == "right") {
            // Knockback horizontal velocity
            velocity.x = 35;

            sprite->set_flip_h(true);

            Vector2 attack_scale =attack_area->get_scale();
            attack_scale.x = -1;
            attack_area->set_scale(attack_scale);

            Vector2 hurtbox_scale = hurtbox_area->get_scale();
            hurtbox_scale.x = -1;
            hurtbox_area->set_scale(hurtbox_scale);
        }
        // Flip the sprite/attack/hurtbox area to face the direction opposite to left
        if (hurtDirection == "left") {
            velocity.x = -35;

            sprite->set_flip_h(false);

            Vector2 attack_scale = attack_area->get_scale();
            attack_scale.x = 1;
            attack_area->set_scale(attack_scale);

            Vector2 hurtbox_scale = hurtbox_area->get_scale();
            hurtbox_scale.x = 1;
            hurtbox_area->set_scale(hurtbox_scale);
        }
        // Knockback vertical velocity
        velocity.y = -100;

        // Play a sound effect once when taking damage
        auto *sound = get_node<AudioStreamPlayer2D>("AudioStreamPlayer2D_Hurt");
        sound->play();
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

    // Returns to NORMAL when the hurt animation finishes
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

    velocity.x = 0;

    set_velocity(velocity);
    move_and_slide();

    // Play the death animation once when entering DIE
    if (is_state_new) {
        // Saves the run's record when the Player enters DIE
        SaveManager *save_manager = get_node<SaveManager>("/root/MainScene/SaveManager");
        save_manager->save_past_record();

        // Disable the hurtbox shape area after death
        get_node<CollisionPolygon2D>("HurtboxArea/Hurtbox")->set_disabled(true);
        // Stop the invincibility timer
        get_node<Timer>("InvincibilityTimer")->stop();
        animationPlayer->play("death");

        Node *camera = get_tree()->get_first_node_in_group("game_camera");
        camera->call_deferred("room_cleared");

        // Open the title screen after three real seconds
        Ref<SceneTreeTimer> timer =get_tree()->create_timer(3.0, true, false, true);
        timer->connect("timeout", Callable(get_tree(), "change_scene_to_file").bind("res://scenes/title_screen.tscn"));
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
            animationPlayer->play("jump");
        }
        if (velocity.y > 0) {
            // Fall
            animationPlayer->play("fall");
        }
    }
    else if (moveVector_x != 0) {
        // Run
        animationPlayer->play("walk"); 
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
        velocity.y = jumpHeight * moveVector_y;
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

    // Only restores the hurtbox while alive to prevent collisions after death
    if (current_state != State::DIE) {
        hurtbox->set_deferred("disabled", false);
    }

}

// Turns the Player and its collision areas towards the movement direction
void Player::_turn_direction() {
    Input *input = Input::get_singleton();
    double moveVector_x = input->get_axis("ui_left", "ui_right");
    Sprite2D * sprite = get_node<Sprite2D>("Sprite2D");
    Area2D *attack_area = get_node<Area2D>("Attack");
    Area2D *hurtbox_area = get_node<Area2D>("HurtboxArea");

    // When the Player moves, flip the sprite/attack/hurtbox collision area to face the direction it was moving toward
    if (moveVector_x < 0) {
        sprite->set_flip_h(true);

        Vector2 attack_scale =attack_area->get_scale();
        attack_scale.x = -1;
        attack_area->set_scale(attack_scale);

        Vector2 hurtbox_scale = hurtbox_area->get_scale();
        hurtbox_scale.x = -1;
        hurtbox_area->set_scale(hurtbox_scale);

    }
    if (moveVector_x > 0) {
        sprite->set_flip_h(false);

        Vector2 attack_scale = attack_area->get_scale();
        attack_scale.x = 1;
        attack_area->set_scale(attack_scale);

        Vector2 hurtbox_scale = hurtbox_area->get_scale();
        hurtbox_scale.x = 1;
        hurtbox_area->set_scale(hurtbox_scale);
    }
}

// Runs when another Area2D enters the Player's hurtbox
void Player::_on_hurtbox_area_entered(Area2D *area) {
    // Ignores repeated hits during invincibility so the Player has time to recover
    if (isInvincible) {
        return;
    }

    PlayerStatusBar *player_status_bar = get_node<PlayerStatusBar>("/root/StatusBar");
    PlayerStatus *player_status = get_node<PlayerStatus>("/root/PlayerStatusData");
    GameCamera *gameCamera = get_node<GameCamera>("/root/MainScene/GameCamera");

    // Reduces the Player's health
    player_status->take_damage(1);
    // Updates the health bar animation
    player_status_bar->refresh_player_status();
    // Acting effects
    gameCamera->camera_shake_big();
    gameCamera->player_hurt();   

    // Sets the knockback direction away from the incoming area
    if (area->get_global_position().x > get_global_position().x) {
        hurtDirection = "left";
    }
    else {
        hurtDirection = "right";
    }

    // Change state to DIE if Player has no more health otherwise change state to HURT
    if (player_status->health <= 0) {
        call_deferred("change_state", static_cast<int>(State::DIE));
    }
    else {
        // Starts the invincibility frames
        start_invincibility();
        call_deferred("change_state", static_cast<int>(State::HURT));
    }
}

// Runs when the Player's attack touches another Area2D
void Player::_on_attack_area_entered(Area2D *area) {
    Sprite2D *sprite = get_node<Sprite2D>("Sprite2D");
    Vector2 player_position = get_global_position();

    // Adds a small camera shake when the attack touches another area
    GameCamera *gameCamera = get_node<GameCamera>("/root/MainScene/GameCamera");
    gameCamera->camera_shake_small();   

    // Randomized the two sound effects
    const char *sound_paths[] = {
        "AudioStreamPlayer2D_Attack1",
        "AudioStreamPlayer2D_Attack2"
    };
    int index = UtilityFunctions::randi_range(0, 1);
    // Play a random sound effect once when attacking enemies
    AudioStreamPlayer2D *sound = get_node<AudioStreamPlayer2D>(sound_paths[index]);
    sound->play();

    // I disabled attack recoil because changing the Player's position caused a single attack to hit more than once
    // Due to time constraints, I set the movement to 0
    // I kept this code to document the approach I tried and use it as a reference when studying this problem later
    if (!sprite->is_flipped_h()) {
        // Facing to the right, the player moves backward to the left
        player_position.x -= 0;
    } else {
        // Facing to the left, the player moves backward to the right
        player_position.x += 0;
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