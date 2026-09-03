#include "interactable.h"
#include "player.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Interactable::Interactable() {
}

Interactable::~Interactable() {
}

void Interactable::_bind_methods() {
    // Allows interact() to be called by Godot
    ClassDB::bind_method(D_METHOD("interact"), &Interactable::interact);
    // Creates the interacted signal
    ADD_SIGNAL(MethodInfo("interacted"));
}

void Interactable::_ready() {
    // Clears all collision layber and masks
    set_collision_layer(0);
    set_collision_mask(0);

    // Detects bodies on collision layer 2
    set_collision_mask_value(2, true);

    // Connects the body entered signal
    connect("body_entered", callable_mp(this, &Interactable::_on_body_entered));

    // Connects the body exited signal
    connect("body_exited", callable_mp(this, &Interactable::_on_body_exited));
}

void Interactable::interact() {
    UtilityFunctions::print("[Interact] ", get_name());

    // Sends the interacted signal
    emit_signal("interacted");
}

void Interactable::_on_body_entered(Node2D *body) {
    // Checks whether the body is the Player
    Player *player = Object::cast_to<Player>(body);

    if (player) {
        // Stores this object as the Player's current interaction
        player->interactingWith = this;
    }
}

void Interactable::_on_body_exited(Node2D *body) {
    // Checks whether the body is the Player
    Player *player = Object::cast_to<Player>(body);

    if (player) {
        // Removes the current interaction
        player->interactingWith = nullptr;
    }
}