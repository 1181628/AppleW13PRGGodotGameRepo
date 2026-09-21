#ifndef PAST_RECORDS_H
#define PAST_RECORDS_H

#include <godot_cpp/classes/control.hpp>

namespace godot {

class PastRecords : public Control {
    GDCLASS(PastRecords, Control);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void display_records();

    void on_return_pressed();
};

}

#endif