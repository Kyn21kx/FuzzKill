#include "ConfigLayer.hpp"
#include "Assertions.hpp"
#include <cstdio>
#include <cstdlib>

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
            	// The use of strtol instead of strtoul here is not a mistake, this is the only 8-bit integer we're parsing
            	// And using strtol will give us a signed 32-bit integer, so it's enough to represent all its values
            	result.flags = static_cast<EConfigFlags>(strtol(buffer, nullptr, 16));
            	break;
        	};
            case EConfigStages::BGColor: {
            	result.backgroundColor = static_cast<uint32_t>(strtoul(buffer, nullptr, 16));
            	break;
        	};
            case EConfigStages::FGColor: {
            	result.foregroundColor = static_cast<uint32_t>(strtoul(buffer, nullptr, 16));
            	break;
        	};
            case EConfigStages::BorderColor: {
            	result.borderColor = static_cast<uint32_t>(strtoul(buffer, nullptr, 16));
            	break;
        	};
            case EConfigStages::HighlightColor: {
            	result.highlightColor = static_cast<uint32_t>(strtoul(buffer, nullptr, 16));
            	break;
        	};
            case EConfigStages::ItemColor: {
            	result.itemColor = static_cast<uint32_t>(strtoul(buffer, nullptr, 16));
            	break;
        	};
        	default: {
        		FUZZ_ASSERT(false, stderr, "Config stage %d not handled!", currLine);
        		break;
        	}
        }
        currLine++;
	}
	return result;
}
