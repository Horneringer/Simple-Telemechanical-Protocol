#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>

//форматы кадра, представленные беззнаковым числом 1 байт
enum FormatType : uint8_t
{
	DigitalPoints		 = 1,
	AnalogPoints		 = 2,
	DigitalControl		 = 3,
	Start				 = 4,
	Stop				 = 5,
	GeneralInterrogation = 6,
	Ack					 = 7,
	Nack				 = 8
};

//качество, беззнаковое число 1 байт
typedef struct Quality
{
	uint8_t Valid;		 // значение достоверно
	uint8_t Substituted; // значение установлено вручную
	uint8_t Overflow;	 // значение переполнено
} m_quality;

//структура дискретного сигнала
typedef struct DigitalPoint
{
	uint32_t  pointId; // номер сигнала
	uint8_t	  value;   // значение сигнала
	int64_t	  timeTag; // временная метка
	m_quality quality; // качество сигнала
} m_digital_point;

//структура аналогового сигнала
typedef struct AnalogPoint
{
	uint32_t  pointId; // номер сигнала
	float	  value;   // значение сигнала
	int64_t	  timeTag; // временная метка
	m_quality quality; // качество сигнала
} m_analog_point;

// причина передачи сигнала
enum TransmissionType
{
	Interrogation = 1, // общий опрос
	Spontaneous	  = 2  // спорадическая передача
};

/*!
 * @brief Класс коиента простого телемеханического протокола
 */
class TeleProtClient
{
public:

	TeleProtClient();
	~TeleProtClient();

	/*!
	 * @brief запуск клиента
	 * @return дескриптор клиента
	 */
	int run();

	/*!
	 * @brief метод упаковки данных в кадр
	 * @param format - формат кадра
	 * @param payload - полезная нагрузка
	 * @return - пакет данных
	 */
	std::vector<uint8_t> packFrame(FormatType				   format,
								   const std::vector<uint8_t>& payload);
	/*!
	 * @brief отправка кадра на сервер
	 * @param cfd - дескриптор клиента
	 * @param format - формат кадра
	 * @param payload - полезная нагрузка
	 */
	void				 sendFrame(int						   cfd,
								   FormatType				   format,
								   const std::vector<uint8_t>& payload = {});

	/*!
	 * @brief - отправка запроса Start
	 */
	void sendStart();

	/*!
	 * @brief - отправка запроса Stop
	 */
	void sendStop();

	/*!
	 * @brief получение и обработка ответа от сервера
	 */
	void receiveData();
	/*!
	 * @brief отправка общего опроса
	 */
	void sendGI();

	/*!
	 * @brief отправка запроса DigitalControl
	 * @param pointId номер сигнала
	 * @param value - значение сигнала
	 * @param payload - полезная нагрузка
	 */
	void sendDigitalControl(uint32_t			 pointId,
							uint8_t				 value,
							std::vector<uint8_t> payload);

	/*! @brief получить дескриптор клиента
	 *  @return дескриптор
	 */
	int getCfd();

private:

	//адрес сервера
	const char* ip = "cpptest.08z.ru";

	//порт
	const int port = 12567;

	//дескриптор клиента
	int cfd;
};