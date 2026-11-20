# 由 lyrics.txt 生成 warble_table.h
# 用法: LC_ALL=C awk -f tools/gen_table.awk lyrics.txt

BEGIN {
    print "/* 由 lyrics.txt 生成，勿手改 */"
    print "static const char *const WARBLE_LINES[] = {"
}

/^[[:space:]]*$/ { next }
/^[[:space:]]*#/ { next }

/["\\]/ {
    printf "gen_table: 第 %d 行含引号或反斜杠，无法嵌入\n", NR > "/dev/stderr"
    bad = 1
    next
}

{ printf "    \"%s\",\n", $0 }

END {
    print "};"
    exit (bad ? 1 : 0)      # 不写 exit bad，mawk 对未定义变量做退出码不稳
}
