#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(){
    size_t y;
    for(y = 0; y < VGA_HEIGHT - 1; y++){
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[vga_index(x,y)] = terminal_buffer[vga_index(x,y+1)];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[vga_index(x,y)] = vga_entry(' ', terminal_color);
    }
}

void terminal_putchar(char c)
{
    if (c == '\n') { //Do not display '\n' code, simply increment row and reset column
		terminal_column = 0;
        ++terminal_row;
        if (terminal_row == VGA_HEIGHT) {
			--terminal_row;
			terminal_scroll();
        }
		return;
	}
	
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
        ++terminal_row;
        if (terminal_row == VGA_HEIGHT) {
			--terminal_row;
			terminal_scroll();
        }
	}

}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
    terminal_write(data,strlen(data));
}

void terminal_writeint(int n)
{
    char buffer[32];
    int i = 0;
    if (n == 0) {
        terminal_putentryat('0', terminal_color, terminal_column, terminal_row);
        terminal_column++;
        return;
    }
    if (n < 0) {
        terminal_putentryat('-', terminal_color, terminal_column, terminal_row);
        terminal_column++;
        n = -n;
    }
    while (n > 0) {
        buffer[i++] = n % 10 + '0';
        n /= 10;
    }
    while (i > 0) {
        terminal_putentryat(buffer[--i], terminal_color, terminal_column, terminal_row);
        terminal_column++;
    }
}

