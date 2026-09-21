#include "roomManager.h"
#include "saveManager.h"
#include "playerStatus.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/animation_player.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>
#include <cmath>

using namespace godot;

RoomManager::RoomManager() {
}

RoomManager::~RoomManager() {
}

void RoomManager::_bind_methods() {
    // Register the method
    ClassDB::bind_method(D_METHOD("go_to_next_room"), &RoomManager::go_to_next_room);
}

// =================================== ROOM INITIALISATION ===================================
void RoomManager::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    // Lets Teleporter find this RoomManager
    add_to_group("room_manager");

    // Create the room definitions and their enemy spawn lists
    create_room_data();

    // Hides the teleporter and disables its processing during initial setup
    Node2D *teleporter = Object::cast_to<Node2D>(get_node_or_null(NodePath("../TileMapLayer/Teleporter")));
    teleporter->hide();
    teleporter->set_process_mode(Node::PROCESS_MODE_DISABLED);

    // Read the starting room ID from the player's status
    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));
    int room_id = status->startRoomId;

    // Load the starting room and save only if this is not a loaded game
    load_room(room_id, !status->loadingSavedGame);
    // Clear the loading flag after the starting room has been loaded
    status->loadingSavedGame = false;
}

// =================================== ROOM DATA ===================================
// Define each room and its enemy spawn positions
void RoomManager::create_room_data() {
    // Define rooms manually from Room 0 to Room 4
    //
    // Rules:
    // 1. Room 0 is the tutorial room and has no enemies
    // 2. Rooms 1–4 contain the same number of enemies as their room ID
    // 3. Enemy types and spawn positions are predefined
    // 4. These starting rooms use the same enemy layouts each run

    // Room 0: Start room. It has no enemies
    rooms.push_back(RoomInfo{0, {
            // Empty
        }
    });

    // Room 1: 1 enemy
    rooms.push_back(RoomInfo{1, {
            { EnemyType::ENEMY2, Vector2(50, 80) }
        }
    });

    // Room 2: 2 enemies
    rooms.push_back(RoomInfo{2, {
            { EnemyType::ENEMY1, Vector2(35, 80) },
            { EnemyType::ENEMY3, Vector2(75, 80) }
        }
    });

    // Room 3: 3 enemies
    rooms.push_back(RoomInfo{3, {
            { EnemyType::ENEMY2, Vector2(35, 80) },
            { EnemyType::ENEMY2, Vector2(50, 80) },
            { EnemyType::ENEMY1, Vector2(75, 80) }
        }
    });

    // Room 4: 4 enemies
    rooms.push_back(RoomInfo{4, {
            { EnemyType::ENEMY3, Vector2(35, 80) },
            { EnemyType::ENEMY3, Vector2(50, 80) },
            { EnemyType::ENEMY3, Vector2(75, 80) },
            { EnemyType::ENEMY3, Vector2(90, 80) }
        }
    });
}

// Reset room progress, spawn the room's enemies, and optionally save
void RoomManager::load_room(int room_id, bool save_progress) {
    // Generate rooms dynamically from Room 5 onward
    //
    // Rules:
    // 1. Each new room ID is the previous room ID + 1
    // 2. Each new room has one more enemy than the previous room
    // 3. Enemy types are random
    // 4. Enemies mainly spawn on the left or right side
    // 5. Enemies are allowed to overlap
    if (room_id >= 5) {
        Ref<RandomNumberGenerator> rng;
        rng.instantiate();
        rng->randomize();

        // Generates all missing room definitions up to the requested saved room
        while (room_id >= static_cast<int>(rooms.size())) {
            RoomInfo random_room;

            // rooms.size() is the next sequential room ID
            random_room.room_id = static_cast<int>(rooms.size());

            // Give the new room one more enemy than the previous room
            int enemy_count = static_cast<int>(rooms.back().enemies.size()) + 1;

            for (int i = 0; i < enemy_count; i++) {
                EnemySpawn enemy;

                // Randomly select ENEMY1, ENEMY2 or ENEMY3
                enemy.type = static_cast<EnemyType>(rng->randi_range(0, 2));

                double random_x = 0.0;
                // Randomly choose the left side or right side
                bool spawn_on_left = rng->randi_range(0, 1) == 0;
                if (spawn_on_left) {
                    random_x = rng->randf_range(-160.0, -40.0);
                } else {
                    random_x = rng->randf_range(40.0, 140.0);
                }

                // Enemies are allowed to use the same or nearby positions
                enemy.position = Vector2(random_x, 80);
                random_room.enemies.push_back(enemy);
            }

            // Store the generated room
            rooms.push_back(random_room);
        }
    }

    // =================================== ROOM LOADING AND GENERATION ===================================
    // Records that Player is now in this room
    current_room_id = room_id;
    // Updates the displayed floor whenever a room is loaded
    Label *floor_label = Object::cast_to<Label>(get_node_or_null(NodePath("/root/StatusBar/FloorLabel")));
    floor_label->set_text(String("Floor: ") + String::num_int64(current_room_id));
    // Every newly entered room starts with no defeated enemies & a not cleared room
    defeated_enemy_count = 0;
    room_is_cleared = false;

    // Handles the tutorial room separately from combat rooms
    if (room_id == 0) {
        AnimationPlayer *animation = Object::cast_to<AnimationPlayer>(get_node_or_null(NodePath("../CanvasLayer/AnimationPlayer")));
        if (animation) animation->play("tutorial");

        // Enables the teleporter's processing after the tutorial delay
        Node2D *teleporter = Object::cast_to<Node2D>(get_node_or_null(NodePath("../TileMapLayer/Teleporter")));
        Ref<SceneTreeTimer> timer = get_tree()->create_timer(12.0, false);
        timer->connect("timeout", Callable(teleporter, "set_process_mode").bind(Node::PROCESS_MODE_INHERIT));
    }else {
        // Hides tutorial instructions when loading a combat room
        AnimationPlayer *animation = Object::cast_to<AnimationPlayer>(get_node_or_null(NodePath("../CanvasLayer/AnimationPlayer")));
        const char *label_paths[] = {"../CanvasLayer/Label1"};
        for (const char *path : label_paths) {Label *label = Object::cast_to<Label>(get_node_or_null(NodePath(path)));
            if (label) {
                label->hide();
            }
        }
    }

    // Removes objects from the previous room
    Node *room_objects = get_node<Node>("../RoomObjects");
    for (int i = room_objects->get_child_count() - 1; i >= 0; i--) {
        room_objects->get_child(i)->queue_free();
    }

    // Load the scene for the enemys
    Ref<PackedScene> enemy1_scene = ResourceLoader::get_singleton()->load("res://scenes/enemy1.tscn");
    Ref<PackedScene> enemy2_scene = ResourceLoader::get_singleton()->load("res://scenes/enemy2.tscn");
    Ref<PackedScene> enemy3_scene = ResourceLoader::get_singleton()->load("res://scenes/enemy3.tscn");

    // Uses the same spawning loop for both predefined and randomly generated rooms
    for (const EnemySpawn &enemy_spawn : rooms[current_room_id].enemies) {
        // If ENEMY1 needs to spawn
        if (enemy_spawn.type == EnemyType::ENEMY1) {
            // Creates one new Enemy1 from enemy1.tscn
            Node *new_enemy = enemy1_scene->instantiate();
             // Adds the new enemy under RoomObjects
            room_objects->add_child(new_enemy);
            // Place the enemy at its configured global position
            Node2D *enemy_node = Object::cast_to<Node2D>(new_enemy);
            enemy_node->set_global_position(enemy_spawn.position);
        }
        // If ENEMY2 needs to spawn
        if (enemy_spawn.type == EnemyType::ENEMY2) {
            Node *new_enemy = enemy2_scene->instantiate();
            room_objects->add_child(new_enemy);
            Node2D *enemy_node = Object::cast_to<Node2D>(new_enemy);
            enemy_node->set_global_position(enemy_spawn.position);
        }
        // If ENEMY3 needs to spawn
        if (enemy_spawn.type == EnemyType::ENEMY3) {
            Node *new_enemy = enemy3_scene->instantiate();
            room_objects->add_child(new_enemy);
            Node2D *enemy_node = Object::cast_to<Node2D>(new_enemy);
            enemy_node->set_global_position(enemy_spawn.position);
        }
    }
    // Automatically saves when proceeding to the next floor
    if (save_progress) {
        save_current_progress();
    }
}

// Advance to the room with the next sequential ID
void RoomManager::go_to_next_room() {
    // Count and load
    int next_room_id = current_room_id + 1;
    load_room(next_room_id);
}

// Ask the save manager to save progress in the current room
void RoomManager::save_current_progress() {
    // Finds SaveManager beside RoomManager
    Node *save_manager_node = get_node_or_null(NodePath("../SaveManager"));
    SaveManager *save_manager = Object::cast_to<SaveManager>(save_manager_node);
    // Save the player's data together with the current room ID
    save_manager->save_game(current_room_id);
}

// =================================== ROOM CLEARED ===================================
// Trigger the camera effect and open rewards after the room is cleared
void RoomManager::enemy_died() {
    // Trigger room completion only once
    if (room_is_cleared) {
        return;
    }

    int total_enemy_count = static_cast<int>(rooms[current_room_id].enemies.size());
    defeated_enemy_count += 1;

    // Checks whether all enemies in this room have been defeated
    if (defeated_enemy_count >= total_enemy_count) {
        // Prevents later death notifications from triggering rewards again
        room_is_cleared = true;

        Node *camera = get_tree()->get_first_node_in_group("game_camera");
        Node *reward_screen = get_tree()->get_first_node_in_group("reward_screen");

        camera->call_deferred("room_cleared");

        // Open the reward screen after three real seconds
        Ref<SceneTreeTimer> timer = get_tree()->create_timer(3.0, true, false, true);
        timer->connect("timeout", Callable(reward_screen, "open_reward"));
    }
}