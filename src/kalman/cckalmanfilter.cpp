#include "cckalmanfilter.h"

// ======================================================================
// Constructor
// ======================================================================
CCKalmanFilter::CCKalmanFilter(const unsigned n_dynamic_parameters,
                               const unsigned n_measurement_parameters,
                               const unsigned n_control_parameters)
    : N_dynamic_parameters(n_dynamic_parameters),
      N_measurement_parameters(n_measurement_parameters),
      N_control_parameters(n_control_parameters),
      Kalman_data_type(CV_64F),
      Noise_covariance_Q(1.0e-5),
      Noise_covariance_R(1.0e-5)
{
    // Initialise matrices and vector used by the Kalman filter

    // --------------------------------------------------
    // Prediction stage ---------------------------------
    // --------------------------------------------------

    // The state vector x
    _x = new cv::Mat(n_dynamic_parameters, 1, Kalman_data_type);

    // The transition matrix
    _F = new cv::Mat(n_dynamic_parameters, n_dynamic_parameters,
                     Kalman_data_type);

    // The control input matrix B
    _B = new cv::Mat(n_dynamic_parameters, n_control_parameters,
                     Kalman_data_type);

    // The control input vector u
    _u = new cv::Mat(n_control_parameters, n_control_parameters,
                     Kalman_data_type);

    // Covariance matrix P
    _P = new cv::Mat(n_dynamic_parameters, n_dynamic_parameters,
                     Kalman_data_type);

    // Process noise covariance Q
    _Q = new cv::Mat(n_dynamic_parameters, n_dynamic_parameters,
                     Kalman_data_type);

    // --------------------------------------------------
    // Updating stage -----------------------------------
    // --------------------------------------------------

    // The measurement vector (values given by sensors)
    _z = new cv::Mat(n_measurement_parameters, 1, Kalman_data_type);

    // Transformation matrix
    _H = new cv::Mat(n_measurement_parameters, n_dynamic_parameters,
                     Kalman_data_type);

    // Measurement noise covariance R
    _R = new cv::Mat(n_measurement_parameters, n_measurement_parameters,
                     Kalman_data_type);

    // Create an instance of the Kalman filter from OpenCV
    Kalman_filter_openCV_pt = new cv::KalmanFilter(n_dynamic_parameters,
                                                   n_measurement_parameters,
                                                   n_control_parameters,
                                                   Kalman_data_type);

}
// ======================================================================
// Destructor
// ======================================================================
CCKalmanFilter::~CCKalmanFilter()
{
    // Free all memory

    // --------------------------------------------------
    // Prediction stage ---------------------------------
    // --------------------------------------------------

    // State vector x
    delete _x;
    _x = 0;
    // Transition matrix F
    delete _F;
    _F = 0;
    // Control input matrix B
    delete _B;
    _B = 0;
    // Control input vector u
    delete _u;
    _u = 0;

    // Covariance matrix P
    delete _P;
    _P = 0;
    // Process noise covariance Q
    delete _Q;
    _Q = 0;

    // --------------------------------------------------
    // Updating stage -----------------------------------
    // --------------------------------------------------
    // Sensor measurements vector
    delete _z;
    _z = 0;
    // Transformation matrix
    delete _H;
    _H = 0;
    // Measurement noise covariance R
    delete _R;
    _R = 0;

    // Delete the OpenCV instance of the Kalman filter
    delete Kalman_filter_openCV_pt;
    Kalman_filter_openCV_pt = 0;

}

// ======================================================================
// Reset all data into the Kalman filter
// ======================================================================
void CCKalmanFilter::reset()
{
    // Initialise the Kalman filter of openCV
    Kalman_filter_openCV_pt->init(N_dynamic_parameters,
                                  N_measurement_parameters,
                                  N_control_parameters,
                                  Kalman_data_type);

    // --------------------------------------------------------------
    // Set pointers to all matrices and vectors in the Kalman filter
    // --------------------------------------------------------------

    // --------------------------------------------------
    // Prediction stage ---------------------------------
    // --------------------------------------------------

    // State vector x
    cv::setIdentity(*_x);
    Kalman_filter_openCV_pt->statePost = *_x;
    // Transition matrix
    cv::setIdentity(*_F);
    Kalman_filter_openCV_pt->transitionMatrix = *_F;
    // Control input matrix B
    cv::setIdentity(*_B);
    Kalman_filter_openCV_pt->controlMatrix = *_B;
    // Control input vector u (zero by default in case no control
    // data is set)
    cv::setIdentity(*_u, cv::Scalar::all(0.0));

    // Covariance matrix P
    cv::setIdentity(*_P);
    Kalman_filter_openCV_pt->errorCovPost = *_P;
    // Process noise covariance Q
    cv::setIdentity(*_Q, cv::Scalar::all(Noise_covariance_Q));
    Kalman_filter_openCV_pt->processNoiseCov = *_Q;

    // --------------------------------------------------
    // Updating stage -----------------------------------
    // --------------------------------------------------
    // Sensor measurements vector
    // -- measurement matrix z_0

    // Transformation matrix
    cv::setIdentity(*_H);
    Kalman_filter_openCV_pt->measurementMatrix = *_H;
    // Measurement noise covariance R
    cv::setIdentity(*_R, cv::Scalar::all(Noise_covariance_R));
    Kalman_filter_openCV_pt->measurementNoiseCov = *_R;

}

// ======================================================================
// Gets a prediction x_{t|t-1} from the current state x_{t-1|t-1}.
// Before calling this method you MAY have set some data in
// the input control vector u, if you have NO DATA for that then
// we set u = 0 such that it has no effect on the prediction
// ======================================================================
cv::Mat CCKalmanFilter::predict()
{
    // Pass the current state to the Kalman filter
    Kalman_filter_openCV_pt->statePost = *_x;
    // Set the transition matrix into the Kalman filter
    Kalman_filter_openCV_pt->transitionMatrix = *_F;
    // Control input matrix B
    Kalman_filter_openCV_pt->controlMatrix = *_B;

    // Covariance matrix P
    Kalman_filter_openCV_pt->errorCovPost = *_P;
    // Process noise covariance Q
    Kalman_filter_openCV_pt->processNoiseCov = *_Q;

    // Predict (using the control vector u that should have been modified
    // before calling this method, otherwise it has no effect since it is
    // set to 0 by default). If no sensor value z is given for the updating
    // stage then we use the same as the predicted one
    //(*_z) = Kalman_filter_openCV_pt->predict(*_u);
    return Kalman_filter_openCV_pt->predict(*_u);

    // Return the predicted value
    //return (*_z);
}

// ======================================================================
// Update the prediction x_{t|t} using the information of the
// sensors z. Before calling this method you MUST set some data
// corresponding to the sensor lectures in the vector z_t, otherwise
// we use the data from the prediction state as the sensor lecture,
// thus the updated value will be the same as the predicted one
// ======================================================================
cv::Mat CCKalmanFilter::update()
{
    // Correction by Kalman filter, pass the measurement data
    return Kalman_filter_openCV_pt->correct(*_z);
}
