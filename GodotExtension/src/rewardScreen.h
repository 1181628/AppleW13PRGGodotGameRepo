#ifndef REWARDSCREEN_H
#define REWARDSCREEN_H

#include "rewardCard.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/base_button.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <vector>

namespace godot {

class RewardScreen : public Control {
    GDCLASS(RewardScreen, Control);

private:
    // Store references to the three selectable reward cards
    RewardCard *cards[3] = {};
    // Store references to the screen's buttons and animation player
    BaseButton *reroll_button = nullptr;
    BaseButton *next_floor_button = nullptr;
    AnimationPlayer *animation_player = nullptr;

    // Group the available rewards into common and rare pools
    std::vector<RewardData> reward_pools[2];
    // Store the reward currently displayed on each card
    RewardData displayed_rewards[3];
    // Match each reward pool with its displayed rarity name
    String rarity_names[2] = {
        "COMMON", "RARE"
    };

    StringName entrance_animation = "intro";

    int rerolls = 3;
    // Uses -1 to indicate that no reward has been selected
    int selected_card = -1;

    // Track whether the screen has finished its initial setup
    bool ready = false;
    // Track whether the reward screen is currently open
    bool reward_open = false;
    // Block selection while the entrance animation is playing
    bool entering = false;

    void on_animation_finished(StringName animation_name);

    void create_reward_data();
    void randomise_rewards();

    void on_reroll_pressed();
    void update_reroll_button();
    void on_card_pressed(int index);
    void on_next_floor_pressed();

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void open_reward();

    // Allow other classes to check whether the reward screen is open
    bool is_reward_open() const {
        return reward_open;
    }
};

}

#endif