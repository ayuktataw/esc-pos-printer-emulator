#ifndef EMULATOR_LOGGER_H
#define EMULATOR_LOGGER_H

#include "emulator.h"

void emulator_logger_log_parser_state_transition( EscPosParser *, FILE * , ParserState );
void fn_emulator_logger_log_message(const char *message, FILE *file);
void fn_emulator_logger_log_command_execution(EscPosParser * parser, FILE *);
void fn_emulator_logger_convert_state_to_string(uint8_t prefix, char * string );
#endif
