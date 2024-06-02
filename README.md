# livox_repub
官方livox_driver驱动livox雷达发出的点云topic有两种，一种是大疆览沃定制的格式CustomMsg格式，另一种是将CustomMsg格式 转换过的pointcloud2格式，参见 Livox雷达驱动程序发布点云格式CustomMsg、PointCloud2、pcl::PointXYZI、pcl::PointXYZINormal解析 现在将转换这部分的代码提取出来，方便随时使用
## 创建ros功能包:
```cpp
	mkdir -p livox_repub/src
	cd livox_repub/src
	catkin_init_workspace 

	git clone https://github.com/CharlieV5/livox_repub.git
	cd ..
	catkin_make
```
## 运行：
```
        roslaunch livox_repub livox_repub.launch
```
## 发布CustomMsg的话题/livox/lidar
可连接Livox激光雷达，通过livox-driver发布；或通过录制好的bag，用rosbag play发布

注意：这个包是要先订阅CustomMsg的话题/livox/lidar，然后发布PointCloud2格式的"/livox_pc2"话题，所以不论是实时驱动livox-driver还是通过bag包发布/livox/lidar，都需要确保有/livox/lidar才能有转换结果

转换后的PointCloud2点云可以通过rviz显示
终端输入：
```
        rviz
```
Fixed Frame设置为"livox_frame"，点云设置为 "/livox_pc2"

## 转为PCL pcd文件：
启动一个终端，输入：
```
	rosrun pcl_ros bag_to_pcd xxx.bag /livox_pc2 pcd_dir
```



