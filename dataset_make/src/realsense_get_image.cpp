#include <ros/ros.h>
#include <iostream>
#include<algorithm>
#include<fstream>
#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int counters = 0;
string save_path = "/home/wpr/code/dataset/realsense_dateset/pudu_tech2";  //根据自己需要修改
void ImageSave(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD)
{
    string imagergb = "image";
    string imaged = "depth";
    string lie;
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv_bridge::CvImageConstPtr cv_ptrD;
    try
    {
        cv_ptrD = cv_bridge::toCvShare(msgD);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
     
    
    counters++;
 

   ofstream frgb(save_path + "/rgb.txt",ios::app);
   ofstream fdepth(save_path + "/depth.txt",ios::app);

   //显示图像
    cv::namedWindow(imagergb, cv::WINDOW_AUTOSIZE); 
    cv::imshow(imagergb,cv_ptrRGB->image);
    cv::waitKey(1);
    cv::Mat image_rgb = cv_ptrRGB->image;
    string picname_rgb =to_string( cv_ptrRGB->header.stamp.toSec());
    picname_rgb =save_path + "/rgb/" + picname_rgb + ".png"; 
    cout << picname_rgb << endl;
    imwrite(picname_rgb, image_rgb);//保存rgb图片
    lie = to_string( cv_ptrRGB->header.stamp.toSec()); 
    frgb << lie << " " << "rgb/" << lie << ".png" << endl;

    
    //显示图像
    cv::namedWindow(imaged, cv::WINDOW_AUTOSIZE);
    cv::imshow(imaged,cv_ptrD->image);
    cv::waitKey(1);
    
    cv::Mat image_depth = cv_ptrD->image; 
    string picname_depth =to_string( cv_ptrD->header.stamp.toSec()); 
    picname_depth =save_path + "/depth/" + picname_depth + ".png";
    imwrite(picname_depth,  image_depth);//保存rgb图片
    lie = to_string( cv_ptrD->header.stamp.toSec()); 
    fdepth << lie << " " << "depth/" << lie << ".png" << endl; 

    // string picname = to_string(counters);   
    // string cutname = picname.substr(picname.length()-4,4); 
    
    std::cout << "输出第  " << counters << "  幅图像" << std::endl;
    if(counters == 1000) // 假设只保存500幅图像
    {
        frgb.close();
        fdepth.close();
        cout << "保存图片成功。\n\n";
        ros::shutdown();
        return;
    } 
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "realsense_get_image");
    ros::start();    
    ros::NodeHandle nh;
    message_filters::Subscriber<sensor_msgs::Image>rgb_sub(nh, "/camera/color/image_raw", 1);
    message_filters::Subscriber<sensor_msgs::Image>depth_sub(nh,"/camera/depth/image_rect_raw",1);
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer<sync_pol> sync(sync_pol(10), rgb_sub,depth_sub);
    sync.registerCallback(boost::bind(&ImageSave,_1,_2));
 
    
    ros::spin();

    ros::shutdown();

    return 0;
}
