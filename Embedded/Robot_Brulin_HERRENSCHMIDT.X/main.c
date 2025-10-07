#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"


int main (void){
    /************************************************************************************************/
    //Initialisation oscillateur
    /***********************************************************************************************/
    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer23();
    
    // Configuration des input et output (IO)
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;
    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    
    EN_PWM = 1;
    
    InitPWM();
    PWMSetSpeedConsigne(20,MOTEUR_DROIT);
    
    // Boucle Principale
    while(1)
    {
    } // fin main
}


