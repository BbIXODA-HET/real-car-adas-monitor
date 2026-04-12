#pragma once
#include <string>
#include <vector>
#include <stdexcept> // Для исключений (out_of_range, invalid_argument)

/**
 * @struct OBDRecord
 * @brief Хранит одну запись телеметрии автомобиля (одна строка из CSV).
 */
struct OBDRecord {
    float speed_kmh;
    float engine_rpm;
    float throttle_pos;
    float coolant_temp;
    float fuel_level;
    float intake_air_temp;
    int label; // Стиль вождения переведен в число: 0 - SLOW, 1 - NORMAL, 2 - AGGRESSIVE
};

/**
 * @class OBDParser
 * @brief Отвечает за чтение и парсинг CSV файлов с данными телеметрии.
 */
class OBDParser {
public:
    /**
     * @brief Загружает данные из CSV файла.
     * @param[in] filepath Путь к CSV файлу.
     * @return Количество успешно загруженных записей или -1 в случае ошибки.
     */
    int load(const std::string& filepath);
    
    /**
     * @brief Возвращает запись телеметрии по указанному индексу.
     * @param[in] index Индекс запрашиваемой записи.
     * @return Структура OBDRecord с данными.
     * @throw std::out_of_range Если индекс находится вне допустимого диапазона.
     */
    OBDRecord getRecord(int index) const;

    /**
     * @brief Конвертирует текстовую метку стиля вождения в число.
     * @param[in] labelStr Текстовая метка ("SLOW", "NORMAL", "AGGRESSIVE").
     * @return Целое число (0, 1, 2) или -1, если метка неизвестна.
     */
    static int parseLabel(const std::string& labelStr);

private:
    // Вектор для хранения всех прочитанных записей
    std::vector<OBDRecord> records;
};