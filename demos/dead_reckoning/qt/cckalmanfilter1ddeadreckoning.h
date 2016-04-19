#ifndef CCKALMANFILTER1DDEADRECKONING_H
#define CCKALMANFILTER1DDEADRECKONING_H

#include "../../../src/kalman/cckalmanfilter.h"

// This class extends the CCKalmanFilter class to give an specific solution
// to the dead reckoning problem when having three inputs from sensors:
// [1] Position, not always present (probably given by a GPS)
// [2] Velocity, always present (given by some sensor that returns velocity)
// [3] Direction, always present (obtained from velocity or from a sensor that
//                gives the direction)
class CCKalmanFilter1DDeadReckoning : public CCKalmanFilter
{
public:

    // Constructor
    CCKalmanFilter1DDeadReckoning(const unsigned n_dynamic_parameters,
                                  const unsigned n_measurement_parameters,
                                  const unsigned n_control_parameters = 0);

    // Desctructor
    ~CCKalmanFilter1DDeadReckoning();

    // Initialise Kalman (matrices)
    virtual void initialise(const double dt = 1.0,
                            const double noise_covariance_q = 1.0e-3,
                            const double noise_covariance_r = 0.5);

    // Apply Kalman PREDICT, then UPDATE
    virtual void apply(const double dt = 1.0,
                       bool predict_only = false);

};

#endif // CCKALMANFILTER1DDEADRECKONING_H
