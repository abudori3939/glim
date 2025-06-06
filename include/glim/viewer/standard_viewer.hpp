#pragma once

#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <boost/weak_ptr.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <glim/util/extension_module.hpp>

#include <unordered_set>
#include <glim/mapping/global_mapping.hpp>
#include <glim/mapping/async_global_mapping.hpp>
#include <glim/viewer/interactive_viewer.hpp>

namespace guik {
  class ProgressModal;
  class ProgressInterface;
}   // namespace guik

namespace spdlog {
class logger;
}

namespace gtsam {
class NonlinearFactor;
}

namespace glim {

class TrajectoryManager;
struct EstimationFrame;

class StandardViewer : public ExtensionModule {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  StandardViewer();
  virtual ~StandardViewer();

  virtual bool ok() const override;

  void invoke(const std::function<void()>& task);

private:
  Eigen::Isometry3f resolve_pose(const std::shared_ptr<const EstimationFrame>& frame);

  void set_callbacks();
  void viewer_loop();

  bool drawable_filter(const std::string& name);
  void drawable_selection();

private:
  void main_menu();
  std::string init_map_path;
  std::unique_ptr<AsyncGlobalMapping> async_global_mapping;
  std::unordered_set<std::string> imported_shared_libs;
  std::unique_ptr<guik::ProgressModal> progress_modal;
  std::shared_ptr<GlobalMapping> load_map(guik::ProgressInterface& progress, const std::string& path, std::shared_ptr<GlobalMapping> global_mapping);

  std::atomic_bool viewer_started;
  std::atomic_bool request_to_terminate;
  std::atomic_bool kill_switch;
  std::thread thread;

  bool enable_partial_rendering;
  int partial_rendering_budget;

  bool track;
  bool show_current_coord;
  bool show_current_points;
  int current_color_mode;

  bool show_odometry_scans;
  bool show_odometry_keyframes;
  bool show_odometry_factors;

  bool show_submaps;
  bool show_factors;

  bool show_odometry_status;
  int last_id;
  int last_num_points;
  std::pair<double, double> last_point_stamps;
  Eigen::Vector3d last_imu_vel;
  Eigen::Matrix<double, 6, 1> last_imu_bias;
  double last_median_distance;
  std::vector<double> last_voxel_resolutions;

  using FactorLine = std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector4f, Eigen::Vector4f>;
  using FactorLineGetter = std::function<std::optional<FactorLine>(const gtsam::NonlinearFactor*)>;
  std::vector<std::pair<boost::weak_ptr<gtsam::NonlinearFactor>, FactorLineGetter>> odometry_factor_lines;
  std::unordered_map<std::uint64_t, Eigen::Isometry3f> odometry_poses;

  bool show_mapping_tools;
  float min_overlap;

  double point_size;
  bool point_size_metric;
  bool point_shape_circle;

  int z_range_mode;
  Eigen::Vector2f z_range;
  Eigen::Vector2f auto_z_range;
  double last_submap_z;
  double points_alpha;
  double factors_alpha;

  std::unique_ptr<TrajectoryManager> trajectory;
  std::vector<Eigen::Isometry3f> submap_keyframes;

  std::vector<std::pair<int, int>> global_between_factors;

  std::mutex invoke_queue_mutex;
  std::vector<std::function<void()>> invoke_queue;

  // Logging
  std::shared_ptr<spdlog::logger> logger;
};
}  // namespace glim
