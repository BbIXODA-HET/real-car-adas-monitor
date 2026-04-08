#pragma once
#include <string>
#include <vector>
#include <array>
#include <onnxruntime_cxx_api.h>

// Структура возвращаемого результата
struct ClassificationResult {
    int label;                  // Предсказанный класс (0, 1, 2)
    float confidence;           // Уверенность (от 0.0 до 1.0)
    std::array<float, 3> scores;// Вероятности всех трех классов
};

class ONNXClassifier {
public:
    // Конструктор принимает пути к модели и файлу с параметрами нормализации
    ONNXClassifier(const std::string& model_path, const std::string& json_path);

    // Главный метод: принимает 6 признаков телеметрии и возвращает предсказание
    ClassificationResult classify(const std::array<float, 6>& features);

private:
    // Вспомогательные методы для чтения JSON (без сторонних библиотек)
    void loadNormalizationParams(const std::string& json_path);
    std::vector<float> parseJsonArray(const std::string& json_str, const std::string& key);

    // Классы ONNX Runtime
    Ort::Env env;
    Ort::Session session{ nullptr };
    Ort::MemoryInfo memory_info;

    // Параметры нормализации
    std::vector<float> mean;
    std::vector<float> std_dev;
};