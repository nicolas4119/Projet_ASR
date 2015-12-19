#include <kernel.h>
#include <queue.h>
#include <sema.h>

// Writes OSticks to the serial port every 100ms.

void task1(void *p)
{
    char buffer[16];
    while(1)
    {
        sprintf(buffer, "Time: %lu", OSticks());
        Serial.println(buffer);
        OSSleep(1000);
    }
}

// Reads the potentiometer at analog input 0 every 10 ms and places

// it onto the message queue.

#define QLEN 8

int qbuf[QLEN];

OSQueue msg;

// Reads the message queue and controls the brightness of the LED at
//PWM output 11.

void task2(void *p)
{
    unsigned int val;

    while(1)

    {
        val=(unsigned int) OSDequeue(&msg);
        analogWrite(11, val);
    }
}

void task3(void *p)

{
    unsigned int val, remap;

    while(1)
    {
        val=analogRead(0);
        remap=map(val, 0, 1023, 0, 255);
        OSEnqueue(remap, &msg);
        OSSleep(200);
    }
}

OSSema sem;

// Interrupt service routine for external interrupt INT0

// (digital pin 2)

void int0handler()
{
    OSGiveSema(&sem);
}

// Toggles the LED at pin 9 every time the push button is pressed.

void task4(void *p)
{
    unsigned char flag=HIGH;

    while(1)
    {
        OSTakeSema(&sem);
        digitalWrite(9, flag);
        flag=!flag;
    }
}

void task5(void *p)
{
    unsigned int pause=(unsigned int) p;
    while(1)
    {
        digitalWrite(6, HIGH);
        OSSleep(pause);
        digitalWrite(6, LOW);
        OSSleep(pause);
    }

}

#define NUM_TASKS 5

void setup()

{
    OSInit(NUM_TASKS);

    Serial.begin(115200);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(11, OUTPUT);
    attachInterrupt(0, int0handler, RISING);

    OSCreateSema(&sem,0,1);
    OSCreateQueue(qbuf,QLEN,&msg);

    OSCreateTask(1, task1, NULL);
    OSCreateTask(2, task2, NULL);
    OSCreateTask(4, task3, NULL);
    OSCreateTask(0, task4, NULL);
    OSCreateTask(3, task5, (void *) 250);

    OSRun();
}

void loop()
{
    // Empty
}
