# CVFramework
Easy to use, test concepts and perform parallel computer vision framework based on state-of-the-art third-part software libraries such as:
- OpenCV


## Included demos

Some of the examples are developed under QtCreated for a grapich interface.

### Tracker and Kalman filter
- Implemented in C++ and QtCreator.
- Basic trackers based on pattern window and search window
  - Differences between pattern and search window sub-windows
  - Weighted differences between pattern and search window sub-windows. Weights are based on a normal distribution assuming that the movement of the target follows a normal distribution.
  - Tracker from OpenCV (some of them implementing machine learning).
- Kalman filter as wrapper of OpenCV's Kalman filter implementation. We use inheritance to produce specialised versions of the Kalman filter applied to specific problems.
- 
### Dead reckoning
- Implemented in C++ and QtCreator.
- We use our Kalman filter classes to follow an object moving in different trajectories in a 2D space.
