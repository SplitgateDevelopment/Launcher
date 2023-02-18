#include <iostream>
#include <format>


struct logDTO {
	std::string text;
};

class Logger {
public:
	void error(std::string message) {
		return _log({
			"ERROR",
		}, message);
	};

	void success(std::string message) {
		return _log({
			"SUCCESS",
			}, message);
	};

	void info(std::string message) {
		return _log({
			"INFO",
			}, message);
	};

private:
	void _log(logDTO logDTO, std::string message) {

		std::cout << format("[{}] ", logDTO.text) << message << std::endl;
	};
};