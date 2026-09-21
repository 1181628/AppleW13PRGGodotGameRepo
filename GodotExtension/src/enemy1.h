#ifndef ENEMY1_H
#define ENEMY1_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {


class Enemy1 : public CharacterBody2D {
    GDCLASS(Enemy1, CharacterBody2D);

private:
    double gravity = 1000;
    double walkSpeed = 20;
    double attackRange = 35;
    double Health = 4000;

    // all possible enemy1 states
	enum class State {
        NORMAL,
        WALK,
        ATTACK,
        DIE
    };
	// record the enemy1's initial state
	State current_state = State::NORMAL;

	// Records whether the enemy1 has just entered a new state
    bool is_state_new = true;
	void change_state(int new_state);

    void process_normal();
    void process_walk();
    void process_attack();
    void process_die();

    void _turn_direction();

    // Stores the Player's last known global position
    godot::Vector2 playerPosition;
    void match_player_position();   

protected:
    static void _bind_methods();

public:
    Enemy1();
    ~Enemy1();

    // Keep only the Godot callbacks that the class needs.
    void _ready() override;
    void _physics_process(double) override;
    void _on_hurtbox_area_entered(godot::Area2D *area);
    void _on_material_timer_timeout();
};

}

#endif
