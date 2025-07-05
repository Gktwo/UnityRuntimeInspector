#pragma once

#include <string>
#include <memory>
#include <stdexcept>

enum LogLevel
{
	None,
	Debug,
	Error,
	Warning
};

enum LogType
{
	Console,
	File
};

#define LOG(fmt, ...)   Utils::log(__FILE__, __LINE__, LogLevel::None, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   Utils::log(__FILE__, __LINE__, LogLevel::Debug, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)   Utils::log(__FILE__, __LINE__, LogLevel::Error, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) Utils::log(__FILE__, __LINE__, LogLevel::Warning, fmt, __VA_ARGS__)

namespace Utils
{
	void attachConsole();
	void detachConsole();
	void clearConsole();
	char consoleReadKey();
	void logToFile(const std::string& filepath, const std::string& msg);
	void log(const char* filepath, int line, LogLevel level, const char* fmt, ...);
	bool prepareFileLogging(const std::string& directory);
	void setLogType(LogType type);
	void closeFileLogging();

	template<typename ... Args>
	std::string string_format(const std::string& format, Args ... args)
	{
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		auto size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}
}