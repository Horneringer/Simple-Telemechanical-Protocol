#include "teleprot.hpp"

#include <iostream>

int main()
{
	TeleProtClient tpc;

	//соединение с сервером
	tpc.run();

	//отправка START запроса
	tpc.sendStart();

	//отправка общего запроса
	tpc.sendGI();

	std::vector<uint8_t> payload;

	int cfd = tpc.getCfd();

	uint8_t value = 8;

	tpc.sendDigitalControl(cfd, value, payload);

	//отправка STOP запроса
	tpc.sendStop();
	return 0;
}
