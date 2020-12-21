#ifndef SERIAL_H
#define SERIAL_H

#include <inttypes.h>
#include <stdbool.h>

/* macros SER_ONLY_TX and SER_ONLY_RX allow to choose only
 * half serial communication during compile time.
 * Default: both tx and rx
 */
#if defined(SER_ONLY_TX) || !defined(SER_ONLY_RX)
#define _SER_TX_
#endif

#if defined(SER_ONLY_RX) || !defined(SER_ONLY_TX)
#define _SER_RX_
#endif




void serial_setup(void);

void serial_open(void);
void serial_close(void);

#ifdef _SER_RX_
bool serial_can_read(void);
uint8_t  serial_read(void);
#endif

#ifdef _SER_TX_
bool serial_can_write(void);
void serial_write(uint8_t c);

void serial_eol(void);
void serial_write_s(char t[]);
void serial_write_ui(unsigned int i);

#endif

#endif
