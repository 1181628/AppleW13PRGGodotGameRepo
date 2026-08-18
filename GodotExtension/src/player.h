#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/animation_player.hpp>

namespace godot {

class Player : public CharacterBody2D {
	GDCLASS(Player, CharacterBody2D);

private:
	double gravity = 580.0;
    double horizontal_acceleration = 2000.0;
	double maxHorizontalSpeed = 120.0;
	double jumpSpeed = 210.0;

	// all possible player states
	enum class State {
        NORMAL,
        DASH,
        ATTACK,
        ATTACK_UP,
        ATTACK_DOWN
    };
	// record the player's initial state
	State current_state = State::NORMAL;

	// Records whether the player has just entered a new state
    bool is_state_new = true;

	void change_state(int new_state);

	void process_normal(double delta);
    void process_dash(double delta);
    void process_attack(double delta);
    void process_attack_up(double delta);
    void process_attack_down(double delta);

	// Changes the player's animation
    void _update_animation();

protected:
	static void _bind_methods();

public:
	Player();
	~Player();

	void _ready() override;
	void _physics_process(double delta) override;
};

}

#endif