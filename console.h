<<<<<<< HEAD
#define SBUFSIZE 16
typedef struct 
{
	volatile unsigned head;
	volatile unsigned tail;
	volatile unsigned count;
	char buffer[SBUFSIZE];
} SerialBuffer;

void putBuf(SerialBuffer *sbuf, char c);
char getBuf(SerialBuffer *sbuf);
int rx_count();
char egetc(void);
void eputc(const char c);
void eputs(const char *s);
void printString(const char *s);
void printHex(unsigned int Number);
void flushinput();
=======
#define SBUFSIZE 16
typedef struct 
{
	volatile unsigned head;
	volatile unsigned tail;
	volatile unsigned count;
	char buffer[SBUFSIZE];
} SerialBuffer;

void putBuf(SerialBuffer *sbuf, char c);
char getBuf(SerialBuffer *sbuf);
int rx_count();
char egetc(void);
void eputc(const char c);
void eputs(const char *s);
void printString(const char *s);
void printHex(unsigned int Number);
void flushinput();
>>>>>>> 1eb23f5f8c2454e5c6a7b51e8fe09242c47e24f6
