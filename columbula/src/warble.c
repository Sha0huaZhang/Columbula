#include <stdio.h>
#include <string.h>

#include "warble.h"
#include "platform.h"
#include "warble_table.h"     /* 由 lyrics.txt 生成 */

/* 只跳过 ASCII 标点；空格和多字节字节一律放行 */
static int is_punct(unsigned char c)
{
    if (c < 0x21 || c > 0x7e) return 0;
    if (c >= '0' && c <= '9') return 0;
    if (c >= 'A' && c <= 'Z') return 0;
    if (c >= 'a' && c <= 'z') return 0;
    return 1;
}

static int utf8_len(unsigned char c)
{
    if (c < 0x80) return 1;
    if ((c >> 5) == 0x06) return 2;
    if ((c >> 4) == 0x0e) return 3;
    if ((c >> 3) == 0x1e) return 4;
    return 1;
}

static size_t nlines(void)
{
    return sizeof WARBLE_LINES / sizeof *WARBLE_LINES;
}

static void sing_line(const char *line, long char_ms)
{
    const unsigned char *p = (const unsigned char *)line;

    while (*p) {
        int n;

        if (is_punct(*p)) { p++; continue; }
        n = utf8_len(*p);
        fwrite(p, 1, (size_t)n, stdout);
        fflush(stdout);
        p += n;
        plat_nap(char_ms);
    }
    putchar('\n');
    fflush(stdout);
    plat_nap(char_ms * 2);              /* 换行 = 两个字符的时间 */
}

void warble_sing(long char_ms)
{
    size_t i;

    fputs("\033[38;5;74m", stdout);     /* 主色 */
    for (i = 0; i < nlines(); i++)
        sing_line(WARBLE_LINES[i], char_ms);
    fputs("\033[0m", stdout);
    fflush(stdout);
}

void warble_plain(void)
{
    size_t i;

    for (i = 0; i < nlines(); i++) {
        const char *p;
        for (p = WARBLE_LINES[i]; *p; p++)
            if (!is_punct((unsigned char)*p))
                putchar(*p);
        putchar('\n');
    }
}
