#ifndef ENEMY3_H
#define ENEMY3_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {


class Enemy3 : public CharacterBody2D {
    GDCLASS(Enemy3, CharacterBody2D);

private:
    double gravity = 1000;
    double walkSpeed = 40;
    double attackRange = 25;
    double Health = 1000;

    // all possible enemy3 states
	enum class State {
        NORMAL,
        WALK,
        ATTACK,
        DIE
    };
	// record the enemy3's initial state
	State current_state = State::NORMAL;

	// Records whether the enemy3 has just entered a new state
    bool is_state_new = true;
	void change_state(int new_state);

    void process_normal();
    void process_walk();
    void process_attack();
    void process_die();

    void _turn_direction();

    godot::Vector2 playerPosition;
    void match_player_position();   

protected:
    static void _bind_methods();

public:
    Enemy3();
    ~Enemy3();

    // Keep only the Godot callbacks that the class needs.
    void _ready() override;
    void _physics_process(double) override;
    void _on_hurtbox_area_entered(godot::Area2D *area);
    void _on_material_timer_timeout();
};

}

#endif
