#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class SaveManager : public Node {
    GDCLASS(SaveManager, Node);

protected:
    static void _bind_methods();

public:
    SaveManager();
    ~SaveManager();

    // Saves PlayerStatus and the room the Player has just entered
    void save_game(int current_room_id);
};

}

#endif