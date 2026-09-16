#pragma once

#include "types/Error.hpp"
#include <array>
#include <string_view>


namespace TaskManager {

	enum class TimeUnit {
		Minute = 'm',
		Hour = 'h',
		Day = 'd',
		Week = 'w'
	};
	
	struct DateInfo {
		uint8_t minute = 0;
		uint8_t hour = 0;
		uint8_t day = 0;
		uint8_t month = 0;
		uint16_t year = 0;

		static DateInfo Now();

		void Add(uint16_t amount, TimeUnit unit);

	};

	constexpr DateInfo UNDEFINED_DATE = {};
	
	constexpr size_t MAX_TASK_NAME_LENGTH = 64;
	
	struct Task {
		std::array<char, MAX_TASK_NAME_LENGTH> name{};
		size_t nameLength = 0;
		DateInfo dueDate{};
	};

	// A task add command looks like this: /task-add {name} -due {time}
	// Where time can be expressed as a number followed by the unit
	// i.e /task-add Review PR -due 2h
	EError AddTaskCommand(std::string_view rawCommand);
}

