#ifndef ENEMY2_H
#define ENEMY2_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {


class Enemy2 : public CharacterBody2D {
    GDCLASS(Enemy2, CharacterBody2D);

private:
    double gravity = 1000;
    double walkSpeed = 40;
    double attackRange = 35;
    double Health = 1000;

    // all possible Enemy2 states
	enum class State {
        NORMAL,
        WALK,
        ATTACK,
        DIE
    };
	// record the Enemy2's initial state
	State current_state = State::NORMAL;

	// Records whether the Enemy2 has just entered a new state
    bool is_state_new = true;
	void change_state(int new_state);

    void process_normal(double delta);
    void process_walk(double delta);
    void process_attack(double delta);
    void process_die(double delta);

    void _turn_direction();

    godot::Vector2 playerPosition;
    void match_player_position();   

protected:
    static void _bind_methods();

public:
    Enemy2();
    ~Enemy2();

    // Keep only the Godot callbacks that the class needs.
    void _ready() override;
    void _process(double delta) override;
    void _on_hurtbox_area_entered(godot::Area2D *area);
    void _on_material_timer_timeout();
};

}

#endif
