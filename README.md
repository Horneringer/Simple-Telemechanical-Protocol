# Клиент для простейшего телемеханического протокола

Реализация простого клиентского приложения для взаимодействия с сервером с использованием базового телемеханического протокола. Клиент подключается к заранее заданному серверу, отправляет определенные запросы и обрабатывает ответы в соответствии с требованиями протокола.

## Особенности

- Подключение к TCP серверу.
- Отправка запросов для начала и завершения сессий.
- Выполнение запросов общего опроса.
- Управление дискретными сигналами.
- Обработка и вывод в лог дискретных и аналоговых сигналов.

## Использование
1. ### Сборка проекта

```g++ -ggdb -O0  -o teleprot teleprot.cpp main.cpp```

2. ### Запуск клиента

```./client```

## Пример логов

```Connecting to cpptest.08z.ru:12567
Connected

Sending START
Receiving ACK

Sending GI
Receiving ACK

Receiving DIGITAL_POINTS
    PointId= 1, Value= 1, TimeTag = 01.01.2022 00:00:00, Quality = [Valid]
    PointId= 2, Value= 0, TimeTag = 01.01.2022 00:00:00, Quality = [Invalid]
    PointId= 3, Value= 1, TimeTag = 01.01.2022 00:00:00, Quality = [Valid]

Receiving ANALOG_POINTS
    PointId= 1000, Value= 1000.0, TimeTag = 01.01.2022 00:00:00, Quality = [Valid]
    PointId= 1001, Value= 1001.0, TimeTag = 01.01.2022 00:00:00, Quality = [Invalid, Overflow]

Sending DIGITAL_CONTROL
    PointId= 2, Value= 1
Receiving ACK

Receiving DIGITAL_POINTS
    PointId= 2, Value= 1, TimeTag = 01.01.2022 00:00:00, Quality = [Valid, Substituted]

Sending STOP
Receiving ACK

Connection closed```

