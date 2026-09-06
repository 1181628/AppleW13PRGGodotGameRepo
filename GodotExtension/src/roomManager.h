#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <vector>

namespace godot {

// The types of enemy that can be placed in a room.
enum class EnemyType {
    ENEMY1
    //ENEMY2,
    //ENEMY3,
    //BOSS
};

// Information about one enemy's type and position.
struct EnemySpawn {
    EnemyType type;
    Vector2 position;
};

// structure of info that every room will have
struct RoomInfo {
    int room_id;
    
    // A list of every enemy that belongs in this room.
    std::vector<EnemySpawn> enemies;
};

class RoomManager : public Node {
    GDCLASS(RoomManager, Node)

private:
    // The list of every room in the game
    std::vector<RoomInfo> rooms;
    // The room Player is currently in
    int current_room_id;

protected:
    static void _bind_methods();

public:
    RoomManager();
    ~RoomManager();

    void _ready();

    // Creates the empty room list
    void create_room_data();

    void load_room(int room_id);
    void go_to_next_room();
};

}

#endif