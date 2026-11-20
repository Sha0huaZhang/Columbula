#include <stdio.h>
#include <string.h>

#include "warble.h"
#include "platform.h"
#include "warble_table.h"     /* 由 lyrics.txt 生成 */

/* 换行停顿占几个字符的时间 */
#define NAP_LINE_UNITS 6

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

/* 一行要花多少个字符的时间：字符加行尾 ~ ，再加换行停顿 */
static long line_units(const char *line, int tildes)
{
    const unsigned char *p = (const unsigned char *)line;
    long n = 0;

    while (*p) {
        if (is_punct(*p)) { p++; continue; }
        p += utf8_len(*p);
        n++;
    }
    return n + tildes + NAP_LINE_UNITS;
}

/* 按截止时间推进一个字符。系统 sleep 总会多睡一点点（macOS 上约 20%），
   盯着绝对时间点睡，误差就不会一行行累积 */
static void pace(long long *deadline, long ms)
{
    long long left;

    *deadline += ms;
    left = *deadline - plat_now_ms();
    if (left > 0)
        plat_nap((long)left);
}

/* 行尾的 ~ 表示拖腔，每个 ~ 停留一个字符的时间。
   短句唱完补 pad 个空格，空格同样按字符节奏输出，
   于是每行都走到同一列，光标也能看见在动。
   最后一个 pad 和 tail_ms 都是 0：唱完 ~ 就收工，不留尾巴 */
static void sing_line(const char *line, long char_ms, int tildes, long pad,
                      long tail_ms, long long *deadline)
{
    const unsigned char *p = (const unsigned char *)line;

    while (*p) {
        int n;

        if (is_punct(*p)) { p++; continue; }
        n = utf8_len(*p);
        fwrite(p, 1, (size_t)n, stdout);
        fflush(stdout);
        p += n;
        pace(deadline, char_ms);
    }
    while (tildes-- > 0) {
        putchar('~');
        fflush(stdout);
        pace(deadline, char_ms);
    }
    while (pad-- > 0) {
        putchar(' ');
        fflush(stdout);
        pace(deadline, char_ms);
    }
    putchar('\n');
    fflush(stdout);
    if (tail_ms > 0)
        pace(deadline, tail_ms);
}

void warble_sing(long char_ms)
{
    size_t i;
    size_t last = nlines() - 1;
    long max_units = 0;
    long long deadline;

    for (i = 0; i <= last; i++) {
        long units = line_units(WARBLE_LINES[i], i == last ? 2 : 1);

        if (units > max_units) max_units = units;
    }

    deadline = plat_now_ms();
    fputs("\033[38;5;74m", stdout);     /* 主色 */
    for (i = 0; i <= last; i++) {
        int is_last = (i == last);
        int tildes = is_last ? 2 : 1;

        sing_line(WARBLE_LINES[i], char_ms, tildes,
                  is_last ? 0 : max_units - line_units(WARBLE_LINES[i], tildes),
                  is_last ? 0 : char_ms * NAP_LINE_UNITS, &deadline);
    }
    fputs("\033[0m", stdout);
    fflush(stdout);
}

void warble_plain(void)
{
    size_t i;
    size_t last = nlines() - 1;

    for (i = 0; i <= last; i++) {
        const char *p;

        for (p = WARBLE_LINES[i]; *p; p++)
            if (!is_punct((unsigned char)*p))
                putchar(*p);
        putchar('~');
        if (i == last)
            putchar('~');
        putchar('\n');
    }
}
