#include "test.h"

void fn_test_underline(FILE *fp){
    uint8_t data[] = {
        0x1B, 0x2D, 0x80, // Enable underline mode
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', // Text to print
        0x1B, 0x2D, 0x00 // Disable underline mode
    }; 
    fwrite(data, sizeof(uint8_t), sizeof(data), fp);
}