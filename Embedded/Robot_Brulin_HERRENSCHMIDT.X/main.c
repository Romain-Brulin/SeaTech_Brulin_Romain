#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"

//unsigned int ADCValue0;
//unsigned int ADCValue1;
//unsigned int ADCValue2;



int main (void){
    /************************************************************************************************/
    //Initialisation oscillateur
    /***********************************************************************************************/
    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer23();
    InitTimer4();
    
    // Configuration des input et output (IO)
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;
    LED_BLANCHE_2 = 0;
    LED_BLEUE_2 = 0;
    LED_ORANGE_2 = 0;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    
    EN_PWM = 1;
    
    InitPWM();
    InitADC1();
    // PWMSetSpeedConsigne(20,MOTEUR_DROIT);
    
    // Boucle Principale
    while(1)
    {
//        unsigned int * result = ADCGetResult();
//        if (ADCIsConversionFinished()){
//            ADCClearConversionFinishedFlag();
//            result = ADCGetResult();
//            ADCValue0 = result[0];
//            ADCValue1 = result[1];
//            ADCValue2 = result[2];
//        }
        
        if (ADCIsConversionFinished() == 1)
        {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
        }
        
        if (robotState.distanceTelemetreDroit > 30){
            LED_ORANGE_2 = 1;
        }
        else{
            LED_ORANGE_2 = 0;
        }
        if (robotState.distanceTelemetreCentre > 30){
            LED_BLEUE_2 = 1;
        }
        else{
            LED_BLEUE_2 = 0;
        }
        if (robotState.distanceTelemetreGauche > 30){
            LED_BLANCHE_2 = 1;
        }
        else{
            LED_BLANCHE_2 = 0;
        }
    } // fin main
}


