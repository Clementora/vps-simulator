#include "vps/profile/ReflowProfile.hpp"

void ProfileController::update(float dt, bool is_running, const ReflowProfile& profile, float& out_elevator_y, double& out_h_coeff) {
    if (!is_running) return;

    elapsed_time += dt;

    // Determine current phase based on time
    if (elapsed_time < profile.preheat_time) {
        current_state = ProcessState::Preheat;
        target_elevator_y = 0.3f; 
    } 
    else if (elapsed_time < profile.preheat_time + profile.soak_time) {
        current_state = ProcessState::Soak;
        target_elevator_y = 0.6f; 
    } 
    else if (elapsed_time < profile.preheat_time + profile.soak_time + profile.reflow_time) {
        current_state = ProcessState::Reflow;
        target_elevator_y = 1.0f; 
    } 
    else {
        current_state = ProcessState::Cooling;
        target_elevator_y = 0.0f; 
    }

    // Smooth elevator movement
    if (out_elevator_y < target_elevator_y) out_elevator_y += 0.005f;
    if (out_elevator_y > target_elevator_y) out_elevator_y -= 0.005f;

    // Convective heat transfer coefficients based on position
    out_h_coeff = 5.0; // Air
    if (out_elevator_y > 0.4f && out_elevator_y < 0.8f) out_h_coeff = 150.0; // Boundary
    else if (out_elevator_y >= 0.8f) out_h_coeff = 600.0; // Boiling vapor
}

void ProfileController::reset() {
    current_state = ProcessState::Idle;
    elapsed_time = 0.0f;
    target_elevator_y = 0.0f;
}