#include "emulator_logger.h"

char * states[] = {"NORMAL","ESC", "GS", "DLE", "FS", "READING_PAYLOAD", "READING_PARAMS"};
void fn_emulator_logger_log_message(const char *message, FILE *file){
    if(file == NULL){
        printf("There was an error in creating the log file. Log file is NULL\n");
        return ;
    }
    if(fwrite(message, sizeof(char), strlen(message), file) != strlen(message)) {
        printf("error writing to log file\n");
        perror("fwrite");
    }
    fflush(file);
}
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
void fn_emulator_logger_log_command_execution(EscPosParser * parser, FILE * logfile){
    //logging the command execution
    char logtext[256] = {0} ;
    char state[10] = {0};
    fn_emulator_logger_convert_state_to_string(parser->prefix, state);
    strcat(logtext, "Executing command:\t");
    strcat(logtext, state);
    strcat(logtext, "\t");

    uint8_t command[4] = {0};
    command[0] = parser->command;
    strcat(logtext, command);

    strcat(logtext, "\t");
    if(parser->params_expected > 0){
                char txt[4] = {0};
        for(int i = 0 ; i < parser->params_expected; i++){
            snprintf(txt, sizeof(txt), "%u", parser->params[i]);
            strcat(logtext, txt);
            strcat(logtext, "\t");
        }
    }
    strcat(logtext,"\n" );
    if(fwrite(logtext, sizeof(char), strlen(logtext), logfile) != strlen(logtext)) {
        printf("error writing to log file\n");
        perror("fwrite");
    }
    fflush(logfile);
}
void fn_emulator_logger_convert_state_to_string(uint8_t prefix, char * string ){
    switch (prefix)
    {
    case 0x1B:
        strcpy(string, "ESC");
        break;
    case 0x1D:
        strcpy(string, "GS");
        break;
    default:
        strcpy(string, "UNKNOWN");
        break;
    }
}