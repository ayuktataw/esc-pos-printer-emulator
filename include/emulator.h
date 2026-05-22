#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pty.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_USER_CHARS 95
#define FONT_A_WIDTH 12
#define FONT_A_HEIGHT 24
#define FONT_B_WIDTH 9
#define FONT_B_HEIGHT 17

typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t data[24][12];
}UserChar;

typedef struct {
    UserChar defaultFontA[MAX_USER_CHARS];
    UserChar defaultFontB[MAX_USER_CHARS];

    UserChar userDefinedFontA[MAX_USER_CHARS];
    UserChar userDefinedFontB[MAX_USER_CHARS];

    bool currentFontIsA;
}Font; 

typedef enum {
	STATE_NORMAL,
	STATE_ESC,
	STATE_GS,
	STATE_DLE,
	STATE_FS,
	STATE_READING_PAYLOAD, 
	STATE_READING_PARAMS
}ParserState;

typedef struct 
{
	ParserState state;

	uint8_t prefix;
	uint8_t command;

	uint8_t params[16];
	size_t params_expected;
	size_t params_received;

	bool has_payload;
	uint8_t *payload;
	size_t payload_expected;
	size_t payload_received;
}EscPosParser;

typedef struct {
	size_t size;
	size_t current_pointer_position;
	char * data;
} PrintBuffer;

typedef struct 
{
	bool bold;
	uint8_t alignment;
	uint8_t font;
	bool double_width;
	bool double_height;
	bool underline;
	Font font_config;
	uint8_t justification;
	PrintBuffer * print_buffer;
}PrinterState;



void fn_emulator_reinitialize_print_buffer_pointer(PrintBuffer * buffer);
void fn_emulator_move_buffer_pointer(PrintBuffer * buffer, size_t offset);
void fn_emulator_decrease_buffer_pointer_position(PrintBuffer * buffer, size_t offset);
void fn_emulator_cleanup(FILE * logfile, int mas_fd, int slave_fd);
void fn_emulator_set_date_string(char * datestring);

#endif
