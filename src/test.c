#include "test.h"

void fn_test_underline(FILE *fp){
    uint8_t data[] = {
        0x1B, 0x21, 0x80, // Enable underline mode
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', // Text to print
        0x1B, 0x2D, 0x00 // Disable underline mode
    }; 
    fwrite(data, sizeof(uint8_t), sizeof(data), fp);
}
void fn_test_underline_mode_2 (FILE * fp){
    uint8_t data[] = {
        0x1B, 0x2D, 0x01, //activate the first underline mode
        't','h','i','s',' ','t','e','x','t',' ','s','h','o','u','l','d',' ','h','a','v','e',' ','a',' ','f',
        'a','i','n','t',' ','u','n','d','e','r','l','i','n','e', 0x1B, 0x2D, 0x00, 'w','h','i','l','e',' ',
        0x1B, 0x2D, 0x02, 't','h','i','s',' ','i','s',' ','t','h','i','c','k','e','r', 0x1B, 0x2D, 0x00
    };
    fwrite(data, sizeof(uint8_t), sizeof(data), fp);
}
void fn_test_justification(FILE *file){
	uint8_t test_data[] = {
	0x1B, 0x61, 0x00, 'l','e','f','t', 0x1B, 0x61, 0x01, 'c',
	'e','n','t','e','r', 0x1B, 0x61, 0x02, 'r','i','g','h','t'
	};
	fwrite(test_data, sizeof(uint8_t), sizeof(test_data), file);
	return;
}
