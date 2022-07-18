#include "ros/ros.h"
#include "rosbag/bag.h"
#include "sensor_msgs/PointCloud2.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include<pcl/io/ply_io.h>
#include <pcl_conversions/pcl_conversions.h>

int main(int argc, char **argv) {

    ros::init(argc, argv, "ply2bag");
    ros::NodeHandle n;

    std::string foldername = argv[1];
    std::cout << "Foldername: " << foldername << std::endl;

    std::vector<std::string> filenames;
    for (const auto & entry : std::filesystem::directory_iterator(foldername))
        filenames.emplace_back(entry.path());

    std::sort(filenames.begin(), filenames.end());

    std::string bagname = argv[2];
    rosbag::Bag bag;
    bag.open(bagname, rosbag::bagmode::Write);

    for (auto filename : filenames) {
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
        pcl::io::loadPLYFile(filename, *cloud);
        sensor_msgs::PointCloud2 cloud_msg;
        pcl::toROSMsg(*cloud.get(), cloud_msg );
        long timestamp_microsecond_long = std::stol(std::filesystem::path(filename).stem());
        double timestamp_microsecond_double = timestamp_microsecond_long;
        cloud_msg.header.frame_id = "ouster";
        ros::Time timestamp(timestamp_microsecond_double/1000000);
        cloud_msg.header.stamp = timestamp;

        bag.write("/ouster/cloud", timestamp, cloud_msg);
    }
    bag.close();
    return 0;
}
