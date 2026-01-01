#ifndef SOFTDOG_H
#define SOFTDOG_H


#define SOFTDOG_TIMEOUT_MS 3000


#ifdef __cplusplus
extern "C"
{
#endif


int initSoftDog(void);

int feedSoftDog(void);
int checkSoftDog(void);


#ifdef __cplusplus
}
#endif


#endif
