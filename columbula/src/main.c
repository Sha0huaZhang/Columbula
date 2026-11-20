#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "version.h"
#include "warble.h"

#define CHAR_MS_DEFAULT 50L
#define CHAR_MS_FAST    15L
#define CHAR_MS_MAX     60000L

static void usage(FILE *out, const char *prog)
{
    fprintf(out,
            "用法: %s [选项]\n"
            "\n"
            "  -s, --speed <时长>  每字符停顿。单位 ms 或 s，不写单位按 ms\n"
            "                      例: -s 50ms、-s=1.2s、-s500（默认 %ldms）\n"
            "  -f, --fast          加速，等同 -s %ldms\n"
            "  -p, --plain         强制纯文本输出，无颜色无停顿\n"
            "  -v, --version       显示版本\n"
            "  -h, --help          显示本帮助\n"
            "\n"
            "带选项时：输出到终端就逐字唱，被重定向或接管道就输出纯文本。\n",
            prog, CHAR_MS_DEFAULT, CHAR_MS_FAST);
}

static int eq_nocase(const char *a, const char *b)
{
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;

        if (ca >= 'A' && ca <= 'Z') ca = (unsigned char)(ca - 'A' + 'a');
        if (ca != cb) return 0;
    }
    return *a == *b;
}

/* "50" / "50ms" / "1.2s" -> 毫秒。返回 0 成功，-1 格式错，-2 超范围 */
static int parse_speed(const char *text, long *out_ms)
{
    char *end = NULL;
    double scale = 1.0;
    double value = strtod(text, &end);

    if (end == text) return -1;                        /* 没有数字 */
    if (!(value > 0.0) || value > 1e9) return -1;       /* 负数、NaN、离谱的大数 */

    if (*end != '\0') {
        if (eq_nocase(end, "ms")) scale = 1.0;
        else if (eq_nocase(end, "s")) scale = 1000.0;
        else return -1;                                /* 不认识的单位 */
    }

    value *= scale;
    if (value < 1.0 || value > (double)CHAR_MS_MAX) return -2;
    *out_ms = (long)(value + 0.5);
    return 0;
}

int main(int argc, char **argv)
{
    long char_ms = CHAR_MS_DEFAULT;
    int force_plain = 0;
    int i;

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];
        const char *speed_text = NULL;

        if (strcmp(arg, "-f") == 0 || strcmp(arg, "--fast") == 0) {
            char_ms = CHAR_MS_FAST;
        } else if (strcmp(arg, "-p") == 0 || strcmp(arg, "--plain") == 0) {
            force_plain = 1;
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) {
            puts(COLUMBULA_VERSION);
            return 0;
        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            usage(stdout, argv[0]);
            return 0;
        } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--speed") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: %s 后面要跟一个时长\n", argv[0], arg);
                return 2;
            }
            speed_text = argv[++i];
        } else if (strncmp(arg, "--speed=", 8) == 0) {
            speed_text = arg + 8;
        } else if (strncmp(arg, "-s=", 3) == 0) {
            speed_text = arg + 3;
        } else if (strncmp(arg, "-s", 2) == 0 && arg[2] != '\0') {
            speed_text = arg + 2;
        } else {
            fprintf(stderr, "%s: 未知选项 %s\n", argv[0], arg);
            usage(stderr, argv[0]);
            return 2;
        }

        if (speed_text != NULL) {
            int rc = parse_speed(speed_text, &char_ms);

            if (rc == -2) {
                fprintf(stderr, "%s: 速度 \"%s\" 超出范围，可用 1ms ~ 60s\n",
                        argv[0], speed_text);
                return 2;
            }
            if (rc != 0) {
                fprintf(stderr,
                        "%s: 速度 \"%s\" 不合法，例: -s 50ms、-s=1.2s、-s500（默认单位 ms）\n",
                        argv[0], speed_text);
                return 2;
            }
        }
    }

    if (force_plain || !plat_is_tty()) {
        warble_plain();
        return 0;
    }

    plat_vt_on();
    warble_sing(char_ms);
    return 0;
}
