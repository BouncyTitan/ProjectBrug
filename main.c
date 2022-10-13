#include "h_bridge.h"
#include "servo.h"
#ifndef F_CPU
#define F_CPU	16000000ul
#endif
#include <avr/io.h>
#include <util/delay.h>

void Noodstop(void)
{
    if((PINC & (1<<PC5))!=0)
    {
        PORTA |= (1<<PA3);                                  //LED ERROR aan
        PORTA &= ~(1<<PA0);                                 //LED programma loopt uit
        h_bridge_set_percentage(0);
        PORTL |= (1<<PL0);
        while(1)
        {
            if((PINC & (1<<PC6))==0&&(PINC & (1<<PC5))==0)
            {
                break;
            }
        }
    }


}

void Brug_open_of_dicht(void)
{
    if ((PINC & (1<<PC0))==0)                               //Checkt of de brug dicht is
    {
        PORTA &= ~(1<<PA2);                                 //LED brug open uit
        PORTA |= (1<<PA1);                                  //LED brug dicht aan
    }

    if ((PINC & (1<<PC1))==0)                               //Checkt of de brug open is
    {
        PORTA &= ~(1<<PA1);                                 //LED brug dicht uit
        PORTA |= (1<<PA2);                                  //LED brug open aan
    }

    if ((PINC & (1<<PC0))!=0)                               //Checkt of de brug dicht is
    {
        PORTA &= ~(1<<PA1);                                 //LED brug open uit
    }

     if ((PINC & (1<<PC1))!=0)                               //Checkt of de brug open is
    {
        PORTA &= ~(1<<PA2);                                 //LED brug open uit
    }

}

void init(void)
{
    init_h_bridge();
    init_servo();
}


int main (void)
{
    init();

    servo1_set_percentage(50);
    DDRL |= ((1<<PL0)|(1<<PL1)|(1<<PL2)|(1<<PL3)|(1<<PL4)|(1<<PL5)|(1<<PL6));                       //Outputs definïeren
    PINL |= (1<<PL6)|(1<<PL5)|(1<<PL2);
    DDRA |= ((1<<PA0)|(1<<PA1)|(1<<PA2)|(1<<PA3));
    DDRC &= ~((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5)|(1<<PC6));                      //Inputs definïeren
    PORTC |= ((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5)|(1<<PC6));                      //Pullup weerstand activeren
    Brug_open_of_dicht();


    while(1)
    {


        if (((PINC & (1<<PC2))==0)&&(PINC & (1<<PC4))!=0)       //Boot detecteren
        {
            PORTA |= (1<<PA0);                                  //LED programma loopt aan
            PORTL ^= (1<<PL1);                                  //Waarschuwingslichten aan
            _delay_ms (5000);
            for (int i = 50; i != -50; i--)                       //slagbomen sluiten half
            {
                servo1_set_percentage(i);
                servo2_set_percentage(i);
                _delay_ms(25);
            }
            _delay_ms (5000);
            for (int i = -50; i != -100; i--)                       //slagbomen sluiten
            {
                servo1_set_percentage(i);
                servo2_set_percentage(i);
                _delay_ms(25);
            }
            _delay_ms (5000);

            for (; (PINC & (1<<PC1))!=0;)                       //Brug openen
            {
                Noodstop();
                h_bridge_set_percentage(50);
                _delay_ms(50);
                Brug_open_of_dicht();

            }

            h_bridge_set_percentage(0);
            _delay_ms (2000);
            PORTL ^= (1<<PL2)|(1<<PL3);                         //Stoplicht boten op groen
            while(1)                                            //Wachten tot boot gepasseerd is
            {
                if((PINC & (1<<PC3))==0)
                {
                    while(1)
                        if((PINC & (1<<PC3))!=0)
                        {
                            break;
                        }
                    break;
                }
            }

            PORTL ^= (1<<PL2)|(1<<PL3);                         //Stoplicht boten op rood
            _delay_ms(5000);

            for (; ((PINC & (1<<PC0))!=0);)                     //Brug sluit
            {
                Noodstop();
                h_bridge_set_percentage(-100);
                _delay_ms(50);
                Brug_open_of_dicht();

            }

            h_bridge_set_percentage(0);
            _delay_ms (2000);
            for (int i = -100; i != 50; i++)                       //slagbomen openen
            {
                servo1_set_percentage(i);
                servo2_set_percentage(i);
                _delay_ms(25);
            }
            _delay_ms (2000);
            PORTL ^= (1<<PL1 );                                 //Waarschuwingslichten gaan uit
            PORTA ^= (1<<PA0);                                  //LED programma loopt uit


        }

        else if ((PINC & (1<<PC4))==0)                          //Windsnelheid meten
        {
            for(; (PINC & (1<<PC4))==0;)
            {
                PORTL |= (1<<PL4);
                _delay_ms(100);
            }
            PORTL ^= (1<<PL4);
        }


    }

    return 0;
}
