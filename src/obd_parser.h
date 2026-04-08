#pragma once
#include <string>
#include <vector>
#include <stdexcept> // Для исключений (out_of_range, invalid_argument)

// Структура, хранящая одну строку из CSV
struct OBDRecord {
    float speed_kmh;
    float engine_rpm;
    float throttle_pos;
    float coolant_temp;
    float fuel_level;
    float intake_air_temp;
    int label; // Стиль вождения переведен в число: 0 - SLOW, 1 - NORMAL, 2 - AGGRESSIVE
};

class OBDParser {
public:
    // Метод загрузки данных из CSV. Возвращает количество загруженных строк или -1 при ошибке
    int load(const std::string& filepath);

    // Получение записи по индексу. Бросает исключение, если индекс неверный
    OBDRecord getRecord(int index) const;

    // Публичный статический метод для конвертации строки "SLOW" в число 0. 
    // Сделан публичным, чтобы его легко было протестировать отдельно
    static int parseLabel(const std::string& labelStr);

private:
    // Вектор для хранения всех прочитанных записей
    std::vector<OBDRecord> records;
};