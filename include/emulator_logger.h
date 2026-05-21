#ifndef EMULATOR_LOGGER_H
#define EMULATOR_LOGGER_H

#include "emulator.h"

void emulator_logger_log_parser_state_transition( EscPosParser *, FILE * , ParserState );
void fn_emulator_logger_log_message(const char *message, FILE *file);
#endif