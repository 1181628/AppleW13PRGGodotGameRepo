#ifndef PLAYER_STATUS_H
#define PLAYER_STATUS_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot {

class PlayerStatus : public Node {
    GDCLASS(PlayerStatus, Node);

private:
    //

protected:
    static void _bind_methods();

public:
    PlayerStatus();
    ~PlayerStatus();

    // These values continue existing when the Player enters another room.
    int health = 5;
    int maxHealth = 5;

    double maxHorizontalSpeed = 120.0;
    double jumpHeight = 250.0;
    int attackDamage = 100;

    void take_damage(int damage);
    void heal(int healAmount);
};

}

#endif