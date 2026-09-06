#include "roomManager.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/object.hpp>

using namespace godot;

RoomManager::RoomManager() {
}

RoomManager::~RoomManager() {
}

void RoomManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("go_to_next_room"), &RoomManager::go_to_next_room);
}

void RoomManager::_ready() {
    // Lets Teleporter find this RoomManager.
    add_to_group("room_manager");

    create_room_data();

    // The game always begins in Room 0
    load_room(0);
}

void RoomManager::create_room_data() {
    // Room 0: Start room. It has no enemies
    rooms.push_back(RoomInfo{0, {
            // Empty
        }
    });

    // Room 1: Fixed enemy room 1
    rooms.push_back(RoomInfo{1, {
            { EnemyType::ENEMY1, Vector2(35, 80) },
            { EnemyType::ENEMY1, Vector2(88, 80) }
        }
    });
}

void RoomManager::load_room(int room_id) {
    // Records that Player is now in this room
    current_room_id = room_id;

    Node *room_objects = get_node<Node>("../RoomObjects");
    // Removes objects from the previous room
    for (int i = room_objects->get_child_count() - 1; i >= 0; i--) {
        room_objects->get_child(i)->queue_free();
    }

    Ref<PackedScene> enemy1_scene = ResourceLoader::get_singleton()->load("res://scenes/enemy1.tscn");
    // Reads every enemy listed in the current room.
    for (const EnemySpawn &enemy_spawn : rooms[current_room_id].enemies) {
        // If ENEMY1 needs to spawn
        if (enemy_spawn.type == EnemyType::ENEMY1) {
            // Creates one new Enemy1 from enemy1.tscn
            Node *new_enemy = enemy1_scene->instantiate();
             // Adds the new enemy under RoomObjects
            room_objects->add_child(new_enemy);
            Node2D *enemy_node = Object::cast_to<Node2D>(new_enemy);

            enemy_node->set_global_position(enemy_spawn.position);
        }
    }

    UtilityFunctions::print("Entered Room: ", current_room_id);
}

void RoomManager::go_to_next_room() {
    int next_room_id = current_room_id + 1;

    load_room(next_room_id);
}