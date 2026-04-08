import csv
import random
import os


def generate_data():
    labels = ["SLOW", "NORMAL", "AGGRESSIVE"]

    # Создаем папку data, если её вдруг нет, чтобы не было ошибки сохранения
    os.makedirs('data', exist_ok=True)

    # Открываем файл для записи (w). newline='' нужен, чтобы в Windows не было пустых строк между записями
    with open('data/obd_data.csv', 'w', newline='') as f:
        writer = csv.writer(f)

        # Пишем первую строку - заголовок таблицы
        writer.writerow(
            ["speed_kmh", "engine_rpm", "throttle_pos", "coolant_temp", "fuel_level", "intake_air_temp", "label"])

        # Генерируем 5000 строк с реалистичными данными
        for _ in range(5000):
            label = random.choice(labels)

            # Задаем логичные значения скорости и оборотов в зависимости от стиля вождения
            if label == "SLOW":
                speed, rpm = random.uniform(0, 40), random.uniform(800, 2000)
            elif label == "NORMAL":
                speed, rpm = random.uniform(40, 90), random.uniform(1500, 3000)
            else:  # AGGRESSIVE
                speed, rpm = random.uniform(90, 160), random.uniform(3000, 6000)

            # Записываем сгенерированную строку в файл (округляем до 1 знака после запятой)
            writer.writerow([
                round(speed, 1),
                round(rpm, 1),
                round(random.uniform(0, 100), 1),  # дроссель 0-100%
                round(random.uniform(70, 105), 1),  # температура антифриза
                round(random.uniform(5, 100), 1),  # топливо
                round(random.uniform(10, 40), 1),  # температура воздуха
                label
            ])


if __name__ == "__main__":
    generate_data()
    print("Dataset created at data/obd_data.csv")