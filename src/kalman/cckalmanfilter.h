#ifndef CCKALMANFILTER_H
#define CCKALMANFILTER_H

// Standard io functions
#include <iostream>

// OpenCV includes for Kalman Filter
#include <opencv2/video/tracking.hpp>

// This class provides a wrapper to the Kalman filter implementation
// of OpenCV
class CCKalmanFilter
{

public:

    // Constructor
    CCKalmanFilter(const unsigned n_dynamic_parameters,
                   const unsigned n_measurement_parameters,
                   const unsigned n_control_parameters = 0);

    // Desctructor
    virtual ~CCKalmanFilter();

    // Reset all data into the Kalman filter (let it ready to work
    // with a new set of data)
    void reset();

    // Initialise Kalman (matrices)
    virtual void initialise(const double dt = 1.0,
                            const double noise_covariance_q = 1.0e-3,
                            const double noise_covariance_r = 0.5);

    // Apply Kalman PREDICT, then UPDATE
    virtual void apply(const double dt = 1.0,
                       bool predict_only = false);

    // Gets a prediction x_{t|t-1} from the current state x_{t-1|t-1}.
    // Before calling this method you may have set some data in
    // the input control vector u, if you have no data for that then
    // we set u = 0 such that it has no effect on the prediction
    cv::Mat predict();

    // Update the prediction x_{t|t} using the information of the
    // sensors z. Before calling this method you MUST set some data
    // corresponding to the sensor lectures in the vector z_t, otherwise
    // we use the data from the prediction state as the sensor lecture,
    // thus the updated value will be the same as the predicted one
    cv::Mat update();

    // Get the number of dynamic parameters
    inline const unsigned n_dynamic_parameters() const
    {return N_dynamic_parameters;}

    // Get the number of measurement parameters
    inline const unsigned n_measurement_parameters() const
    {return N_measurement_parameters;}

    // Get the number of control parameters
    inline const unsigned n_control_parameters() const
    {return N_control_parameters;}

    // Read access for the noise covariance of Q
    inline double noise_covariance_Q() const
    {return Noise_covariance_Q;}

    // Read-write access for the noise covariance of Q
    inline double &noise_covariance_Q()
    {return Noise_covariance_Q;}

    // Read access for the noise covariance of R
    inline double noise_covariance_R() const
    {return Noise_covariance_R;}

    // Read-write access for the noise covariance of R
    inline double &noise_covariance_R()
    {return Noise_covariance_R;}

    // --------------------------------------------------
    // Access functions to the matrices and vectors
    // --------------------------------------------------

    // --------------------------------------------------
    // Prediction stage ---------------------------------
    // --------------------------------------------------

    // State vector x
    inline cv::Mat *x() {return _x;}
    // Read access to the state vector x
    inline double x(const unsigned i) const {return _x->at<double>(i, 0);}
    // Read-write access to the state vector x
    inline double &x(const unsigned i) {return _x->at<double>(i, 0);}

    // Transition matrix F
    inline cv::Mat *F() {return _F;}
    // Read access to the transition matrix F
    inline double F(const unsigned i, const unsigned j) const
     {return _F->at<double>(i, j);}
    // Read-write access to the transition matrix F
    inline double &F(const unsigned i, const unsigned j)
     {return _F->at<double>(i, j);}

    // Control input matrix B
    inline cv::Mat *B() {return _B;}
    // Read access to the control input matrix B
    inline double B(const unsigned i, const unsigned j) const
     {return _B->at<double>(i, j);}
    // Read-write access to the control input matrix B
    inline double &B(const unsigned i, const unsigned j)
     {return _B->at<double>(i, j);}

    // Control input vector u
    inline cv::Mat *u() {return _u;}
    // Read access to the control input vector u
    inline double u(const unsigned i) const {return _u->at<double>(i, 0);}
    // Read-write access to the control input vector u
    inline double &u(const unsigned i) {return _u->at<double>(i, 0);}

    // Covariance matrix P
    inline cv::Mat *P() {return _P;}
    // Read access to the covariance matrix P
    inline double P(const unsigned i, const unsigned j) const
     {return _P->at<double>(i, j);}
    // Read-write access to the covariance matrix P
    inline double &P(const unsigned i, const unsigned j)
     {return _P->at<double>(i, j);}

    // Process noise covariance Q
    inline cv::Mat *Q() {return _Q;}
    // Read access to the process noise matrix Q
    inline double Q(const unsigned i, const unsigned j) const
     {return _Q->at<double>(i, j);}
    // Read-write access to the process noise matrix Q
    inline double &Q(const unsigned i, const unsigned j)
     {return _Q->at<double>(i, j);}

    // State vector x_predicted
    inline cv::Mat *x_predicted() {return _x_predicted;}
    // Read access to the state vector x_predicted
    inline double x_predicted(const unsigned i) const
    {return _x_predicted->at<double>(i, 0);}
    // Read-write access to the state vector x_predicted
    inline double &x_predicted(const unsigned i)
    {return _x_predicted->at<double>(i, 0);}

    // --------------------------------------------------
    // Updating stage -----------------------------------
    // --------------------------------------------------
    // Sensor measurements vector z
    inline cv::Mat *z() {return _z;}
    // Read access to the sensor measurements vector z
    inline double z(const unsigned i) const {return _z->at<double>(i, 0);}
    // Read-write access to the sensor measurements vector z
    inline double &z(const unsigned i) {return _z->at<double>(i, 0);}

    // Transformation matrix
    inline cv::Mat *H() {return _H;}
    // Read access to the transformation matrix H
    inline double H(const unsigned i, const unsigned j) const
     {return _H->at<double>(i, j);}
    // Read-write access to the transformation matrix H
    inline double &H(const unsigned i, const unsigned j)
     {return _H->at<double>(i, j);}

    // Measurement noise covariance RNoise_covariance
    inline cv::Mat *R() {return _R;}
    // Read access to the measurement noise matrix R
    inline double R(const unsigned i, const unsigned j) const
     {return _R->at<double>(i, j);}
    // Read-write access to the measurement noise matrix R
    inline double &R(const unsigned i, const unsigned j)
     {return _R->at<double>(i, j);}

    // State vector x_updated
    inline cv::Mat *x_updated() {return _x_updated;}
    // Read access to the state vector x_updated
    inline double x_updated(const unsigned i) const
    {return _x_updated->at<double>(i, 0);}
    // Read-write access to the state vector x_updated
    inline double &x_updated(const unsigned i)
    {return _x_updated->at<double>(i, 0);}

protected:

    // OpenCV Kalman filter
    cv::KalmanFilter* Kalman_filter_openCV_pt;

    // Number of dynamic parameters or variables, those that change
    // along the computation (ex. position, velocity).
    const unsigned N_dynamic_parameters;
    // Number of parameters given by the sensors
    const unsigned N_measurement_parameters;
    // Number of parameters in the input control
    const unsigned N_control_parameters;

    // Kalman filter data type (all matrices and vectors MUST share the
    // same data type)
    const int Kalman_data_type;

    // The default "covariance of noise" used for the process noise
    // matrix (Q) in the diagonal in case that no entries are specified
    // Its value is small, meaning that the noise in the process is
    // negligible. As you increase the value, the more error in the
    // process you are introducing, thus it gets less weight at the Kalman
    // update stage
    double Noise_covariance_Q;

    // The default "covariance of noise" used for the measurement noise
    // matrix (R) in the diagonal in case that no entries are specified
    // Its value is small, meaning that the noise in the measurements
    // (sensors) is negligible. As you increase the value, the more
    // error in the measurements you are introducing, thus it gets less
    // weight at the Kalman update stage
    double Noise_covariance_R;

    // --------------------------------------------------
    // Prediction stage ---------------------------------
    // --------------------------------------------------

    // State vector x
    cv::Mat *_x;
    // Transition matrix F
    cv::Mat *_F;
    // Control input matrix B
    cv::Mat *_B;
    // Control input vector u
    cv::Mat *_u;

    // Covariance matrix P
    cv::Mat *_P;
    // Process noise covariance Q
    cv::Mat *_Q;

    // Result from prediction
    cv::Mat *_x_predicted;

    // --------------------------------------------------
    // Updating stage -----------------------------------
    // --------------------------------------------------
    // Sensor measurements vector
    cv::Mat *_z;
    // Transformation matrix
    cv::Mat *_H;
    // Measurement noise covariance R
    cv::Mat *_R;

    // Result from updating
    cv::Mat *_x_updated;

};

#endif // CCKALMANFILTER_H
