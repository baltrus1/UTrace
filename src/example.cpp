#include "UTrace/UTrace.h"
#include <string>

int main() {
	int i = 23;
	std::string name = "Baltrus";
	trace(TraceLevel::WARNING, "Mano vardas %s ir man yra %d metai.", name.c_str(), i);

	//tcp vulnerability example
	// trace(TraceLevel::ERROR, "you have been pwnd & ls -la & echo you have been pwnd");

}