#pragma once
#include <mutex>
#include <atomic>
#include "obd_parser.h"
#include "onnx_classifier.h"

/**
 * @struct SharedState
 * @brief Структура для потокобезопасного обмена данными.
 *
 * Используется для передачи данных телеметрии (OBD) в главный поток (UI/Камера)
 * с использованием мьютекса для предотвращения состояния гонки (race condition).
 */
struct SharedState {
    std::mutex mtx;                 // Мьютекс для защиты данных
    std::atomic<bool> running{ true }; // Флаг работы программы

    // Текущие данные для отрисовки
    OBDRecord current_obd;
    ClassificationResult drive_style;

    // Счетчики для финальной статистики
    int alert_drowsy_count = 0;
    int alert_distracted_count = 0;
    int alert_aggressive_count = 0;
};