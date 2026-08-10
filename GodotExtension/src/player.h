#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

class Player : public CharacterBody2D {
	GDCLASS(Player, CharacterBody2D)

private:
	double gravity = 580.0;
    double horizontal_acceleration = 2000.0;
	double maxHorizontalSpeed = 120.0;
	double jumpSpeed = 250.0;

protected:
	static void _bind_methods();

public:
	Player();
	~Player();

	void _ready() override;
	void _physics_process(double delta) override;
	void _update_animation();
};

}

#endif