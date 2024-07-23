#include "teleprot.hpp"

TeleProtClient::TeleProtClient() {}

TeleProtClient::~TeleProtClient() {}

int TeleProtClient::run()
{
	std::cout << "Connecting to " << ip << ":" << port;
	int cfd;

	//структура сокета
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(port);

	//преобразование адреса
	if (inet_pton(AF_INET, ip, &addr.sin_addr) < 0)
	{
		std::cerr << "Socket creation error";
		return -1;
	}

	//подключение к серверу
	if (connect(cfd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Connection error";
		return -1;
	}

	std::cout << "Connected"
			  << "\n";

	return cfd;
}

std::vector<uint8_t> TeleProtClient::packFrame(
  FormatType format, const std::vector<uint8_t>& payload)
{
	std::vector<uint8_t> result;

	//длина кадра
	uint32_t length = 4 + 1 + payload.size();

	//изменение размера вектора на длину кадра
	result.resize(length);

	//запись длины кадра в начало
	memcpy(result.data(), &length, 4);

	//запись значения формата в 5 байт
	result.at(4) = format;

	//если есть полезная нагрузка
	if (!payload.empty())
		//запись значения полезной нагрузки, начиная с 6го байта
		memcpy(result.data() + 5, &payload, payload.size());

	return result;
}

void TeleProtClient::sendFrame(int						   cfd,
							   FormatType				   format,
							   const std::vector<uint8_t>& payload)
{
	//создание кадра
	std::vector<uint8_t> frame = packFrame(format, payload);

	//запись в сокет данных и отправка
	send(cfd, frame.data(), frame.size(), 0);
}

void TeleProtClient::sendStart()
{
	sendFrame(cfd, FormatType::Start);
	std::cout << "Sending START";
	receiveData();
}

void TeleProtClient::sendStop()
{
	sendFrame(cfd, FormatType::Stop);
	std::cout << "Sending Stop";
	receiveData();
}

void TeleProtClient::receiveData()
{
	//буфер дляф данных
	uint8_t buffer[1024];

	//чтение ответа
	int data = recv(cfd, &buffer, sizeof(buffer), 0);
	if (data < 0)
	{
		std::cout << "Receiving error";
		return;
	}

	//сохраняем тип кадра
	uint8_t format = buffer[4];

	//в зависимости от типа выводим сообщение
	if (format == FormatType::Ack)
	{
		std::cout << "Received ACK";
		return;
	}
	else if (format == FormatType::Nack)
	{
		std::cout << "Received NACK";
		return;
	}

	else if (format == FormatType::DigitalPoints)
	{
		//парсим полезную нагрузку
		//начальная позиция полезной нагрузки
		size_t offset = 5;

		//причина передачи(1 байт)
		uint8_t reasonForTransfer = buffer[offset];

		//количество сигналов(2 байта)
		uint16_t count = 0;
		std::memcpy(&count, &buffer[offset], 2);
		offset += 2;

		for (int i = 0; i < count; i++)
		{
			m_digital_point mdp;
			// номер сигнала PointId (4 байта LSB)
			std::memcpy(&mdp.pointId, &buffer[offset], 4);
			offset += 4;

			// значение сигнала Value (1 байт)
			mdp.value = buffer[offset];
			offset += 1;

			// метка времени (8 байт LSB)
			std::memcpy(&mdp.timeTag, &buffer[offset], 8);
			offset += 8;

			// качество сигнала (1 байт)
			mdp.quality.Valid		= buffer[offset] & 0x01;
			mdp.quality.Substituted = (buffer[offset] >> 1) & 0x01;
			mdp.quality.Overflow	= (buffer[offset] >> 2) & 0x01;

			offset += 1;

			std::cout << "PointId=" << mdp.pointId
					  << " Value=" << static_cast<int>(mdp.value)
					  << "TimeTag = " << mdp.timeTag << "Quality = ["
					  << (mdp.quality.Valid ? "Valid" : "Invalid")
					  << (mdp.quality.Substituted ? "Substituted" : "")
					  << (mdp.quality.Overflow ? "Overflow" : "") << "]\n";
		}
	}

	else if (format == FormatType::AnalogPoints)
	{
		//парсим полезную нагрузку
		//начальная позиция полезной нагрузки
		size_t offset = 5;

		//причина передачи(1 байт)
		uint8_t reasonForTransfer = buffer[offset];

		//количество сигналов(2 байта)
		uint16_t count = 0;
		std::memcpy(&count, &buffer[offset], 2);
		offset += 2;

		for (int i = 0; i < count; i++)
		{
			m_analog_point map;
			// номер сигнала PointId (4 байта LSB)
			std::memcpy(&map.pointId, &buffer[offset], 4);
			offset += 4;

			// значение сигнала Value (1 байт)
			map.value = buffer[offset];
			offset += 1;

			// метка времени (8 байт LSB)
			std::memcpy(&map.timeTag, &buffer[offset], 8);
			offset += 8;

			// качество сигнала (1 байт)
			map.quality.Valid		= buffer[offset] & 0x01;
			map.quality.Substituted = (buffer[offset] >> 1) & 0x01;
			map.quality.Overflow	= (buffer[offset] >> 2) & 0x01;

			offset += 1;

			std::cout << "PointId=" << map.pointId
					  << " Value=" << static_cast<float>(map.value)
					  << "TimeTag = " << map.timeTag << "Quality = ["
					  << (map.quality.Valid ? "Valid" : "Invalid")
					  << (map.quality.Substituted ? "Substituted" : "")
					  << (map.quality.Overflow ? "Overflow" : "") << "]\n";
		}
	}
}

void TeleProtClient::sendGI()
{
	sendFrame(cfd, FormatType::GeneralInterrogation);
	std::cout << "Sending GI"
			  << "\n";
	receiveData();
}

void TeleProtClient::sendDigitalControl(uint32_t			 pointId,
										uint8_t				 value,
										std::vector<uint8_t> payload)
{
	//запись номера сигнала
	std::memcpy(payload.data(), &pointId, 4);

	//запись значения сигнала
	payload.at(4) = value;

	//упаковка и отправка кадра
	sendFrame(cfd, FormatType::DigitalControl, payload);

	std::cout << "Sending DigitalControl"
			  << "\n";

	receiveData();
}

int TeleProtClient::getCfd()
{
	return cfd;
}
