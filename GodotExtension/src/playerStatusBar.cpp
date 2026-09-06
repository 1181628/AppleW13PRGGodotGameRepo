#include "PlayerStatusBar.h"
#include "playerStatus.h"

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

PlayerStatusBar::PlayerStatusBar() {
}

PlayerStatusBar::~PlayerStatusBar() {
}

void PlayerStatusBar::_bind_methods() {
}

void PlayerStatusBar::_ready() {
    // Prevents gameplay code from running inside the editor
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Displays the starting health bar animation
    refresh_player_status();
}

void PlayerStatusBar::refresh_player_status() {
    AnimationPlayer *animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");
    
    PlayerStatus *playerStatus = get_node<PlayerStatus>("/root/PlayerStatusData");
    int playerHealth = playerStatus->health;

    // Plays an animation matching the current health value
    if (playerHealth == 1) {
        animationPlayer->play("1");
    }
    if (playerHealth == 2) {
        animationPlayer->play("2");
    }
    if (playerHealth == 3) {
        animationPlayer->play("3");
    }
    if (playerHealth == 4) {
        animationPlayer->play("4");
    }
    if (playerHealth == 5) {
        animationPlayer->play("5");
    }
    // Prevents the health value from going below 0
    if (playerHealth <= 0) {
        playerHealth = 0;
        animationPlayer->play("0");
    }
    // Prevents the health value from going above 5
    if (playerHealth > 5) {
        playerHealth = 5;
        animationPlayer->play("5");
    }
}