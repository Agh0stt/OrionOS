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

// ANSI escape codes
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"

// UART functions
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

int strncmp(const char *s1, const char *s2, int n) {
while (n > 0 && *s1 && *s1 == *s2) { s1++; s2++; n--; }
return n == 0 ? 0 : *s1 - *s2;
}

// Simple integer to string (for calc output)
void itoa(long n, char *buf, int base) {
char *p = buf;
char *start = buf;
int sign = 0;

if (n < 0 && base == 10) {  
    sign = 1;  
    n = -n;  
}  

if (n == 0) {  
    *p++ = '0';  
} else {  
    while (n > 0) {  
        int digit = n % base;  
        *p++ = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');  
        n /= base;  
    }  
}  

if (sign) *p++ = '-';  
*p = '\0';  

// Reverse the string  
p--;  
while (start < p) {  
    char tmp = *start;  
    *start++ = *p;  
    *p-- = tmp;  
}

}

// Screen control
void clear_screen(void) {
puts("\033[2J\033[H");  // ANSI: Clear screen and home cursor
}

void print_centered(const char *s) {
int term_width = 80;  // Assume standard width
int padding = (term_width - strlen(s)) / 2;
for (int i = 0; i < padding; i++) putchar(' ');
puts(s);
puts("\n");
}

// Fancy colored prompt
void print_prompt(void) {
puts(ANSI_GREEN ANSI_BOLD "OrionOS" ANSI_RESET);
puts(ANSI_CYAN "@" ANSI_RESET);
puts(ANSI_YELLOW "aarch64" ANSI_RESET);
puts(ANSI_MAGENTA " ~ " ANSI_RESET);
puts(ANSI_BLUE "→ " ANSI_RESET);
}

// Commands
void version(void) {
puts(ANSI_CYAN ANSI_BOLD "OrionOS v1.0" ANSI_RESET " — Abhigyan Ghosh 2026\n");
puts("Tiny aarch64 kernel running on QEMU\n");
puts("Type '" ANSI_YELLOW "help" ANSI_RESET "' for available commands.\n");
}

void clear(void) {
clear_screen();
}

void help(void) {
puts("Available commands:\n");
puts("  " ANSI_YELLOW "help"     ANSI_RESET "     - show this help\n");
puts("  " ANSI_YELLOW "clear"    ANSI_RESET "    - clear the screen\n");
puts("  " ANSI_YELLOW "echo"     ANSI_RESET " [text] - print text back\n");
puts("  " ANSI_YELLOW "calc"     ANSI_RESET " [expr] - simple calculator (e.g. 5 + 3 * 2)\n");
puts("  " ANSI_YELLOW "version"  ANSI_RESET "  - show OS version\n");
puts("  " ANSI_YELLOW "reboot"   ANSI_RESET "   - reboot system\n");
puts("  " ANSI_YELLOW "halt"     ANSI_RESET "     - halt system\n");
puts("  " ANSI_YELLOW "shutdown" ANSI_RESET "  - same as halt\n");
puts("  " ANSI_YELLOW "kpanic"   ANSI_RESET "   - trigger kernel panic (test)\n");
}

void echo(const char *cmd) {
const char *text = cmd + 5;           // skip "echo"
while (*text == ' ') text++;          // skip spaces
if (*text) {
puts(text);
puts("\n");
} else {
puts("Usage: echo some text here\n");
}
}

void calc(const char *cmd) {
    const char *p = cmd + 5;   // skip "calc "
    while (*p == ' ') p++;

    long a = 0, b = 0;
    char op = 0;

    // Parse first number
    while (*p >= '0' && *p <= '9') {
        a = a * 10 + (*p - '0');
        p++;
    }

    // Operator
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
        op = *p++;
    } else {
        puts("Invalid expression\n");
        return;
    }

    // Parse second number
    while (*p >= '0' && *p <= '9') {
        b = b * 10 + (*p - '0');
        p++;
    }

    long result = 0;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                puts(ANSI_RED "Division by zero!" ANSI_RESET "\n");
                return;
            }
            result = a / b;
            break;
    }

    char buf[32];
    itoa(result, buf, 10);
    puts("Result: ");
    puts(buf);
    puts("\n");
}
// PSCI calls via SMC (for reboot/poweroff)
#define PSCI_FN_RESET 0x84000009  // System reset

void psci_call(uint64_t fn) {
register uint64_t x0 asm("x0") = fn;
asm volatile("smc #0" : "+r"(x0) :: "memory");
}

void halt(void) {
puts("System halted.\n");
while (1) asm volatile("wfi");
}

void reboot(void) {
puts("Rebooting...\n");
psci_call(PSCI_FN_RESET);
halt();  // Fallback if PSCI fails
}
void kpanic(void) {
puts(ANSI_RED ANSI_BOLD "\n!!! KERNEL PANIC !!!\n" ANSI_RESET);
puts("Critical error detected. Rebooting...\n");
// In real kernel you'd print registers/stack etc.
reboot();
}

// Main shell
void shell(void) {
char cmd[64];
while (1) {
print_prompt();
gets(cmd, sizeof(cmd));

if (cmd[0] == '\0') continue;  

    if (strcmp(cmd, "help") == 0)          help();  
    else if (strcmp(cmd, "clear") == 0)     clear();  
    else if (strcmp(cmd, "version") == 0)   version();  
    else if (strcmp(cmd, "halt") == 0)      halt();  
    else if (strcmp(cmd, "reboot") == 0)    reboot();  
    else if (strcmp(cmd, "kpanic") == 0)    kpanic();  
    else if (strcmp(cmd, "shutdown") == 0 ||  
             strcmp(cmd, "poweroff") == 0)  halt();  
    else if (strncmp(cmd, "echo ", 5) == 0) echo(cmd);  
    else if (strncmp(cmd, "calc ", 5) == 0) calc(cmd);  
    else {  
        puts(ANSI_YELLOW "Unknown command. Type 'help' for list." ANSI_RESET "\n");  
    }  
}

}

// Kernel entry
void kmain(void) {
clear_screen();
print_centered("OrionOS - aarch64");
puts("\nBooting kernel...\n\n");
version();
shell();
}
