#include "saveManager.h"
#include "playerStatus.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

SaveManager::SaveManager() {
}

SaveManager::~SaveManager() {
}

void SaveManager::_bind_methods() {
}

void SaveManager::save_game(int current_room_id) {
    // Finds the PlayerStatus Autoload
    Node *player_status_node = get_node_or_null(NodePath("/root/PlayerStatusData"));

    PlayerStatus *player_status = Object::cast_to<PlayerStatus>(player_status_node);

    // Creates one collection containing all data to save
    Dictionary save_data;

    // Saves PlayerStatus values
    save_data["health"] = player_status->health;
    save_data["max_health"] = player_status->maxHealth;
    save_data["jump_height"] = player_status->jumpHeight;
    save_data["attack_damage"] = player_status->attackDamage;
    save_data["max_horizontal_speed"] = player_status->maxHorizontalSpeed;

    // Saves which room the Player has entered
    save_data["current_room_id"] = current_room_id;

    // Opens or creates the save file inside Godot's user data folder
    Ref<FileAccess> save_file = FileAccess::open("user://save_game.json", FileAccess::WRITE);
    
    // Converts the Dictionary into JSON text and writes it into the file
    save_file->store_string(JSON::stringify(save_data));
    
    UtilityFunctions::print("Game saved in Room: ", current_room_id);
}