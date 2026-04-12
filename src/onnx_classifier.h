#pragma once
#include <string>
#include <vector>
#include <array>
#include <onnxruntime_cxx_api.h>

/**
 * @struct ClassificationResult
 * @brief Хранит результат классификации стиля вождения.
 */
struct ClassificationResult {
    int label;                  // Предсказанный класс (0, 1, 2)
    float confidence;           // Уверенность (от 0.0 до 1.0)
    std::array<float, 3> scores;// Вероятности всех трех классов
};

/**
 * @class ONNXClassifier
 * @brief Класс для загрузки ONNX модели и классификации данных телеметрии.
 */
class ONNXClassifier {
public:

    /**
     * @brief Конструктор, загружающий ONNX модель и параметры нормализации.
     * @param[in] model_path Путь к файлу .onnx.
     * @param[in] json_path Путь к файлу .json с параметрами.
     * @throw std::runtime_error Если не удалось загрузить модель или JSON.
     */
    ONNXClassifier(const std::string& model_path, const std::string& json_path);
    
    /**
     * @brief Классифицирует один набор признаков телеметрии.
     * @param[in] features Массив из 6 float-значений (скорость, обороты и т.д.).
     * @return Структура ClassificationResult с предсказанным классом и уверенностью.
     */
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