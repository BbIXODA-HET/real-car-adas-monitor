#pragma once
#include <opencv2/opencv.hpp>
#include "dms_monitor.h"

/**
 * @class DMSHUD
 * @brief Класс для отрисовки интерфейса (HUD) системы мониторинга водителя.
 */
class DMSHUD {
public:
    /**
    * @brief Вставляет изображение с камеры и рисует поверх него рамки, алерты и индикаторы.
    * @param[in,out] display_frame Финальный кадр для отображения (1280x480).
    * @param[in] camera_frame Оригинальный кадр с веб-камеры.
    * @param[in] state Текущее состояние водителя от DMSMonitor.
    */
    void draw(cv::Mat& display_frame, const cv::Mat& camera_frame, const DriverState& state);
};