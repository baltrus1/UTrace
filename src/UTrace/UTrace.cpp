#include "UTrace.h"

#include <iostream>
#include <cstdarg>
#include <unistd.h>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

namespace {
	std::string getTraceName(TraceLevel level) {
		switch(level) {
			case TraceLevel::INFO:
				return "INFO";
			case TraceLevel::WARNING:
				return "WARNING";
			case TraceLevel::ERROR:
				return "ERROR";
		}
	}

	enum class TraceMethod {
		TCP,
		FILE,
		SERIALPORT
	};

	class Tracer {
	public:
		Tracer() {
			const char* traceLevel = getenv("TRACELEVEL");
			if (traceLevel) {
				m_level = static_cast<TraceLevel>(strtol(traceLevel, nullptr, 10));
			}
			const char* traceMethod = getenv("TRACEMETHOD");
			if (traceMethod) {
				m_method = static_cast<TraceMethod>(strtol(traceMethod, nullptr, 10));
			}
			
			switch(m_method) {
				case TraceMethod::TCP: {
					const char* host = getenv("TCP_HOST");
					const char* port = getenv("TCP_PORT");
					if (!host || !port) {
						std::cerr << "Sending logs with tcp requires host and port. Specify with 'export TCP_HOST=? & export TCP_PORT=?'" << std::endl;
						m_error = true;
						break;
					}
					m_dest = std::string("/dev/tcp/") + host + '/' + port;
					break;
				}
				case TraceMethod::FILE: {
					const char* traceFile = getenv("TRACEFILE");
					if (traceFile) {
						m_file = fopen(traceFile, "w");
					} else {
						std::cerr << "Specify trace file with 'export TRACEFILE=?'. Default file location at 'output.txt' will be used" << std::endl;
						m_file = fopen("output.txt", "w");
					}
				}
				case TraceMethod::SERIALPORT: {
					const char* serialPort = getenv("SERIAL_PORT");
					if (!serialPort) {
						std::cerr << "Sending logs to serial ports requires specification with 'export SERIAL_PORT=/dev/?'" << std::endl;
						m_error = true;
						break;
					}
					m_dest = std::string(serialPort);
				}
			}
		}

		~Tracer() {
			for (const auto& thread : logThreads) {
				thread->join();
			}

			if (m_file) {
				fclose(m_file);	
			}
		}

		void trace(TraceLevel level, const std::string& message) {
			if (!traceEnabled(level)) {
				return;
			}

			if (m_error) {
				return;
			}

			std::lock_guard<std::mutex> guard(m_traceMutex);
			switch(m_method) {
				case TraceMethod::TCP: {
					auto cmd = std::string("/bin/bash -c 'echo ") + message + " > " + m_dest + '\'';
					system(cmd.c_str());
					break;
				}
				case TraceMethod::FILE: {
					fprintf(m_file, "%s", message.c_str());
					break;
				}
				case TraceMethod::SERIALPORT: {
					auto cmd = std::string("/bin/bash -c 'echo ") + message + " > " + m_dest + '\'';
					system(cmd.c_str());
					break;
				}
			}
		}

	private:
		bool traceEnabled(TraceLevel level) {
			return static_cast<int>(level) >= static_cast<int>(m_level);
		}

	public:
		std::vector<std::shared_ptr<std::thread>> logThreads;

	private:
		TraceLevel m_level{TraceLevel::INFO}; // this and more important levels will be printed only
		TraceMethod m_method{TraceMethod::FILE};
		bool m_error = false;
		std::mutex m_traceMutex;

		FILE* m_file;
		
		std::string m_dest;
	};

	static Tracer tracer;
} // end namespace

void trace(TraceLevel level, const char* message, ...) {
	va_list ap;
	va_start(ap, message);
	char* buf = (char*)malloc(sizeof(char) * 150);
	if (std::vsprintf(buf, message, ap) < 0) {
		std::cerr << "Failed to write message to buffer." << std::endl;
		return;
	}
	va_end(ap);
	std::string fullMessage = getTraceName(level) + ": " + buf;
	
	auto logThread = std::make_shared<std::thread>(&Tracer::trace, &tracer, level, fullMessage);
	tracer.logThreads.push_back(logThread);
}
