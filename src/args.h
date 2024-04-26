#pragma once


#define NOT_A_VALUE(__val__) (__val__ == NULL || __val__[0] == LIN_PARAM_IDENTIFIER || __val__[0] == WIN_PARAM_IDENTIFIER)
//#define IS_VALUE(__val__) (__val__ != NULL || __val__[0] != LIN_PARAM_IDENTIFIER || __val__[0] != WIN_PARAM_IDENTIFIER)
#define IS_1C_ARG(_a_, _v_) ( ( _a_[0] == LIN_PARAM_IDENTIFIER || _a_[0] == WIN_PARAM_IDENTIFIER ) && _a_[1] == _v_ && _a_[2] == 0 )