#include "teleporter.h"

#include <godot_cpp/classes/scene_tree.hpp>

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

    // Changes to the target scene
    get_tree()->change_scene_to_file(path);
}