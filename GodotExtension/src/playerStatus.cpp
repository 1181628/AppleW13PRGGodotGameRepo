#include "playerStatus.h"

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

PlayerStatus::PlayerStatus() {
}

PlayerStatus::~PlayerStatus() {
}

void PlayerStatus::_bind_methods() {
}

void PlayerStatus::_ready() {
    // Prevents gameplay code from running inside the editor
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    playerStatusValue = 5;
    // Displays the starting health bar animation
    refresh_player_status();
}

void PlayerStatus::refresh_player_status() {
    AnimationPlayer *animationPlayer = get_node<AnimationPlayer>("AnimationPlayer");

    // Plays an animation matching the current health value
    if (playerStatusValue == 1) {
        animationPlayer->play("1");
    }
    if (playerStatusValue == 2) {
        animationPlayer->play("2");
    }
    if (playerStatusValue == 3) {
        animationPlayer->play("3");
    }
    if (playerStatusValue == 4) {
        animationPlayer->play("4");
    }
    if (playerStatusValue == 5) {
        animationPlayer->play("5");
    }
    // Prevents the health value from going below 0
    if (playerStatusValue <= 0) {
        playerStatusValue = 0;
        animationPlayer->play("0");
    }
    // Prevents the health value from going above 5
    if (playerStatusValue > 5) {
        playerStatusValue = 5;
        animationPlayer->play("5");
    }
}