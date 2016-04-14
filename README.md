# CVFramework
Easy to use, test concepts and parallel computer vision framework based on state-of-the-art third-part software libraries such as:
- OpenCV

## Included examples

### Tracker implementing Kalman filter
- Implemented in C++ and QtCreator (for interface)
- Basic trackers based on pattern window and search window
  - Differences between pattern and search window sub-windows
  - Weighted differences between pattern and search window sub-windows. Weights are based on a normal distribution assuming that the movement of the target follows a normal distribution
  - Other trackers from OpenCV
- OpenCV's implementation of Kalman filter
