#include "vps/physics/HeatSolver.hpp"

HeatSolver::HeatSolver(int nodes, PCBProps p)
    : N(nodes), pcb(p), T(nodes, 25.0) {
    dx = pcb.thickness / (nodes - 1);
}

void HeatSolver::step(double dt, double T_vapor, double h_coeff) {
    std::vector<double> T_next = T;
    double alpha = pcb.k / (pcb.rho * pcb.cp);
    double r = alpha * dt / (dx * dx);

    // Conduction through internal nodes
    for (int i = 1; i < N - 1; ++i) {
        T_next[i] = T[i] + r * (T[i + 1] - 2.0 * T[i] + T[i - 1]);
    }

    // Convection at boundary nodes
    double q_top = h_coeff * (T_vapor - T[0]);
    double q_bot = h_coeff * (T_vapor - T[N - 1]);

    T_next[0] += (2.0 * r * (T[1] - T[0])) + (2.0 * dt * q_top / (pcb.rho * pcb.cp * dx));
    T_next[N - 1] += (2.0 * r * (T[N - 2] - T[N - 1])) + (2.0 * dt * q_bot / (pcb.rho * pcb.cp * dx));

    T = T_next;
}

double HeatSolver::getNodeTemp(int index) const { return T[index]; }
int HeatSolver::getNodeCount() const { return N; }