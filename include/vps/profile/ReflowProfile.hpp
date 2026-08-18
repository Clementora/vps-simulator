#pragma once

enum class ProcessState {
    Idle,
    Preheat,
    Soak,
    Reflow,
    Cooling
};

struct ReflowProfile {
    float preheat_time = 30.0f;  // Seconds
    float soak_time = 60.0f;     // Seconds
    float reflow_time = 45.0f;   // Seconds
    float vapor_temp = 230.0f;   // Galden boiling point
};

class ProfileController {
public:
    void update(float dt, bool is_running, const ReflowProfile& profile, float& out_elevator_y, double& out_h_coeff);
    void reset();

    ProcessState getState() const { return current_state; }
    float getElapsedTime() const { return elapsed_time; }

private:
    ProcessState current_state = ProcessState::Idle;
    float elapsed_time = 0.0f;
    float target_elevator_y = 0.0f;
};