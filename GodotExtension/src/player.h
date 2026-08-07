#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>

namespace godot {

class Player : public CharacterBody2D {
	GDCLASS(Player, CharacterBody2D)

private:
    //
	double gravity = 580.0;

protected:
	static void _bind_methods();

public:
	Player();
	~Player();

    //
	void _ready() override;
	void _process(double delta) override;
};

}

#endif