#pragma once
#include <vector>

struct PCBProps {
    double k = 0.3;         // Thermal conductivity W/(m·K)
    double rho = 1850.0;    // Density kg/m³
    double cp = 1100.0;     // Specific heat J/(kg·K)
    double thickness = 0.0016; // 1.6mm thickness
};

class HeatSolver {
public:
    HeatSolver(int nodes, PCBProps p);
    
    void step(double dt, double T_vapor, double h_coeff);
    
    double getNodeTemp(int index) const;
    int getNodeCount() const;

private:
    int N;
    double dx;
    PCBProps pcb;
    std::vector<double> T;
};