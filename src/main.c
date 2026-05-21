

#include "emulator.h"
#include "emulator_parser.h"
#include "emulator_printer.h"
#include "emulator_logger.h"

volatile sig_atomic_t running = 1;
void sigint_handler(int signum) {
	running = 0;
}

int main(){

	//initializing a signal handler
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	int master_fd, slave_fd;
	char slave_name[100];

	// Initializing print buffer 
	PrintBuffer print_buffer_object = {
		.size = 4096*1024,
		.current_pointer_position = 0,
		.data = (char *)malloc(4096*1024)
	};
	//Instatiating parser and printer state objects with default values
	EscPosParser parser_object  = {
		.state = STATE_NORMAL,
		.prefix = 0,
		.command = 0,
		.params = {0},
		.params_expected = 0,
		.params_received = 0,
		.user_defined_params = {0},
		.user_defined_params_expected = 0,
		.user_defined_params_received = 0,
		.has_payload = false,
		.payload = NULL,
		.payload_expected = 0,
		.payload_received = 0
	};
	PrinterState printer_state = {
		.bold = false,
		.alignment = 0,
		.font = 0,
		.double_width = false,
		.double_height = false,
		.print_buffer = &print_buffer_object,
		.font_config = {
			.currentFontIsA = true
		}
	};
	//writing the initial HTML tags to the print buffer and moving the buffer pointer to the end of the string
	fn_emulator_printer_write_string_to_buffer("<html><body>", &printer_state);
	//creating a pointer to the parser and printer state objects to pass to the feeder function
	EscPosParser * parser= &parser_object; 
	PrinterState * printer_st = &printer_state;
	FILE * log_file;
	log_file = fopen("log_file", "a+");	

	if(openpty( &master_fd, &slave_fd, slave_name, NULL, NULL ) < 0)
	{
		perror("openpty");
		//didn't find any available terminals
		if(errno == EACCES){
			printf("Permission denied while creating pseudo terminal. Please run the emulator with appropriate permissions.\n");
		}
		fn_emulator_cleanup(log_file, master_fd, slave_fd);
		exit(EXIT_FAILURE);

	}
	
	
	if( symlink( slave_name, "/tmp/pos_printer" ) < 0 )
	{
		int symlink_error = errno;
		//didn't create symbolic link either due to access prohibition
		if(symlink_error == EACCES) {
			printf("Permission denied while creating symbolic link. Please run the emulator with appropriate permissions.\n");
			fn_emulator_cleanup(log_file, master_fd, slave_fd);
			exit(EXIT_FAILURE);
			
		} 
	}
	printf("Emulator started. Connect to the printer using /tmp/pos_printer\n");
	while( running )
	{
		char buffer[256];
		int bytes_read = read( master_fd, buffer, sizeof(buffer) );
		if(bytes_read > 0)
		{
			fn_emulator_parser_feeder(parser, (uint8_t *)buffer, bytes_read, log_file, printer_st);
		}

	}
	fn_emulator_printer_write_string_to_buffer("</body></html>", printer_st);
	fn_emulator_reinitialize_print_buffer_pointer(printer_st->print_buffer);
	fn_emulator_printer_print_html(printer_st->print_buffer->data);
	fn_emulator_cleanup(log_file, master_fd, slave_fd);
	return 0;
}

void fn_emulator_reinitialize_print_buffer_pointer(PrintBuffer * buffer){
	buffer->data -= buffer->current_pointer_position;
	buffer->current_pointer_position = 0;
}
void fn_emulator_move_buffer_pointer(PrintBuffer * buffer, size_t offset){
	buffer->data += offset;
	buffer->current_pointer_position += offset;
	buffer->size += offset;
}
void fn_emulator_decrease_buffer_pointer_position(PrintBuffer * buffer, size_t offset){
	buffer->data -= offset;
	buffer->current_pointer_position -= offset;
}
void fn_emulator_cleanup(FILE * logfile, int mas_fd, int slave_fd){
	unlink("/tmp/pos_printer");
	fclose(logfile);
	close(mas_fd);
	close(slave_fd);
	return ;
}