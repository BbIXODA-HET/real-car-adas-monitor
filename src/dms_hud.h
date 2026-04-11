#pragma once
#include <opencv2/opencv.hpp>
#include "dms_monitor.h"

class DMSHUD {
public:
    void draw(cv::Mat& display_frame, const cv::Mat& camera_frame, const DriverState& state);
};