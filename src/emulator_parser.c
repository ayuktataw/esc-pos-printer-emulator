/*
 * =============================================================================================
 * Author: Tataw H. Ayuk
 * Description: This function is written for the sole purpose of interpreting and deciding 
 * 				The right command to use and function to run.
 * ==============================================================================================
*/
#include "emulator_printer.h"
#include "emulator_parser.h"
#include "emulator_logger.h"

static const EscPosCommand commands[] = {
	{0x1B, 0x0C, 0, false, false},
	{0x1B, 0x20, 1, false, false},
	{0x1B, 0x21, 1, false, false},
	{0x1B, 0x24, 2, false, false},
	{0x1B, 0x25, 1, false, false},
	{0x1B, 0x26, 3, true, true},//has user defined variables
	{0x1B, 0x2A, 3, false, true},//has user defined variables

	{0x1B, 0x2D, 1, false, false},
	{0x1B, 0x32, 0, false, false},
	{0x1B, 0x33, 1, false, false},
	{0x1B, 0x3D, 1, false, false},
	{0x1B, 0x3F, 1, false, false},
	{0x1B, 0x40, 0, false, false},
	{0x1B, 0x44, 0, true, true}, //has user defined variables,
	{0x1B, 0x45, 1, false, false},
	{0x1B, 0x47, 1, false, false},
	{0x1B, 0x4A, 1, false, false},
	{0x1B, 0x4C, 0, false, false},
	{0x1B, 0x4D, 1, false, false},
	{0x1B, 0x52, 1, false, false},
	{0x1B, 0x53, 0, false, false},
	{0x1B, 0x54, 1, false, false},
	{0x1B, 0x56, 1, false, false},
	{0x1B, 0x57, 0, true, true}, //has user defined variables
	{0x1B, 0x5C, 2, false, false},
	{0x1B, 0x61, 1, false, false},
	{0x1B, 0x63, 2, false, false},//there is a variable overloading here
	{0x1B, 0x64, 1, false, false},
	{0x1B, 0x69, 0, false, false},
	{0x1B, 0x6D, 0, false, false},
	{0x1B, 0x70, 3, false, false},
	{0x1B, 0x74, 1, false, false},
	{0x1B, 0x7B, 1, false, false}
};

void fn_emulator_parser_feeder(EscPosParser *parser, uint8_t * data, size_t len, FILE * file, PrinterState * state)
{
	/* 
	 * This function receives a parser pointer from the main function, the data from the main loop
	 * and the length of bytes read to loop over and call a byte processor to handle each byte
	 */
	for(size_t i = 0 ; i < len ; i++ )
	{
		//Within this loop we check if we are in Normal state. 
			//If so then check if current byte is a command prefix
			//If yes then we call the process bytes function to handle the state transition and command parsing
			//If not we write the byte to the printer buffer and continue
		//If we are not in normal state, we call the process bytes function to handle the byte based on the current state of the parser

		if(parser->state == STATE_NORMAL)
		{
			if(data[i] == 0x1B || data[i] == 0x1D || data[i] == 0x10 || data[i] == 0x1C)
			{
				fn_emulator_parser_process_bytes(parser , data[i], file, state);
			}else{
				//check for horizontal tab (0x09), line feed (0x0A), carriage return (0x0D), print (0x0C) and cancel print (0x18)
				//if none then we just write the byte to the printer buffer and continue
				switch (data[i])
				{
				case 0x09: // horizontal tab
					fn_emulator_printer_write_string_to_buffer(parser, "    ");
					break;
				case 0x0A: // line feed
					fn_emulator_printer_write_string_to_buffer(parser, "<br>");
					break;
				case 0x0D: // carriage return
					fn_emulator_printer_write_string_to_buffer(parser, "<br>");
					break;
				case 0x0C: // print
					fn_emulator_printer_write_string_to_buffer(parser, "</body></html>");
					break;
				case 0x18: // cancel print
					fn_emulator_logger_log_message(file, "Print cancelled by command 0x18");
					break;

				default:
					fn_emulator_printer_write_byte_to_buffer(data[i], state);
					break;
				}
			}
		}else{
			fn_emulator_parser_process_bytes(parser , data[i], file, state);
		}
	}
	return;
}


void fn_emulator_parser_process_bytes(EscPosParser * parser , uint8_t byte, FILE * log_file, PrinterState * state)
{
	//This function selects the proper function to handle the byte based on
	//the state of the parser. 
	switch(parser->state )
	{
		case STATE_NORMAL:
			if(byte == 0x1B)
			{
				parser-> state = STATE_ESC;
				parser->prefix = 0x1B;
				 emulator_logger_log_parser_state_transition( parser, log_file, STATE_NORMAL);
			}
			else if(byte == 0x1D)
			{
				parser->state = STATE_GS;
				parser->prefix = 0x1D;
				 emulator_logger_log_parser_state_transition( parser, log_file, STATE_NORMAL);
			}
			else if(byte == 0x10)
			{
				// real time status transmission mode
				parser->state = STATE_DLE;
				parser->prefix = 0x10;
				 emulator_logger_log_parser_state_transition( parser, log_file, STATE_NORMAL);
			}
			else if( byte == 0x1C )
			{
				parser->state = STATE_FS;
				parser->prefix = 0x1C;	
				 emulator_logger_log_parser_state_transition( parser, log_file, STATE_NORMAL);
			}	
			else{
				fn_emulator_printer_print_byte(byte, log_file, state, parser);
			}
			break;
		case STATE_ESC:
			fn_emulator_parser_handle_esc_mode(byte, log_file , state, parser);
			break;
		case STATE_GS:
			fn_emulator_parser_handle_gs_mode(byte, log_file , state, parser);
			break;
		case STATE_DLE:
			fn_emulator_parser_handle_dle_mode(byte, log_file, state, parser);
			break;
		case STATE_FS:
			fn_emulator_parser_handle_fs_mode(byte, log_file, state, parser);
			break;
		case STATE_READING_PARAMS:
				if(parser->prefix == 0x1B)
				{
					fn_emulator_parser_handle_esc_mode(byte, log_file, state, parser);
				}else if(parser->prefix == 0x1D)
				{
					fn_emulator_parser_handle_gs_mode(byte, log_file, state, parser);
				}else if(parser->prefix == 0x10)
				{
					fn_emulator_parser_handle_dle_mode(byte, log_file, state, parser);
				}else if(parser->prefix == 0x1C)
				{
					fn_emulator_parser_handle_fs_mode(byte, log_file, state, parser);
				}
			break;
		case STATE_READING_PAYLOAD:
			
			break;
	}	
	return;
}


void fn_emulator_parser_handle_esc_mode(uint8_t byte, FILE * logfile, PrinterState * state, EscPosParser * parser){
	fn_emulator_parser_process_command(parser, logfile, byte, STATE_ESC, state);
	return;
}

void fn_emulator_parser_handle_dle_mode(uint8_t byte, FILE * logfile, PrinterState * state, EscPosParser * parser){
	fn_emulator_parser_process_command(parser, logfile, byte, STATE_DLE, state	);
	return;
}

void fn_emulator_parser_handle_gs_mode(uint8_t byte, FILE * logfile, PrinterState * state, EscPosParser * parser){	
	fn_emulator_parser_process_command(parser, logfile, byte, STATE_GS, state);
	return;
}

void fn_emulator_parser_handle_fs_mode(uint8_t byte, FILE * logfile, PrinterState * state, EscPosParser * parser){
	fn_emulator_parser_process_command(parser, logfile, byte, STATE_FS, state);
	return;
}

int fn_emulator_parser_find_command_index(uint8_t prefix, uint8_t command){

	for(size_t i = 0 ; i < sizeof(commands) / sizeof(commands[0]) ; i++)
	{
		if(commands[i].prefix == prefix && commands[i].command == command)
		{
			return i;
		}
	}
	return -1; // Command not found
}

bool fn_emulator_parser_verify_command_exists(uint8_t prefix, uint8_t command){

	return fn_emulator_parser_find_command_index(prefix, command) != -1;
}

unsigned char * fn_emulator_parser_change_command_to_string(EscPosParser * parser)

{
	int cmd_index = fn_emulator_parser_find_command_index(parser->prefix, parser->command);
	if(cmd_index == -1)
	{
		return NULL; // Command not found
	}
	
	static unsigned char cmd_string[256] = {0};
	snprintf((char *)cmd_string, sizeof(cmd_string), "Prefix: 0x%02X, Command: 0x%02X", parser->prefix, parser->command);
	strcat((char *)cmd_string, ", Params: ");
	for(size_t i = 0 ; i < parser->params_received ; i++)
	{		char param_str[16];
		snprintf(param_str, sizeof(param_str), "0x%02X ", parser->params[i]);
		strcat((char *)cmd_string, param_str);
	}
	strcat((char *)cmd_string, ", User Defined Params: ");
	return cmd_string;
}

void fn_emulator_parser_process_command(EscPosParser * parser,  FILE * logfile, uint8_t byte, ParserState previous_state, PrinterState * state)
{
	if( !(parser->state == STATE_READING_PARAMS || parser->state == STATE_READING_PAYLOAD)  )
		{
			//if the parser is not in any of the states above means we are here for the first time after 
			//receiving the command prefix, so we need to set the command and check if we need to read parameters or payload
			parser->command = byte;
			//If there are parameters expected, change state to reading parameters, otherwise we will execute the command immediately in the main loop and change state back to normal
			if( fn_emulator_parser_verify_command_exists(parser->prefix, parser->command) )
			{
				//the command exist in our command table
				int cmd_index = fn_emulator_parser_find_command_index(parser->prefix, parser->command);
				if(commands[cmd_index].params_expected > 0)
				{
					parser->state = STATE_READING_PARAMS;
					emulator_logger_log_parser_state_transition( parser, logfile, previous_state);
				}else {

						//if it neither has fixed parameters nor user defined parameters, but has a payload, we change state to reading payload
					if(commands[cmd_index].has_payload)
					{
						parser->state = STATE_READING_PAYLOAD;
						emulator_logger_log_parser_state_transition( parser, logfile, previous_state);
					}else{
						//if it has no parameters and no payload, we can execute the command immediately in the main loop and change state back to normal
						fn_emulator_printer_select_command_handler(state, parser, logfile);
						parser->state = STATE_NORMAL;
						emulator_logger_log_parser_state_transition( parser, logfile, previous_state);	
					}
					
				}
			}else{
				parser->state = STATE_NORMAL;
				fprintf(logfile, "Unknown command 0x%02X received", byte);
				emulator_logger_log_parser_state_transition( parser, logfile, previous_state);
				return;
			}
		}
	else{
		int cmd_index = fn_emulator_parser_find_command_index(parser->prefix, parser->command);
		EscPosCommand cmd = commands[cmd_index];
		parser->params_expected = cmd.params_expected;
		parser->has_payload = cmd.has_payload;
		fn_emulator_parser_process_params(parser, cmd, logfile, byte, state);
	}
	return;
}

void fn_emulator_parser_process_params(EscPosParser * parser, EscPosCommand cmd, FILE * logfile, uint8_t byte, PrinterState * state)
{
	//if we are here it means the command was already set and we are in the process of reading parameters or payload, so we need to check if we are still reading parameters or if we have finished reading parameters and need to check for payload

		if(parser->state == STATE_READING_PARAMS && parser->params_received < parser->params_expected){
			parser->params[parser->params_received] = byte;
			parser->params_received++;
			return;
		}
		else
		{
			//we have read all the parameters, now we can check if we need to read a payload
			if(cmd.has_payload)
			{
				if(parser->state != STATE_READING_PAYLOAD)
				{
					parser->state = STATE_READING_PAYLOAD;
				}
				emulator_logger_log_parser_state_transition( parser, logfile, STATE_READING_PARAMS);
				//we will handle the payload reading in another function, for now we just change the state
				//the command handler will decide how the memory space for the payload and read the payload
				//When it is done, it will set the has_payload flag to false
				fn_emulator_printer_select_command_with_payload_handler(state, parser, logfile, byte);
				return;
			}
			else{
				//if we are here, it either means there is no payload or the reader has finished reading the payload, 
				//so we can execute the command immediately in the main loop and change state back to normal
				//the byte we are holding in this case now is an ordinary byte that comes after the parameters or payload, so we will not pass it to the command handler, instead we will just execute the command and then process this byte in the main loop
				//we should write that to the buffer after executing the command since some commands might change the state of the printer and affect how the byte is printed
				if(parser->state == STATE_READING_PAYLOAD)
				{
					fn_emulator_printer_select_command_with_payload_handler(state, parser, logfile, byte);
				}else{
					//no payload 
					fn_emulator_printer_select_command_handler(state, parser, logfile);					
				}
				fn_emulator_printer_write_byte_to_buffer(byte, state);

				ParserState previous_state = parser->state;
				parser->state = STATE_NORMAL;
				emulator_logger_log_parser_state_transition( parser, logfile, previous_state);
			}
		}
		
}
	
