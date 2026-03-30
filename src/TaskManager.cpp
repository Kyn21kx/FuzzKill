#include "TaskManager.hpp"
#include "types/Error.hpp"
#include <array>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <string_view>

TaskManager::DateInfo TaskManager::DateInfo::Now() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);

    DateInfo info;
    info.minute = static_cast<uint8_t>(t->tm_min);
    info.hour = static_cast<uint8_t>(t->tm_hour);
    info.day = static_cast<uint8_t>(t->tm_mday);
    info.month = static_cast<uint8_t>(t->tm_mon + 1);   // tm_mon is 0–11
    info.year = static_cast<uint16_t>(t->tm_year + 1900); // tm_year is years since 1900

    return info;
}


void TaskManager::DateInfo::Add(uint16_t amount, TaskManager::TimeUnit unit) {
    // Pack into tm
    tm t{};
    t.tm_min  = minute;
    t.tm_hour = hour;
    t.tm_mday = day;
    t.tm_mon  = month - 1;
    t.tm_year = year - 1900;

    // Convert to epoch seconds
    time_t epoch = mktime(&t);

    // Add duration in seconds
    switch (unit) {
        case TaskManager::TimeUnit::Minute: epoch += amount * 60;            break;
        case TaskManager::TimeUnit::Hour:   epoch += amount * 3600;          break;
        case TaskManager::TimeUnit::Day:    epoch += amount * 86400;         break;
        case TaskManager::TimeUnit::Week:   epoch += amount * 604800;        break;
    }

    // Unpack back
    tm* result = localtime(&epoch);
    minute = static_cast<uint8_t>(result->tm_min);
    hour   = static_cast<uint8_t>(result->tm_hour);
    day    = static_cast<uint8_t>(result->tm_mday);
    month  = static_cast<uint8_t>(result->tm_mon + 1);
    year   = static_cast<uint16_t>(result->tm_year + 1900);
}

// i.e /task-add Review PR -due 2h
EError TaskManager::AddTaskCommand(std::string_view rawCommand) {
	constexpr std::string_view baseCommand = "/task-add";

	Task result{};
	
	// Separate the first space from everything else
	std::string_view cmdInfo = rawCommand.substr(baseCommand.size() + 1);

	// Parse everything until a -due is found

	constexpr std::string_view DUE_SEPARATOR = "-due";

	size_t duePosition = cmdInfo.find(DUE_SEPARATOR);

	if (duePosition == std::string_view::npos) {
		// We will leave the task's due date as undefined
		return EError::Ok;
	}

	// Now, the due date should be a number and a unit
	std::array<char, 32> buffer;
	size_t digitCount = 0;
	size_t subStrPosition = 0;
	
	for (size_t i = duePosition + DUE_SEPARATOR.size(); i < cmdInfo.size(); i++) {
		char currChar = cmdInfo.at(i);
		if (currChar == ' ' || currChar == '\t') continue;
		if (!std::isdigit(currChar)) break;
		buffer[digitCount] = currChar;
		digitCount++;
		subStrPosition = i;
	}

	if (digitCount <= 0) {
		return EError::ParseFailed;
	}

	int32_t amount = std::atoi(buffer.data());

	if (amount <= 0 || cmdInfo.size() <= subStrPosition + 1) {
		return EError::InvalidArgument;
	}

	// Then we get the unit of time, we assume it's simply the character right next to the number that isn't a whitespace
	char unitRaw = 0;
	for (size_t i = subStrPosition + 1; i < cmdInfo.size(); i++) {
		char currChar = cmdInfo.at(i);
		if (currChar == ' ' || currChar == '\t') continue;
		unitRaw = currChar;
		break;
	}

	TimeUnit parsedTimeUnit = static_cast<TimeUnit>(unitRaw);
	switch (parsedTimeUnit) {
        case TimeUnit::Minute:
        case TimeUnit::Hour:
        case TimeUnit::Day:
        case TimeUnit::Week:
			break;
        default:
        	return EError::InvalidArgument;
    }

    DateInfo dueDate = DateInfo::Now();
    dueDate.Add(amount, parsedTimeUnit);
}
