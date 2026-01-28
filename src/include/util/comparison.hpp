#ifndef N_BODY_SIMULATION_COMPARISON_H
#define N_BODY_SIMULATION_COMPARISON_H

#include "models/Body.hpp"
#include <vector>
#include <glm/glm.hpp>

struct QualityMetrics {
    double maxDistanceError;
    double meanSquaredError;
    bool isDivergent;
};

// Calculates the error between a test run and the baseline (sequential) run
inline QualityMetrics calculateQualityMetrics(
    const std::vector<Body>& baseline,
    const std::vector<Body>& target,
    double toleranceThreshold = 1e-3)
{
    if (baseline.size() != target.size()) {
        return { -1.0, -1.0, true }; // Size mismatch implies immediate failure
    }

    double totalErrorSq = 0.0;
    double maxDist = 0.0;

    for (size_t i = 0; i < baseline.size(); ++i) {
        const float dist = glm::distance(baseline[i].position, target[i].position);
        if (dist > maxDist) maxDist = dist;
        totalErrorSq += (dist * dist);
    }

    const double mse = totalErrorSq / static_cast<double>(baseline.size());

    return { maxDist, mse, mse > toleranceThreshold };
}

#endif //N_BODY_SIMULATION_COMPARISON_H