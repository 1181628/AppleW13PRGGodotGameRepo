#include "playerStatus.h"

using namespace godot;

PlayerStatus::PlayerStatus() {
}

PlayerStatus::~PlayerStatus() {
}

void PlayerStatus::_bind_methods() {
}

// Reduce the player's health by the specified damage amount
void PlayerStatus::take_damage(int damage) {
    health -= damage;
    // Prevent health from falling below zero
    if (health < 0) {
        health = 0;
    }
}

// Restore the player's health by the specified healing amount
void PlayerStatus::heal(int healAmount) {
    health += healAmount;
    // Prevent health from exceeding the maximum health
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// Reset the player's stats and loading state for a new game
void PlayerStatus::reset_for_new_game() {
    health = 5;
    maxHealth = 5;
    maxHorizontalSpeed = 120.0;
    jumpHeight = 250.0;
    attackDamage = 100;
    bonusRerolls = 0;
    startRoomId = 0;
    elapsedSeconds = 0.0;
    loadingSavedGame = false;
}