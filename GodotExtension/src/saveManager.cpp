#include "saveManager.h"
#include "playerStatus.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>

using namespace godot;

SaveManager::SaveManager() {
}

SaveManager::~SaveManager() {
}

void SaveManager::_bind_methods() {
}

void SaveManager::_process(double delta) {
    // Do not count time while editing the scene.
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Access the elapsed time stored in PlayerStatus
    PlayerStatus *status = get_node<PlayerStatus>("/root/PlayerStatusData");
    // Count gameplay time until the player dies
    if (status->health > 0) {
        status->elapsedSeconds += delta;
    }
}

// =================================== SAVING CURRENT PROGRESS ===================================
// Save the player's stats and current room to a JSON file
void SaveManager::save_game(int current_room_id) {
    // Finds the PlayerStatus Autoload
    Node *player_status_node = get_node_or_null(NodePath("/root/PlayerStatusData"));

    PlayerStatus *player_status = Object::cast_to<PlayerStatus>(player_status_node);

    // Create a dictionary to hold all data to be saved
    Dictionary save_data;

    // Saves PlayerStatus values
    save_data["health"] = player_status->health;
    save_data["max_health"] = player_status->maxHealth;
    save_data["jump_height"] = player_status->jumpHeight;
    save_data["attack_damage"] = player_status->attackDamage;
    save_data["max_horizontal_speed"] = player_status->maxHorizontalSpeed;
    save_data["bonus_rerolls"] = player_status->bonusRerolls;

    // Saves which room the Player has entered
    save_data["current_room_id"] = current_room_id;
    // Save the time passed through the game
    save_data["elapsed_seconds"] = player_status->elapsedSeconds;

    // Opens or creates the save file inside Godot's user data folder
    Ref<FileAccess> save_file = FileAccess::open("user://save_game.json", FileAccess::WRITE);
   
    // Converts the Dictionary into JSON text and writes it into the file
    save_file->store_string(JSON::stringify(save_data));
}

// =================================== SAVE VALIDATION AND LOADING ===================================
// Load saved data into the supplied player status object
bool SaveManager::load_game(PlayerStatus *player_status) {
    // Stop loading if the save file does not exist
    if (!FileAccess::file_exists("user://save_game.json")) {
        OS::get_singleton()->alert("No previous saved game found.", "Load Game");
        return false;
    }

    // Open the save file for reading
    Ref<FileAccess> file = FileAccess::open("user://save_game.json", FileAccess::READ);

    // Stop loading if the file could not be opened
    if (file.is_null()) {
        return false;
    }

    // Declare a reference to a JSON parser
    Ref<JSON> json;
    // Create the JSON parser instance
    json.instantiate();

    // Stop if the JSON is invalid
    if (json->parse(file->get_as_text()) != OK) {
        return false;
    }

    // Retrieve the parsed JSON data
    Variant parsed = json->get_data();

    // Require the top-level JSON value to be a dictionary otherwise stop the loading
    if (parsed.get_type() != Variant::DICTIONARY) {
        return false;
    }

    // Convert the validated value to a dictionary
    Dictionary data = parsed;

    // Checks every required field before changing PlayerStatus,
    // so missing fields or incorrect types do not cause a partial restore
    const char *keys[] = {
        "health",
        "max_health",
        "jump_height",
        "attack_damage",
        "max_horizontal_speed",
        "bonus_rerolls",
        "current_room_id",
        "elapsed_seconds"
    };

    // Validate the presence and numeric type of each required field
    for (const char *key : keys) {
        // Stop loading if a required field is missing
        if (!data.has(key)) {
            return false;
        }

        // Retrieve the field value for type checking
        Variant value = data[key];

        // Accept only integer or floating-point values
        if (value.get_type() != Variant::INT && value.get_type() != Variant::FLOAT) {
            return false;
        }
    }
    // Restore the all the player's status
    player_status->health = int(data["health"]);
    player_status->maxHealth = int(data["max_health"]);
    player_status->jumpHeight = double(data["jump_height"]);
    player_status->attackDamage = int(data["attack_damage"]);
    player_status->maxHorizontalSpeed = double(data["max_horizontal_speed"]);
    player_status->bonusRerolls = int(data["bonus_rerolls"]);

    player_status->startRoomId = int(data["current_room_id"]);
    // Tells RoomManager not to overwrite the save when loading the starting room
    player_status->loadingSavedGame = true;

    player_status->elapsedSeconds = double(data.get("elapsed_seconds", 0.0));
    player_status->startRoomId = int(data["current_room_id"]);

    // Indicate that the saved data was successfully loaded
    return true;
}

// =================================== PAST RUN RECORDS ===================================
void SaveManager::save_past_record() {
    // Read the time and room ID
    PlayerStatus *status = get_node<PlayerStatus>("/root/PlayerStatusData");
    Ref<FileAccess> file = FileAccess::open("user://save_game.json", FileAccess::READ);

    // Stop if the current save cannot be opened
    if (file.is_null()) {
        return;
    }

    // Convert the JSON text into a Godot value
    Variant data = JSON::parse_string(file->get_as_text());
    file->close();

    // Check the parsed value before converting it into a Dictionary
    if (data.get_type() != Variant::DICTIONARY) {
        return;
    }

    // Create this run's result using only time and floor
    Dictionary game = data;
    Dictionary record;
    record["time"] = status->elapsedSeconds;
    record["floor"] = game["current_room_id"];
    // Start with an empty list for the player's first completed run
    Array records;

    // If a history file exists, read its previous results
    if (FileAccess::file_exists("user://past_records.json")) {
        file = FileAccess::open("user://past_records.json", FileAccess::READ);
        // Stop rather than overwrite history that could not be read
        if (file.is_null()) {
            return;
        }

        // Parse the history file and release the file handle
        data = JSON::parse_string(file->get_as_text());
        file->close();

        // Get the saved results
        Dictionary history = data;
        Variant old_records = history.get("records", Array());

        records = old_records;
    }

    // Insert this run at index 0 so the newest result appears first
    records.push_front(record);

    // Keeps only the latest three runs by removing the oldest results
    while (records.size() > 3) {
        records.pop_back();
    }
    // Store the results under the "records" key
    Dictionary history;
    history["records"] = records;
    // Open the history file for writing
    file = FileAccess::open("user://past_records.json", FileAccess::WRITE);

    // Keep the current save if the history file cannot be opened
    if (file.is_null()) {
        return;
    }

    // Convert the history Dictionary into JSON text and write it
    file->store_string(JSON::stringify(history));
    // Flush buffered data so pending writes reach the file
    file->flush();
    // Check for write errors before closing the file
    Error result = file->get_error();
    file->close();

    // Keeps the current save if writing the history failed.
    if (result != OK) {
        return;
    }

    // Attempts to remove this run's current save after saving its record
    DirAccess::remove_absolute("user://save_game.json");
}