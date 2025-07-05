#include "pch.h"
#include "helpers.h"

std::string logFilePath;
LogType logType = Console;
std::mutex logMutex;
std::unique_ptr<std::ofstream> logFile;

namespace Utils
{
	void attachConsole()
	{
		AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
		SetConsoleOutputCP(CP_UTF8);
	}

	void detachConsole()
	{
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
		FreeConsole();
	}

	void clearConsole()
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (h == INVALID_HANDLE_VALUE) return;

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

		DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
		COORD coord = { 0, 0 };
		DWORD n;

		// Overwrite the screen buffer with whitespace
		FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &n);
		GetConsoleScreenBufferInfo(h, &csbi);
		FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);

		// Reset the cursor to the top left position
		SetConsoleCursorPosition(h, coord);
	}

	char consoleReadKey()
	{
		return std::cin.get();
	}

	void logToFile(const std::string& filepath, const std::string& msg)
	{
		if (logFile && logFile->is_open())
		{
			*logFile << msg << std::endl;
			logFile->flush(); // Ensure immediate write
		}
		else
		{
			// Fallback to individual file operations if persistent file isn't available
			std::ofstream tempFile(filepath, std::ios::out | std::ios::app | std::ios::binary);
			if (tempFile.is_open())
			{
				tempFile << msg << std::endl;
			}
		}
	}

	void log(const char* filepath, int line, LogLevel level, const char* fmt, ...)
	{
		char buf[4096];
		const char* levelStr = "";
		WORD levelColor, filenameColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Light Blue
		WORD lineColor = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Light Yellow
	
		// Determine log level string and corresponding color
		switch (level)
		{
		case Debug:
			levelStr = "Debug";
			levelColor = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY; // Magenta
			break;
			
		case Error:
			levelStr = "Error";
			levelColor = FOREGROUND_RED | FOREGROUND_INTENSITY; // Red
			break;
			
		case Warning:
			levelStr = "Warning";
			levelColor = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Yellow
			break;
			
		case None:
		default:
			levelStr = "Log";
			levelColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // Light Gray
			break;
		}
	
		va_list va;
		va_start(va, fmt);
		vsprintf_s(buf, fmt, va);
		va_end(va);
	
		const std::lock_guard<std::mutex> lock(logMutex);
	
		// Console output with coloring
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsole != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo))
			{
				WORD saved_attributes = consoleInfo.wAttributes;
				auto filename = std::filesystem::path(filepath).filename().string();
	
				// Print '[' in default color
				std::cout << "[";
	
				// Print filename in light blue
				SetConsoleTextAttribute(hConsole, filenameColor);
				std::cout << filename;
	
				// Print ':' in default color
				SetConsoleTextAttribute(hConsole, saved_attributes);
				std::cout << ":";
	
				// Print line in light yellow
				SetConsoleTextAttribute(hConsole, lineColor);
				std::cout << line;
	
				// Reset to default color, print level in its color, and reset again
				SetConsoleTextAttribute(hConsole, saved_attributes);
				std::cout << "] [";
				SetConsoleTextAttribute(hConsole, levelColor);
				std::cout << levelStr;
				SetConsoleTextAttribute(hConsole, saved_attributes);
				std::cout << "] " << buf << std::endl;
			}
			else
			{
				// Fallback without coloring if console info retrieval fails
				auto filename = std::filesystem::path(filepath).filename().string();
				std::cout << "[" << filename << ":" << line << "] [" << levelStr << "] " << buf << std::endl;
			}
		}
	
		// File logging
		if (logType == File && !logFilePath.empty())
		{
			auto rawTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			struct tm gmtm;
			gmtime_s(&gmtm, &rawTime);
			
			auto filename = std::filesystem::path(filepath).filename().string();
			auto logLineFile = string_format("[%02d:%02d:%02d] [%s] [%s:%d] %s", 
				gmtm.tm_hour, gmtm.tm_min, gmtm.tm_sec,
				levelStr, filename.c_str(), line, buf);
			
			logToFile(logFilePath, logLineFile);
		}
	}

	bool prepareFileLogging(const std::string& directory)
	{
		try
		{
			auto rawTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			tm gmtm;
			gmtime_s(&gmtm, &rawTime);

			if (!std::filesystem::is_directory(directory))
			{
				if (!std::filesystem::create_directories(directory))
				{
					return false;
				}
			}

			logFilePath = string_format("%s\\log_%04d-%02d-%02d_%02d-%02d.txt", directory.c_str(),
				1900 + gmtm.tm_year, gmtm.tm_mon + 1, gmtm.tm_mday, gmtm.tm_hour, gmtm.tm_min);

			// Close existing file if open
			if (logFile && logFile->is_open())
			{
				logFile->close();
			}

			// Open the log file for appending
			logFile = std::make_unique<std::ofstream>(logFilePath, std::ios::out | std::ios::app | std::ios::binary);
			if (!logFile->is_open())
			{
				logFile.reset();
				return false;
			}

			logType = File;
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	void setLogType(LogType type)
	{
		const std::lock_guard lock(logMutex);
		logType = type;
	}

	void closeFileLogging()
	{
		const std::lock_guard lock(logMutex);
		if (logFile && logFile->is_open())
		{
			logFile->close();
			logFile.reset();
		}
		logType = Console;
	}
}