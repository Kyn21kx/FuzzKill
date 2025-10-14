#include "ConfigLayer.hpp"

ConfigData ReadConfigFile(const std::string_view& file, EError* outErr) {
	ConfigData result{};
	FILE* handle = fopen(file.data(), "r");
	if (handle == nullptr) {
		*outErr = EError::FileNotFound;
		fprintf(stderr, "There was an error opening the file %s, please make sure it exists and is readable!", file.data());
		return result;
	}
	char buffer[MAX_LINE_LENGTH];
	uint8_t currLine = 0;
	while (fgets(buffer, MAX_LINE_LENGTH, handle) != nullptr) {
		auto stage = static_cast<EConfigStages>(currLine);
		switch (stage) {
            case EConfigStages::Flags: {
            	result.flags = static_cast<EConfigFlags>(strtol(buffer, nullptr, 10));
            	break;
        	};
            case EConfigStages::BGColor: {
            	result.backgroundColor = static_cast<uint32_t>(strtol(buffer, nullptr, 10));
            	break;
        	};
            case EConfigStages::FGColor: {
            	result.foregroundColor = static_cast<uint32_t>(strtol(buffer, nullptr, 10));
            	break;
        	};
            case EConfigStages::BorderColor: {
            	result.borderColor = static_cast<uint32_t>(strtol(buffer, nullptr, 10));
            	break;
        	};
        	default:
        		break;
        }
        currLine++;
	}
	return result;
}
