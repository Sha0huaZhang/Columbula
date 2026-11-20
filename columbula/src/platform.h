#ifndef COLUMBULA_PLATFORM_H
#define COLUMBULA_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

int  plat_is_tty(void);
void plat_nap(long ms);
void plat_vt_on(void);

#ifdef __cplusplus
}
#endif

#endif
