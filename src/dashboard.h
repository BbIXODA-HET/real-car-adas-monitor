#pragma once
#include <opencv2/opencv.hpp>
#include <string>

/**
 * @struct DashboardData
 * @brief —труктура дл€ передачи всех необходимых данных на приборную панель.
 */
struct DashboardData {
    float speed;
    float rpm;
    float coolant_temp;
    float fuel_level;
    float throttle_pos;
    int driving_style; // 0-SLOW, 1-NORMAL, 2-AGGRESSIVE
};

/**
 * @class Dashboard
 * @brief  ласс дл€ отрисовки графической приборной панели с помощью OpenCV.
 */
class Dashboard {
public:
    /**
     * @brief –исует всю приборную панель на левой половине кадра.
     * @param[in,out] frame  адр OpenCV, на котором будет производитьс€ отрисовка.
     * @param[in] data —труктура с текущими значени€ми телеметрии.
     */
    void draw(cv::Mat& frame, const DashboardData& data);

private:
    // ѕриватные методы дл€ отрисовки конкретных приборов
    void drawGauge(cv::Mat& frame, cv::Point center, int radius, float value, float max_value,
        const std::string& label, const std::string& unit, bool is_rpm);

    void drawLinearGauge(cv::Mat& frame, cv::Point top_left, int width, int height,
        float value, float max_value, const std::string& label, const std::string& unit);

    void drawWarnings(cv::Mat& frame, const DashboardData& data);
};