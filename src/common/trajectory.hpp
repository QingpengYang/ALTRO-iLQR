#pragma once

#include <iostream>
#include <vector>

#include "eigentypes.hpp"
#include "common/knotpoint.hpp"
#include "utils/assert.hpp"

namespace altro {

/**
 * @brief Represents a state and control trajectory
 *
 * If the number of states or controls across the trajectory is not constant
 * the associated type parameter can be set to Eigen::Dynamic.
 *
 * @tparam n size of the state vector. Can be Eigen::Dynamic.
 * @tparam m size of the control vector. Can be Eigen::Dynamic.
 * @tparam T floating precision of state and control vectors
 */
template <int n, int m, class T = double>
class Trajectory {
  using StateVector = Vector<n, T>;
  using ControlVector = Vector<m, T>;

 public:
  /**
   * @brief Construct a new Trajectory object of size N
   * 
   * @param N the number of segments in the trajectory. This means there are
   * N+1 state vectors and N control vectors.
   */
  explicit Trajectory(int N) : traj_(N) {}
  explicit Trajectory(int _n, int _m, int N)
      : traj_(N + 1, KnotPoint<n, m, T>(_n, _m)) {}
  explicit Trajectory(std::vector<KnotPoint<n, m, T>> zs) : traj_(zs) {}

  /**
   * @brief Construct a new Trajectory object from state, control and times
   *
   * @param X (N+1,) vector of states
   * @param U (N,) vector of controls
   * @param times (N+1,) vector of times
   */
  Trajectory(std::vector<Vector<n, T>> X, std::vector<Vector<m, T>> U,
             std::vector<float> times) {
    ALTRO_ASSERT(X.size() == U.size() + 1,
                 "Length of control vector must be one less than the length of "
                 "the state trajectory.");
    ALTRO_ASSERT(X.size() == times.size(),
                 "Length of times vector must be equal to the length of the "
                 "state trajectory.");
    int N = U.size();
    traj_.reserve(N + 1);
    for (int k = 0; k < N; ++k) {
      float h = times[k + 1] - times[k];
      traj_.emplace_back(X[k], U[k], times[k], h);
    }
    traj_.emplace_back(X[N], 0 * U[N], times[N], 0.0);
  }

  int NumSegments() const { return traj_.size() - 1; }
  StateVector& State(int k) { return traj_[k].State(); }
  ControlVector& Control(int k) { return traj_[k].Control(); }

  const KnotPoint<n, m, T>& GetKnotPoint(int k) const { return traj_[k]; }
  const StateVector& State(int k) const { return traj_[k].State(); }
  const ControlVector& Control(int k) const { return traj_[k].Control(); }

  int StateDimension(int k) const { return traj_[k].StateDimension(); }
  int ControlDimension(int k) const { return traj_[k].ControlDimension(); }

  float GetTime(int k) const { return traj_[k].GetTime(); }
  float GetStep(int k) const { return traj_[k].GetStep(); }

  /**
   * @brief Check if the times and time steps are consistent
   *
   * @param eps tolerance check for float comparison
   * @return true if t[k+1] - t[k] == h[k] for all k
   */
  bool CheckTimeConsistency(const double eps = 1e-6) {
    for (int k = 0; k < NumSegments(); ++k) {
      float h_calc = GetTime(k + 1) - GetTime(k);
      float h_stored = GetStep(k);
      if (abs(h_stored - h_calc) > eps) {
        std::cout << "k=" << k << " h=" << h_stored << " dt=" << h_calc
                  << std::endl;
        std::cout << "t-" << GetTime(k) << " t+" << GetTime(k + 1)
                  << " h=" << GetStep(k) << std::endl;
        return false;
      }
    }
    return true;
  }

 private:
  std::vector<KnotPoint<n, m, T>> traj_;
};

using TrajectoryXXd = Trajectory<Eigen::Dynamic, Eigen::Dynamic, double>;

}  // namespace altro