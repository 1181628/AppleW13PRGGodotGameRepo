#ifndef ENEMY1_H
#define ENEMY1_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/timer.hpp>

namespace godot {


class Enemy1 : public CharacterBody2D {
    GDCLASS(Enemy1, CharacterBody2D);

private:
    // all possible enemy1 states
	enum class State {
        NORMAL,
        ATTACK,
    };
	// record the enemy1's initial state
	State current_state = State::NORMAL;

	// Records whether the enemy1 has just entered a new state
    bool is_state_new = true;
	void change_state(int new_state);

    void process_normal(double delta);
    void process_attack(double delta);

protected:
    static void _bind_methods();

public:
    Enemy1();
    ~Enemy1();

    // Keep only the Godot callbacks that the class needs.
    void _ready() override;
    void _process(double delta) override;
    void _on_hurtbox_area_entered(godot::Area2D *area);
    void _on_material_timer_timeout();

    // Declare custom functions here.
    // void take_damage(int damage);
    // void attack();
    // bool is_alive() const;
};

}

#endif
