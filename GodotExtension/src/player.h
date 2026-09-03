#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/area2d.hpp>

namespace godot {

class Player : public CharacterBody2D {
	GDCLASS(Player, CharacterBody2D);

private:
	double gravity = 580.0;
    double horizontalAcceleration = 2000.0;
	double maxHorizontalSpeed = 120.0;
	double jumpSpeed = 250.0;
	bool isInvincible = false;
	String hurtDirection;

	// all possible player states
	enum class State {
        NORMAL,
        DASH,
        ATTACK,
        ATTACK_UP,
        ATTACK_DOWN,
		HURT,
		DIE
    };
	// record the player's initial state
	State current_state = State::NORMAL;

	// Records whether the player has just entered a new state
    bool is_state_new = true;
	void change_state(int new_state);

	void apply_gravity_movement(double delta);
	void process_normal(double delta);
    void process_dash(double delta);
    void process_attack(double delta);
    void process_attack_up(double delta);
    void process_attack_down(double delta);
	void process_hurt(double delta);
	void process_die(double delta);

	// Changes the player's animation
    void _update_animation();

	void start_invincibility();
	void _on_flash_timer_timeout();
	void _on_invincibility_timer_timeout();

	void _turn_direction();

protected:
	static void _bind_methods();

public:
	Player();
	~Player();

	void _ready() override;
	void _physics_process(double delta) override;
	void _on_hurtbox_area_entered(godot::Area2D *area);
	void _on_attack1_area_entered(godot::Area2D *area);
};

}

#endif