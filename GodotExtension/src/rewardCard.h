#ifndef REWARDCARD_H
#define REWARDCARD_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/rect2.hpp>

namespace godot {

// Store the information belonging to one reward
struct RewardData {
    String name;
    String description;
    Ref<Texture2D> icon;
    int status_amount;
    Rect2 icon_region;
};

class RewardCard : public Button {
    GDCLASS(RewardCard, Button);

private:
    // Store references to the labels & image used to display the reward
    Label *rarity_label = nullptr;
    Label *name_label = nullptr;
    Label *description_label = nullptr;
    TextureRect *icon = nullptr;

protected:
    static void _bind_methods();

public:
    void _ready() override;

    bool detect_invalid() const;
    Ref<Texture2D> get_preview_icon() const;

    void display_reward(const String &rarity, const RewardData &reward);
};

}

#endif
