#include "teleporter.h"

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/animation_player.hpp>

using namespace godot;

Teleporter::Teleporter() {
}

Teleporter::~Teleporter() {
}

void Teleporter::_bind_methods() {
}

void Teleporter::interact() {
    // Runs the original Interactable interaction
    Interactable::interact();
    // Stops the tutorial animation and hides its instruction label
    AnimationPlayer *animation = Object::cast_to<AnimationPlayer>(get_node_or_null(NodePath("../../CanvasLayer/AnimationPlayer")));
    animation->stop();
    animation->set_process_mode(Node::PROCESS_MODE_DISABLED);
    Node *label1 = get_node_or_null(NodePath("../../CanvasLayer/Label1"));
    label1->call("hide");

    // Asks RoomManager to move the game to the next room
    get_tree()->call_group("room_manager", "go_to_next_room");
}