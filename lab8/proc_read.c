#include <asm/segment.h>  // put_fs_byte
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // process-related variables: task struct, FIRST_TASK
#include <stdarg.h>       //  vs_start va_end
#include <unistd.h>

char krnbuf[1024] = {'\0'};

extern int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    int i;
    // initalize args to first changeable parameter
    va_start(args, fmt);
    // vsprintf function returns the number of characters written to the buffer
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

int get_psinfo() {
    int buf_offset = 0;
    buf_offset += sprintf(krnbuf + buf_offset, "%s",
                          "pid\tstate\tfather\tcounter\tstart_time\n");
    struct task_struct **p;
    for (p = &LAST_TASK; p >= &FIRST_TASK; --p) {
        if (*p != NULL) {
            buf_offset += sprintf(krnbuf + buf_offset, "%d\t", (*p)->pid);
            buf_offset += sprintf(krnbuf + buf_offset, "%d\t", (*p)->state);
            buf_offset += sprintf(krnbuf + buf_offset, "%d\t", (*p)->father);
            buf_offset += sprintf(krnbuf + buf_offset, "%d\t", (*p)->counter);
            buf_offset +=
                sprintf(krnbuf + buf_offset, "%d\n", (*p)->start_time);
        }
    }
    return buf_offset;
}

int proc_read(int dev, off_t *pos, char *buf, int count) {
    int i;
    if (*pos / BLOCK_SIZE == 0)  // already read a whole block, start write
    {
        get_psinfo();
    }
    for (i = 0; i < count; ++i) {
        if (krnbuf[*pos + i] == '\0') break;  // end of message
        put_fs_byte(krnbuf[*pos + i],
                    buf + i + *pos);  // synchronous replication
    }
    *pos += i;
    return i;  // return the actual number of bytes read
}

