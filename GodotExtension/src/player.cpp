//read player.h
#include "player.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Player::Player() {
	// 
	gravity = 580.0;
}

Player::~Player() {
}

void Player::_bind_methods() {
}

void Player::_ready() {
	//pass
}

void Player::_process(double delta) {
	// 
	Vector2 velocity = get_velocity();

	// 
	velocity.y = 10.0;

	//
	set_velocity(velocity);

	//
	move_and_slide();
}