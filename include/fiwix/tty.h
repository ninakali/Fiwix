/*
 * fiwix/include/fiwix/tty.h
 *
 * Copyright 2018, Jordi Sanfeliu. All rights reserved.
 * Distributed under the terms of the Fiwix License.
 */

#ifndef _FIWIX_TTY_H
#define _FIWIX_TTY_H

#include <fiwix/termios.h>
#include <fiwix/fs.h>

#define NR_TTYS		15	/* maximum number of tty devices */

#define CBSIZE		32	/* number of characters in cblock */
#define NR_CB_QUEUE	10	/* number of cblocks per queue */
#define CB_POOL_SIZE	128	/* number of cblocks in the central pool */

#define LAST_CHAR(q)	((q)->tail ? (q)->tail->data[(q)->tail->end_off - 1] : NULL)

struct clist {
	unsigned short int count;
	unsigned short int cb_num;
	struct cblock *head;
	struct cblock *tail;
};

struct cblock {
	unsigned short int start_off;
	unsigned short int end_off;
	unsigned char data[CBSIZE];
	struct cblock *prev;
	struct cblock *next;
};

struct tty {
	__dev_t dev;
	struct clist read_q;
	struct clist cooked_q;
	struct clist write_q;
	unsigned short int count;
	struct termios termios;
	struct winsize winsize;
	__pid_t pid, pgid, sid;
	unsigned char lnext;
	void *driver_data;
	int canon_data;

	/* formerly tty_operations */
	void (*stop)(struct tty *);
	void (*start)(struct tty *);
	void (*deltab)(struct tty *);
	void (*reset)(struct tty *);
	void (*input)(struct tty *);
	void (*output)(struct tty *);
};
extern struct tty tty_table[];

int register_tty(__dev_t);
struct tty * get_tty(__dev_t);
void disassociate_ctty(struct tty *);
void termios_reset(struct tty *);
void do_cook(struct tty *);
int tty_putchar(struct tty *, unsigned char);
int tty_open(struct inode *, struct fd *);
int tty_close(struct inode *, struct fd *);
int tty_read(struct inode *, struct fd *, char *, __size_t);
int tty_write(struct inode *, struct fd *, const char *, __size_t);
int tty_ioctl(struct inode *, int cmd, unsigned long int);
int tty_lseek(struct inode *, __off_t);
int tty_select(struct inode *, int);
void tty_init(void);

int tty_queue_putchar(struct tty *, struct clist *, unsigned char);
int tty_queue_unputchar(struct clist *);
unsigned char tty_queue_getchar(struct clist *);
void tty_queue_flush(struct clist *);
void tty_queue_init(struct tty *);

int vt_ioctl(struct tty *, int, unsigned long int);

#endif /* _FIWIX_TTY_H */