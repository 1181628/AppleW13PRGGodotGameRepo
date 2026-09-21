#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <vector>

namespace godot {

// The types of enemy that can be placed in a room
enum class EnemyType {
    ENEMY1,
    ENEMY2,
    ENEMY3
};

// Structure of info about one enemy's type and position
struct EnemySpawn {
    EnemyType type;
    Vector2 position;
};

// Groups each room's ID and enemy list so room data is separate from spawning logic
struct RoomInfo {
    int room_id;
   
    // A list of every enemy that belongs in this room
    std::vector<EnemySpawn> enemies;
};

class RoomManager : public Node {
    GDCLASS(RoomManager, Node)

private:
    // The list of every room in the game
    std::vector<RoomInfo> rooms;
    // The room Player is currently in
    int current_room_id;
    // Number of enemies defeated in the current room
    int defeated_enemy_count = 0;
    // Room Cleared
    bool room_is_cleared = false;

protected:
    static void _bind_methods();

public:
    RoomManager();
    ~RoomManager();

    void _ready();

    // Creates the empty room list
    void create_room_data();

    void load_room(int room_id, bool save_progress = true);
    void go_to_next_room();
    void save_current_progress();

    // Called by an Enemy when it dies
    void enemy_died();
};

}

#endif