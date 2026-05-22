#ifndef EMULATOR_PARSER_H
#define EMULATOR_PARSER_H

#include "emulator.h"
typedef struct {
    uint8_t prefix;
    uint8_t command;
    size_t params_expected;
    bool has_payload;
    bool has_user_defined_params;
}EscPosCommand;

void fn_emulator_parser_feeder(EscPosParser *, uint8_t * , size_t , FILE * , PrinterState *);
void fn_emulator_parser_process_bytes(EscPosParser *, uint8_t , FILE * , PrinterState *);
void fn_emulator_parser_handle_esc_mode(uint8_t , FILE * , PrinterState *, EscPosParser * );
void fn_emulator_parser_handle_gs_mode(uint8_t , FILE * , PrinterState * , EscPosParser * );
void fn_emulator_parser_handle_dle_mode(uint8_t , FILE * , PrinterState * , EscPosParser * );
void fn_emulator_parser_handle_fs_mode(uint8_t , FILE * , PrinterState * , EscPosParser * );
int fn_emulator_parser_find_command_index(uint8_t prefix, uint8_t command);
bool fn_emulator_parser_verify_command_exists(uint8_t prefix, uint8_t command);

unsigned char * fn_emulator_parser_change_command_to_string(EscPosParser * parser);
void fn_emulator_parser_process_params(EscPosParser * parser , EscPosCommand cmd, FILE * logfile, uint8_t byte, PrinterState *);
void fn_emulator_parser_process_command(EscPosParser * parser,  FILE * logfile, uint8_t byte, ParserState previous_state, PrinterState *);
void fn_emulator_parser_reset(EscPosParser * parser);
#endif