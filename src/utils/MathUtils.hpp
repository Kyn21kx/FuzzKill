#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
namespace MathUtils {
	enum class OperationType : uint8_t {
		Sum = '+',
		Sub = '-',
		Mult = '*',
		Div = '/',
		Pow = '^'
	};

	constexpr inline bool IsOperator(char op) {
	    switch (static_cast<OperationType>(op)) {
	    	case OperationType::Sum:
	    	case OperationType::Sub:
	    	case OperationType::Mult:
	    	case OperationType::Div:
	    	case OperationType::Pow:
	    		return true;
	    }
	    return false;
	}
	
	constexpr inline int32_t OperatorPrecedence(char op)
	{
	    // Returns the precedence of the operator
	    switch (static_cast<OperationType>(op)) {
	    	case OperationType::Sum:
	    	case OperationType::Sub:
	    		return 1;
	    	case OperationType::Mult:
	    	case OperationType::Div:
	    		return 2;
	    	case OperationType::Pow:
	    		return 3;
	    }
	    return 0;
	}	

	inline float ApplyOperation(float a, float b, OperationType op) {
		
	    switch (static_cast<OperationType>(op)) {
	    	case OperationType::Sum:
	    		return a + b;
	    	case OperationType::Sub:
	    		return a - b;
	    	case OperationType::Mult:
	    		return a * b;
	    	case OperationType::Div:
	    		return a / b;
	    	case OperationType::Pow:
	    		return powf(a, b);
	    }
	    return std::numeric_limits<float>::infinity();
	}
}

