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
#include "Bouton.h"

//unsigned int ADCValue0;
//unsigned int ADCValue1;
//unsigned int ADCValue2;

unsigned char stateRobot;
unsigned char nextStateRobot = 0;
unsigned int marche = 0;
int robotEnMarche = 0;
int timer;

int main(void) {
    /************************************************************************************************/
    //Initialisation oscillateur
    /***********************************************************************************************/
    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer23();
    InitTimer4();
    //initBouton();

    // Configuration des input et output (IO)
    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 0;
    LED_ORANGE_1 = 0;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 0;
    LED_VERTE_2 = 0;

    EN_PWM = 1;

    InitPWM();
    InitADC1();
    // PWMSetSpeedConsigne(20,MOTEUR_DROIT);

    // Boucle Principale
    while (1) {
        //        unsigned int * result = ADCGetResult();
        //        if (ADCIsConversionFinished()){
        //            ADCClearConversionFinishedFlag();
        //            result = ADCGetResult();
        //            ADCValue0 = result[0];
        //            ADCValue1 = result[1];
        //            ADCValue2 = result[2];
        //        }
        if(!robotEnMarche && _RH0 ==1){
            robotEnMarche = 1;
            timer = timestamp;
            stateRobot = STATE_ATTENTE;
            LED_ROUGE_2 = 1;
        }
        setMarche();
        if (robotEnMarche){
            if (ADCIsConversionFinished() == 1) {
                ADCClearConversionFinishedFlag();
                unsigned int * result = ADCGetResult();
                float volts = ((float) result [1])* 3.3 / 4096;
                robotState.distanceTelemetreGauche = 34 / volts - 5;
                volts = ((float) result [2])* 3.3 / 4096;
                robotState.distanceTelemetreCentre = 34 / volts - 5;
                volts = ((float) result [3])* 3.3 / 4096;
                robotState.distanceTelemetreDroit = 34 / volts - 5;
                volts = ((float) result [4])* 3.3 / 4096;
                robotState.distanceTelemetreExtremeDroit = 34 / volts - 5;
                volts = ((float) result [0])* 3.3 / 4096;
                robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
            }

            if (robotState.distanceTelemetreCentre > 30) {
                LED_ORANGE_1 = 1;
            } else {
                LED_ORANGE_1 = 0;
            }
            if (robotState.distanceTelemetreGauche > 30) {
                LED_BLEUE_1 = 1;
            } else {
                LED_BLEUE_1 = 0;
            }
            if (robotState.distanceTelemetreExtremeGauche > 30) {
                LED_BLANCHE_1 = 1;
            } else {
                LED_BLANCHE_1 = 0;
            }
            if (robotState.distanceTelemetreDroit > 30) {
                LED_ROUGE_1 = 1;
            } else {
                LED_ROUGE_1 = 0;
            }
            if (robotState.distanceTelemetreExtremeDroit > 30) {
                LED_VERTE_1 = 1;
            } else {
                LED_VERTE_1 = 0;
            }
        }
        
        OperatingSystemLoop();
        
        if(robotEnMarche && (timestamp - timer >= 60000)){
            robotEnMarche = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            PWMUpdateSpeed();
            LED_ROUGE_2 = 0;
        }
    } // fin main
}

void OperatingSystemLoop(void) {
    if(!robotEnMarche){
        PWMSetSpeedConsigne(0, MOTEUR_DROIT);
        PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
        return;
    }
    
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
    
}

void SetNextRobotStateInAutomaticMode() {
    
    int capteur0 = robotState.distanceTelemetreExtremeGauche < 15;
    int capteur1 = robotState.distanceTelemetreGauche < 40;
    int capteur2 = robotState.distanceTelemetreCentre < 50;
    int capteur3 = robotState.distanceTelemetreDroit < 40;
    int capteur4 = robotState.distanceTelemetreExtremeDroit < 15;
    
    int positionObstacle = (capteur0 << 4) | (capteur1 << 3) | (capteur2 << 2) | (capteur3 << 1) | capteur4;

    if (positionObstacle == 0b00000){
        nextStateRobot = STATE_AVANCE;
    }
    else if(positionObstacle == 0b00001){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b00010){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b00011){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b00100){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b00101){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b00110){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b00111){
        nextStateRobot = STATE_TOURNE_GAUCHE;
    }
    else if(positionObstacle == 0b01000){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b01001){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b01010){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b01011){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b01100){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b01101){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b01110){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b01111){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b10000){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b10001){
        nextStateRobot = STATE_AVANCE;
    }
    else if(positionObstacle == 0b10010){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b10011){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b10100){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b10101){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b10110){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b10111){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    }
    else if(positionObstacle == 0b11000){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b11001){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b11010){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b11011){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b11100){
        nextStateRobot = STATE_TOURNE_DROITE;
    }
    else if(positionObstacle == 0b11101){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b11110){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    else if(positionObstacle == 0b11111){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }
    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

void setMarche(){
    if (boutonPressed()) 
    {
        marche = 1;
    }
}