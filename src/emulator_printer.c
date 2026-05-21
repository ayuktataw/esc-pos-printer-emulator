#include "emulator_printer.h"
#include "emulator_parser.h"
#include "emulator_logger.h"
void fn_emulator_printer_print_byte(uint8_t  byte, FILE * log_file, PrinterState * state, EscPosParser * parser){
    //This function is responsible for printing a byte to the console. 
    //In a real implementation, this would be where you send the byte to the printer hardware. 
    //For this emulator, we will just print the byte as a hexadecimal value. 
    fprintf(log_file, "%02X ", byte);
}
void fn_emulator_printer_write_byte_to_buffer(uint8_t byte, PrinterState * state){
    //This function writes a byte to the print buffer. 
    //For this emulator, we will just append the byte to the buffer as an ascii character.

    *state->print_buffer->data = byte;
    fn_emulator_move_buffer_pointer(state->print_buffer, 1);
}
void fn_emulator_printer_write_string_to_buffer(const char * str, PrinterState * state){
    //This function writes a string to the print buffer. 
    //For this emulator, we will just append the string to the buffer as ascii characters. 
    strcat(state->print_buffer->data, str);
    fn_emulator_move_buffer_pointer(state->print_buffer, strlen(str));
}


void fn_emulator_printer_print_html(char * buffer){
    //This function prints the contents of the print buffer as HTML. 
    //In a real implementation, this would be where you send the HTML to a web server or save it to a file. 
    FILE * file;
    file = fopen("print_output.html", "w");
    if(file == NULL){
        printf("There was an error in creating the HTML file. HTML file is NULL\n");
        return ;
    }
    fprintf(file, "%s", buffer);
    fclose(file);
}
void fn_emulator_printer_select_command_with_payload_handler(PrinterState * printer, EscPosParser * parser, FILE * log_file, uint8_t byte)
{
    switch (parser->command)
    {
    case 0x26: // ESC & - Define user-defined characters
        fn_emulator_printer_handle_command_1B_26(printer, parser, log_file, byte);
        break;
    
    default:
        break;
    }
}
void fn_emulator_printer_select_command_handler(PrinterState * printer, EscPosParser * parser, FILE * log_file){
    //This function selects the proper command handler based on the command received. 
    if(parser->prefix == 0x1B)
    {
        switch(parser->command)
        {
            case 0x0C: // ESC FF
                //do nothing yet
                break;
            case 0x20: // ESC SP n 
                printer->alignment = parser->params[0];
                break;
            case 0x21: // ESC ! n - Select print mode
                fn_emulator_printer_handle_command_1B_21(printer, parser, log_file);
                break;
            case 0x25: // ESC % - Select/cancel user-defined character set
                //do nothing yet
                 break;
            case 0x26: // ESC & - Define user-defined characters
                //we will handle this command in a separate function since it has a payload
                 break;
                break;
            default:
                fprintf(log_file, "Unknown ESC command: %02X\n", parser->command);
                break;
        }
    }
    return;
}

void fn_emulator_printer_handle_command_1B_0C(PrinterState * printer, EscPosParser * parser, FILE * log_file){
    //This function handles the ESC FF command, which is used to print data in page mode
    
}
void fn_emulator_printer_handle_command_1B_21(PrinterState * printer, EscPosParser * parser, FILE * log_file)
{
    //This function handles the ESC ! n command, which is used to select print mode. 
    //The n parameter is a bit field that specifies the print mode settings. 
    //For this emulator, we will just set the printer state based on the bits in the n parameter. 
    uint8_t n = parser->params[0];
    if((n & 0x08) != 0)
    {
        printer->bold = true;
        fn_emulator_printer_write_string_to_buffer("<b>", printer);
    }else{
        printer->bold = false;
        fn_emulator_printer_write_string_to_buffer("</b>", printer);    
    }
    if((n & 0x10) != 0)
    {
        printer->double_width = true;
        fn_emulator_printer_write_string_to_buffer("<span style=\"font-size: 200%;\">", printer);
    }else{
        printer->double_width = false;
        fn_emulator_printer_write_string_to_buffer("</span>", printer);
    }
    if((n & 0x20) != 0)
    {
        printer->double_height = true;
        fn_emulator_printer_write_string_to_buffer("<span style=\"font-size: 200%;\">", printer);
    }else{
        printer->double_height = false;
        fn_emulator_printer_write_string_to_buffer("</span>", printer);
    }
    if((n & 0x80) != 0)
    {
        printer->underline = true;
        fn_emulator_printer_write_string_to_buffer("<u>", printer);
    }else{
        printer->underline = false;
        fn_emulator_printer_write_string_to_buffer("</u>", printer);
    }
}
void fn_emulator_printer_handle_command_1B_26(PrinterState * printer, EscPosParser * parser, FILE * log_file, uint8_t byte)
{
    if(parser->has_payload)
    {
        //Calculating the number of bytes of payload we need to read based on the parameters received.
        int x  = 0;
        if(printer->font_config.currentFontIsA)
        {
            if(parser->params[0] != 3) return; // y value not 3 bytes (24 bits/dots)
            if(parser->params[3] !=12 ) return; //font mode is A which means x value must be 12 bits/dots
            x = 12;
        }else{
            //current font is B
            if(parser->params[0] != 3) return; // y value not 3 bytes (24 bits/dots) of which we will use 17
            if(parser->params[3] != 9 ) return; //font mode is A which means x value must be 9 bits/dots
            x = 9;
        }
        if(parser->payload_expected == 0)
        {
            //expected payload not yet set
            //allocate memory for payload data
            parser->payload_expected = parser->params[0] * (parser->params[2] - parser->params[1] + 1) * x  ;
            parser->payload = (uint8_t *) malloc(parser->payload_expected * sizeof(uint8_t));
        }

        if(parser->payload_received < parser->payload_expected){
            parser->payload[parser->payload_received] = byte; 
            parser->payload_received += 1;
        }else{
            parser->has_payload = false;
        }
    }
    else
    {
        //finished reading payload. ignore the sent byte as it was already written to buffer.
        //Enter the font data into printer->font_config
        //deallocate memory for the payload data
            if(parser->params[1] < 32 || parser->params[2] > 126 || parser->params[1] > parser->params[2]) return; // we will only handle user defined characters in the printable ascii range for now
        for(int code = parser->params[1] ; code <= parser->params[2] ; code++)
        {
            int char_index = code - 32;
            uint8_t x = *parser->payload++ ;
            if( x > (printer->font_config.currentFontIsA ? 12 : 9) )
                x = printer->font_config.currentFontIsA ? 12 : 9; // we will only use the first 12 or 9 bits/dots for font A and B respectively, any extra bits will be ignored
                UserChar * uc = printer->font_config.currentFontIsA ? &printer->font_config.userDefinedFontA[char_index] : &printer->font_config.userDefinedFontB[char_index];
                uc->width = x;
                uc->height = 24; // height is always 24 bits/dots for this
                memset(uc->data, 0, sizeof(uc->data));
                int bytesPerChar = parser->params[0] * x ;
                for(int col = 0 ; col < x ; col++)
                {
                    for(int row = 0 ; row < parser->params[0] ; row++)
                    {
                        uint8_t byte = *parser->payload++;
                        for(int bit = 0 ; bit < 8 ; bit++)
                        {
                            if(byte & (1 << (7 - bit)))
                            {
                                uc->data[row * 8 + bit][col] = 1;
                            }
                        }
                    }
                }
            }
    }
}