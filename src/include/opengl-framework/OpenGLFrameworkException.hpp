#pragma once

#include <exception>
#include <string>

class OpenGLFrameworkException : public std::exception {
public:
	explicit OpenGLFrameworkException(const std::string& message)
		: msg(message) {}

	const char* what() const noexcept override {
		return msg.c_str();
	}

private:
	std::string msg;
};