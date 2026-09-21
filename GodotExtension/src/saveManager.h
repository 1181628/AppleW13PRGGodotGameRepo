#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class PlayerStatus;

class SaveManager : public Node {
    GDCLASS(SaveManager, Node);

protected:
    static void _bind_methods();

public:
    SaveManager();
    ~SaveManager();

    void _process(double delta) override;
    
    void save_game(int current_room_id);
    
    // Allows the title screen to load data without a SaveManager instance
    static bool load_game(PlayerStatus *player_status);
    void save_past_record();
};

}

#endif