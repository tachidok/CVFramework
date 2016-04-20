#include "cckalmanfilter1ddeadreckoning.h"

// ======================================================================
// Constructor
// ======================================================================
CCKalmanFilter1DDeadReckoning::CCKalmanFilter1DDeadReckoning(
        const unsigned n_dynamic_parameters,
        const unsigned n_measurement_parameters,
        const unsigned n_control_parameters)
    : CCKalmanFilter(n_dynamic_parameters,
                     n_measurement_parameters,
                     n_control_parameters)
{
    // Use information from position sensor by default
    Use_position_sensor = true;
}

// ======================================================================
// Destructor
// ======================================================================
CCKalmanFilter1DDeadReckoning::~CCKalmanFilter1DDeadReckoning()
{

}

void CCKalmanFilter1DDeadReckoning::initialise(const double dt,
                                               const double noise_covariance_q,
                                               const double noise_covariance_r)
{
    // -------------------------------------------------------
    // Before reset set default covariances for process noise
    // and measurement noise
    Noise_covariance_Q = noise_covariance_q;
    Noise_covariance_R = noise_covariance_r;

    // Reset
    reset();

    //std::cerr << "State vector (init)" << std::endl;
    //std::cerr << (*Kalman_filter_pt[0]->x()) << std::endl;

    // Initialise covariance matrix P
    P(0, 0) = dt*dt*dt*dt/4.0;
    P(0, 1) = dt*dt*dt/3.0;
    P(1, 0) = dt*dt*dt/3.0;
    P(1, 1) = dt*dt;

    // Initilaise process noise covariance Q
    Q(0, 0) = dt*dt*dt*dt/4.0 * noise_covariance_q;
    Q(0, 1) = dt*dt*dt/3.0 * noise_covariance_q;
    Q(1, 0) = dt*dt*dt/3.0 * noise_covariance_q;
    Q(1, 1) = dt*dt * noise_covariance_q;

    // Initialise transformation matrix H
    H(0, 0) = 1.0;  H(0, 1) = 0.0;
    H(1, 0) = 0.0;  H(1, 1) = 1.0;

    // Set measurement (sensors) noise covariance
    R(0, 0) = noise_covariance_r;
}

void CCKalmanFilter1DDeadReckoning::apply(const double dt,
                                          bool predict_only)
{
    // To compute dT
#if 0
    const double precTick = ticks;
    const double ticks = (double) cv::getTickCount();
    const double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
    F->at<double>(0, 1) =  dT;
#endif // #if 0
    // -------------------------------------------------------
    // PREDICTION stage
    // -------------------------------------------------------
    // Prepara data and do prediction
    // -------------------------------------------------------

    // -------------------------------------------------------
    // Set the transition matrix
    F(0, 0) = 1.0;
    F(0, 1) = dt;
    F(1, 0) = 0.0;
    F(1, 1) = 1.0;

    // Set control matrix
    B(0, 0) = dt*dt/2.0;
    B(1, 0) = dt;

    // Set input control parameters
    // Acceleration
    const double a_x = 0.05; // pixels per frame
    //const double a_x = 1.0; // pixels per frame
    u(0) = a_x;

    // -------------------------------------------------------
    // Do prediction (Kalman filter)
    // -------------------------------------------------------
    cv::Mat prediction = predict();
    for (unsigned i = 0; i < N_dynamic_parameters; i++)
    {
        x_predicted(i) = prediction.at<double>(i, 0);
    }

    // We can use this prediction in case we have no input
    // from the sensor

    // Only do prediction step, do not update using sensores measurements
    if (predict_only)
    {
        return;
    }

    // Show we use the data provided by the position sensor?
    if (Use_position_sensor)
    {
        H(0, 0) = 1.0;
    }
    else
    {
        H(0, 0) = 0.0;
    }

    // -------------------------------------------------
    // UPDATE stage
    // -------------------------------------------------
    // Use the sensor input to update the predicted value
    // -------------------------------------------------
    // Do update (Kalman filter)
    cv::Mat updating = update();
    for (unsigned i = 0; i < N_measurement_parameters; i++)
    {
        x_updated(i) = updating.at<double>(i, 0);
    }

}
