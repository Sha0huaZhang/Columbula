#ifndef COLUMBULA_WARBLE_H
#define COLUMBULA_WARBLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 逐字唱出，每个字符停 char_ms，换行停 6 * char_ms。行尾加 ~ 表示拖腔，最后一行加 ~~。
   每行总时长统一取最长的一行，短句唱完补空格补足（光标会跟着走到同一列）；
   最后一行只补 ~~ ，不补空格、不额外停顿 */
void warble_sing(long char_ms);

/* 非终端场景：一次性输出纯文本，无颜色无停顿 */
void warble_plain(void);

#ifdef __cplusplus
}
#endif

#endif
