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
//unsigned int marche = 0;

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
//        setMarche();
//        if (marche == 1){
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
        //}
    } // fin main
}

void OperatingSystemLoop(void) {
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
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
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
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreDroit < 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche > 30) //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche < 30) //Obstacle à gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 20) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche > 30)
    {//pas d?obstacle
        if (robotState.distanceTelemetreExtremeGauche < 5)
            positionObstacle = OBSTACLE_EXTREME_GAUCHE;
        else if (robotState.distanceTelemetreExtremeDroit < 5)
            positionObstacle = OBSTACLE_EXTREME_DROITE;
        else
        positionObstacle = PAS_D_OBSTACLE;
    }
    //éDtermination de lé?tat àvenir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else if (positionObstacle == OBSTACLE_EXTREME_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EXTREME_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    
    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

//void setMarche()
//{
//    if (PORTDbits.RD5 == 0) 
//    {
//        marche = 1;
//    }
//
//}