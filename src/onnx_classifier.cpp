#include "onnx_classifier.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

ONNXClassifier::ONNXClassifier(const std::string& model_path, const std::string& json_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "ONNXClassifier"),
    memory_info(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault))
{
    loadNormalizationParams(json_path);

    // На Windows путь к модели нужно передавать как std::wstring (по заданию)
    std::wstring w_model_path(model_path.begin(), model_path.end());
    Ort::SessionOptions session_options;

    try {
        session = Ort::Session(env, w_model_path.c_str(), session_options);
    }
    catch (const Ort::Exception& e) {
        // Бросаем исключение, если модель не загружена
        throw std::runtime_error(std::string("Failed to load ONNX model: ") + e.what());
    }
}

// Минимальный JSON парсер (ищет ключ и извлекает массив чисел)
std::vector<float> ONNXClassifier::parseJsonArray(const std::string& json_str, const std::string& key) {
    std::vector<float> result;
    size_t key_pos = json_str.find("\"" + key + "\"");
    if (key_pos == std::string::npos) return result;

    size_t start_bracket = json_str.find('[', key_pos);
    size_t end_bracket = json_str.find(']', start_bracket);
    if (start_bracket == std::string::npos || end_bracket == std::string::npos) return result;

    std::string array_str = json_str.substr(start_bracket + 1, end_bracket - start_bracket - 1);
    std::stringstream ss(array_str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        result.push_back(std::stof(item));
    }
    return result;
}

void ONNXClassifier::loadNormalizationParams(const std::string& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) throw std::runtime_error("Cannot open JSON file");

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_str = buffer.str();

    mean = parseJsonArray(json_str, "mean");
    std_dev = parseJsonArray(json_str, "std");

    if (mean.size() != 6 || std_dev.size() != 6) {
        throw std::runtime_error("Invalid normalization parameters");
    }
}

ClassificationResult ONNXClassifier::classify(const std::array<float, 6>& features) {
    // Нормализуем входные признаки (z-score: (x - mean) / std)
    std::array<float, 6> normalized;
    for (int i = 0; i < 6; ++i) {
        normalized[i] = (features[i] - mean[i]) / std_dev[i];
    }

    // Подготавливаем тензор для ONNX
    const char* input_names[] = { "features" };
    const char* output_names[] = { "class_scores" };
    std::vector<int64_t> input_shape = { 1, 6 };

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, normalized.data(), normalized.size(), input_shape.data(), input_shape.size());

    // Запускаем inference (предсказание)
    auto output_tensors = session.Run(
        Ort::RunOptions{ nullptr }, input_names, &input_tensor, 1, output_names, 1);

    // Получаем сырые результаты (логиты)
    float* out_arr = output_tensors.front().GetTensorMutableData<float>();

    // Применяем Softmax для получения вероятностей 0..1
    std::array<float, 3> scores;
    float max_val = std::max({ out_arr[0], out_arr[1], out_arr[2] });
    float sum_exp = 0.0f;
    for (int i = 0; i < 3; ++i) {
        scores[i] = std::exp(out_arr[i] - max_val);
        sum_exp += scores[i];
    }

    // Формируем финальный результат
    ClassificationResult result;
    result.confidence = -1.0f;
    for (int i = 0; i < 3; ++i) {
        result.scores[i] = scores[i] / sum_exp;
        if (result.scores[i] > result.confidence) {
            result.confidence = result.scores[i];
            result.label = i;
        }
    }
    return result;
}