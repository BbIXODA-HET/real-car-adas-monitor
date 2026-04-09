#include "dashboard.h"
#define _USE_MATH_DEFINES
#include <cmath>

// Функция отрисовки круглого прибора (Спидометр / Тахометр)
void Dashboard::drawGauge(cv::Mat& frame, cv::Point center, int radius, float value, float max_value,
    const std::string& label, const std::string& unit, bool is_rpm) {
    // Темный фон прибора
    cv::circle(frame, center, radius, cv::Scalar(40, 40, 40), cv::FILLED);
    cv::circle(frame, center, radius, cv::Scalar(100, 100, 100), 2); // Ободок

    // Определение цвета дуги
    cv::Scalar arc_color = cv::Scalar(0, 255, 0); // Зеленый по умолчанию
    if (is_rpm) {
        if (value > 4500) arc_color = cv::Scalar(0, 0, 255); // Красный для RPM > 4500
    }
    else {
        if (value > 90) arc_color = cv::Scalar(0, 0, 255);   // Красный для Скорости > 90
    }

    // Углы для отрисовки (от 150 до 390 градусов)
    int start_angle = 150;
    int end_angle = 390;
    int total_angle = end_angle - start_angle;

    // цветная дуга до текущего значения
    float fill_ratio = std::min(value / max_value, 1.0f);
    int current_angle = start_angle + (int)(fill_ratio * total_angle);
    cv::ellipse(frame, center, cv::Size(radius - 10, radius - 10), 0, start_angle, current_angle, arc_color, 8);

    // стрелка
    double rad = current_angle * CV_PI / 180.0;
    int arrow_len = radius - 20;
    cv::Point arrow_end(center.x + arrow_len * cos(rad), center.y + arrow_len * sin(rad));
    cv::line(frame, center, arrow_end, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::circle(frame, center, 5, cv::Scalar(255, 255, 255), cv::FILLED); // Центр стрелки

    // Текст значения и единиц
    std::string val_text = std::to_string((int)value);
    cv::putText(frame, val_text, cv::Point(center.x - 20, center.y + 40),
        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    cv::putText(frame, unit, cv::Point(center.x - 15, center.y + 60),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);
    cv::putText(frame, label, cv::Point(center.x - 30, center.y - 15),
        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
}

// Функция отрисовки горизонтальной полосы
void Dashboard::drawLinearGauge(cv::Mat& frame, cv::Point top_left, int width, int height,
    float value, float max_value, const std::string& label, const std::string& unit) {
    // Рамка и фон
    cv::rectangle(frame, top_left, cv::Point(top_left.x + width, top_left.y + height), cv::Scalar(40, 40, 40), cv::FILLED);
    cv::rectangle(frame, top_left, cv::Point(top_left.x + width, top_left.y + height), cv::Scalar(150, 150, 150), 1);

    // Заполнение полосы
    float fill_ratio = std::min(value / max_value, 1.0f);
    int fill_width = (int)(width * fill_ratio);
    cv::Scalar fill_color = cv::Scalar(255, 200, 0); // Голубой

    if (fill_width > 0) {
        cv::rectangle(frame, top_left, cv::Point(top_left.x + fill_width, top_left.y + height), fill_color, cv::FILLED);
    }

    // Текст
    cv::putText(frame, label, cv::Point(top_left.x, top_left.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

    char val_str[16];
    snprintf(val_str, sizeof(val_str), "%.1f %s", value, unit.c_str());
    cv::putText(frame, val_str, cv::Point(top_left.x + width + 10, top_left.y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

// Отрисовка предупреждений и стиля вождения
void Dashboard::drawWarnings(cv::Mat& frame, const DashboardData& data) {
    // Стиль вождения
    std::string style_text = "STYLE: NORMAL";
    cv::Scalar style_color = cv::Scalar(0, 255, 0); // Зеленый
    if (data.driving_style == 0) {
        style_text = "STYLE: SLOW";
        style_color = cv::Scalar(255, 200, 0); // Голубой (BGR)
    }
    else if (data.driving_style == 2) {
        style_text = "STYLE: AGGRESSIVE";
        style_color = cv::Scalar(0, 0, 255); // Красный
    }
    cv::putText(frame, style_text, cv::Point(30, 470), cv::FONT_HERSHEY_SIMPLEX, 0.8, style_color, 2);

    // Предупреждения (по заданию)
    int y_offset = 410;
    if (data.coolant_temp > 100.0f) {
        cv::putText(frame, "WARNING: HIGH TEMP!", cv::Point(30, y_offset), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
        y_offset += 30;
    }
    if (data.fuel_level < 15.0f) {
        cv::putText(frame, "WARNING: LOW FUEL!", cv::Point(30, y_offset), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 165, 255), 2); // Оранжевый
    }
}

// Главный метод: собирает всю панель
void Dashboard::draw(cv::Mat& frame, const DashboardData& data) {
    // Рисуем полупрозрачный фон для левой половины кадра
    cv::Mat overlay;
    frame.copyTo(overlay);
    cv::rectangle(overlay, cv::Point(0, 0), cv::Point(640, 480), cv::Scalar(15, 15, 15), cv::FILLED);
    cv::addWeighted(overlay, 0.8, frame, 0.2, 0, frame); // 80% непрозрачности

    // Спидометр 
    drawGauge(frame, cv::Point(150, 150), 100, data.speed, 140.0f, "SPEED", "km/h", false);

    // Тахометр 
    drawGauge(frame, cv::Point(400, 150), 100, data.rpm, 6000.0f, "RPM", "", true);

    // Полосы состояния 
    drawLinearGauge(frame, cv::Point(50, 300), 200, 20, data.throttle_pos, 100.0f, "Throttle", "%");
    drawLinearGauge(frame, cv::Point(50, 360), 200, 20, data.coolant_temp, 120.0f, "Coolant", "C");
    drawLinearGauge(frame, cv::Point(350, 300), 200, 20, data.fuel_level, 100.0f, "Fuel", "%");

    // Тексты и предупреждения
    drawWarnings(frame, data);
}