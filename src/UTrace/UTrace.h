#pragma once

enum class TraceLevel {
	INFO,
	WARNING,
	ERROR
};

void trace(TraceLevel level, const char* message, ...);
