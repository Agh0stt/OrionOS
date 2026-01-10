/* Project Orion - OrionOS - Abhigyan Ghosh
* Lisenced Under the MIT lisence
* Copyright - 2026 - present Abhigyan Ghosh
* Project OrionOS - Aarch64
* kernel.c - kernel and shell
*/
typedef unsigned long size_t;
typedef unsigned char uint8_t;
typedef unsigned long uint64_t;

// UART base address for QEMU virt (PL011)
volatile uint8_t *uart = (uint8_t *)0x09000000;

// UART registers offsets
#define UART_FR 0x18  // Flag register
#define UART_DR 0x00  // Data register

// Flags
#define FR_RXFE (1 << 4)  // RX FIFO empty
#define FR_TXFF (1 << 5)  // TX FIFO full

void putchar(char c) {
    while (uart[UART_FR] & FR_TXFF);  // Wait if TX full
    uart[UART_DR] = c;
}

void puts(const char *s) {
    while (*s) {
        if (*s == '\n') putchar('\r');  // CR before LF for terminals
        putchar(*s++);
    }
}

char getchar() {
    while (uart[UART_FR] & FR_RXFE);  // Wait for RX data
    return uart[UART_DR];
}

void gets(char *buf, int max_len) {
    int i = 0;
    char c;
    while (i < max_len - 1 && (c = getchar()) != '\r' && c != '\n') {
        putchar(c);  // Echo
        buf[i++] = c;
    }
    buf[i] = '\0';
    puts("\n");
}

// Minimal string functions
int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return *s1 - *s2;
}

void memcpy(char *dest, const char *src, int n) {
    for (int i = 0; i < n; i++) dest[i] = src[i];
}

// PSCI calls via SMC (for reboot/poweroff)
#define PSCI_FN_RESET 0x84000009  // System reset

void psci_call(uint64_t fn) {
    register uint64_t x0 asm("x0") = fn;
    asm volatile("smc #0" : "+r"(x0) :: "memory");
}

void halt() {
    puts("System halted.\n");
    while (1) asm volatile("wfi");
}

void reboot() {
    puts("Rebooting...\n");
    psci_call(PSCI_FN_RESET);
    halt();  // Fallback if PSCI fails
}

void kpanic() {
    puts("Kernel panic Robooting\n");
    reboot();
}

void version() {
    puts("OrionOS v1.0\n");
}

void clear_screen() {
    puts("\033[2J\033[H");  // ANSI: Clear screen and home cursor
}

void print_centered(const char *s) {
    int term_width = 80;  // Assume standard width
    int padding = (term_width - strlen(s)) / 2;
    for (int i = 0; i < padding; i++) putchar(' ');
    puts(s);
    puts("\n");
}

void shell() {
    char cmd[64];
    while (1) {
        puts("OrionOS> ");
        gets(cmd, sizeof(cmd));

        if (strcmp(cmd, "halt") == 0) halt();
        else if (strcmp(cmd, "version") == 0) version();
        else if (strcmp(cmd, "kpanic") == 0) kpanic();
        else if (strcmp(cmd, "reboot") == 0) reboot();
        else if (strcmp(cmd, "shutdown") == 0 || strcmp(cmd, "poweroff") == 0) halt();
        else if (cmd[0] != '\0') puts("Unknown command.\n");
    }
}

void kmain() {
    clear_screen();
    print_centered("OrionOS - aarch64");
    puts("Booting kernel...\n");
    shell();
}
