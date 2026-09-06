#include "playerStatus.h"

using namespace godot;

PlayerStatus::PlayerStatus() {
}

PlayerStatus::~PlayerStatus() {
}

void PlayerStatus::_bind_methods() {
}

void PlayerStatus::take_damage(int damage) {
    health -= damage;

    if (health < 0) {
        health = 0;
    }
}

void PlayerStatus::heal(int healAmount) {
    health += healAmount;

    if (health > maxHealth) {
        health = maxHealth;
    }
}