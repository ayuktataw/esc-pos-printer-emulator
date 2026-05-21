#include "emulator_logger.h"

char * states[] = {"NORMAL","ESC", "GS", "DLE", "FS", "READING_PAYLOAD", "READING_PARAMS"};
void emulator_logger_log_parser_state_transition( EscPosParser * parser, FILE * file, ParserState previous_state) 
{
    if(file == NULL){
        printf("There was an error in creating the log file. Log file is NULL\n");
        return ;
    }

    char log_text[256] = {0};
    strcat(log_text, states[previous_state]);
    strcat(log_text, "\t->\t");
    strcat(log_text, states[parser->state]);
    strcat(log_text, "\n");

    if(fwrite(log_text, sizeof(char), strlen(log_text), file) != strlen(log_text)) {
        printf("error writing to log file\n");
        perror("fwrite");
    }
    fflush(file);

}