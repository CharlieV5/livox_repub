#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include "livox_ros_driver/CustomMsg.h"
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

struct EIGEN_ALIGN16 PointXYZIT {
    PCL_ADD_POINT4D;
    float intensity;
    float time_offset;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
POINT_CLOUD_REGISTER_POINT_STRUCT(PointXYZIT,
                                  (float, x, x)
                                  (float, y, y)
                                  (float, z, z)
                                  (float, intensity, intensity)
                                  (float, time_offset, time_offset)
                                  )

typedef pcl::PointCloud<PointXYZIT> PointCloudXYZIT;


typedef PointXYZIT PointType;
typedef pcl::PointCloud<PointType> PointCloudXYZI;

ros::Publisher pub_pcl_out0, pub_pcl_out1;
uint64_t TO_MERGE_CNT = 1; 
constexpr bool b_dbg_line = false;
std::vector<livox_ros_driver::CustomMsgConstPtr> livox_data;

void LivoxMsgCbk1(const livox_ros_driver::CustomMsgConstPtr& livox_msg_in)
{
  livox_data.push_back(livox_msg_in);
  if (livox_data.size() < TO_MERGE_CNT) return;

  unsigned long timebase_ns = livox_data[0]->timebase;

  pcl::PointCloud<PointType> pcl_in;

  for (size_t j = 0; j < livox_data.size(); j++)
  {
    auto& livox_msg = livox_data[j];
    auto time_end = livox_msg->points.back().offset_time;
    for (unsigned int i = 0; i < livox_msg->point_num; ++i)
    {
      PointType pt;
      pt.x = livox_msg->points[i].x;
      pt.y = livox_msg->points[i].y;
      pt.z = livox_msg->points[i].z;

      // The integer part is intensity(0~255) and the decimal part is times offset in second
      // With 10Hz point frequency, the original offset_time is between 0 and 0.1s
      //pt.intensity = float(livox_msg->points[i].offset_time*1e-9 + livox_msg->points[i].reflectivity);

      pt.intensity = float(livox_msg->points[i].reflectivity);
      pt.time_offset = (livox_msg->points[i].offset_time)*1e-9;

      pcl_in.push_back(pt);
    }
  }

  
  ros::Time timestamp;
  timestamp.fromNSec(timebase_ns);

  sensor_msgs::PointCloud2 pcl_ros_msg;
  pcl::toROSMsg(pcl_in, pcl_ros_msg);
  pcl_ros_msg.header.stamp.fromNSec(timebase_ns);
  pcl_ros_msg.header.frame_id = "livox_frame";
  pub_pcl_out1.publish(pcl_ros_msg);
  livox_data.clear();
  printf("%.6lf\r", timebase_ns*1e-9);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "livox_repub");
  ros::NodeHandle nh;

  ROS_INFO("start livox_repub");

  ros::Subscriber sub_livox_msg1 = nh.subscribe<livox_ros_driver::CustomMsg>(
      "/livox/lidar", 10, LivoxMsgCbk1);
  pub_pcl_out1 = nh.advertise<sensor_msgs::PointCloud2>("/livox_pc2", 10);

  ros::spin();
}


