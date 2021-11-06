#ifndef KEEP_ALIVE_H
#define KEEP_ALIVE_H

// Periferiche
#define N_PERIFERICHE 5
#define FINESTRINO 1
#define LUCI 2
#define PEDALI 3
#define KEYGEN 4
#define DASHBOARD 10

extern char alive_message[8], connectedPeripherals[];
extern int peripheralsIDs[];

void alive_timer_init(int timerID);

void sendHello();
void receiveHello(int id);
void voidHello(int id);

void disconnected();

int checkMsg(char *buff, int id);

#endif // KEEP_ALIVE_H