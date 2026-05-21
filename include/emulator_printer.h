#ifndef EMULATOR_PRINTER_H
#define EMULATOR_PRINTER_H

#include "emulator.h"



void fn_emulator_printer_print_byte(uint8_t  , FILE * , PrinterState * , EscPosParser * );
void fn_emulator_printer_write_byte_to_buffer(uint8_t  , PrinterState * );
void fn_emulator_printer_write_string_to_buffer(const char * , PrinterState *);
void fn_emulator_printer_update_state(PrinterState * , EscPosParser * , FILE * );
void fn_emulator_printer_select_command_handler(PrinterState * , EscPosParser * , FILE * );
void fn_emulator_printer_select_command_with_payload_handler(PrinterState * , EscPosParser * , FILE *, uint8_t  );
void fn_emulator_printer_handle_command_1B_0C(PrinterState * , EscPosParser * , FILE * );
void fn_emulator_printer_handle_command_1B_26(PrinterState * , EscPosParser * , FILE * );
void fn_emulator_printer_handle_command_1B_21(PrinterState * , EscPosParser * , FILE * );
void fn_emulator_printer_print_html(char * );

#endif