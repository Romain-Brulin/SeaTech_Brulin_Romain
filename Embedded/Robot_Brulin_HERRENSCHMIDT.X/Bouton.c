#include <xc.h>

#include <xc.h>

void initBouton(void)
{
    TRISHbits.TRISH1 = 1;    // RH1 en entrée
    //ANSELBbits.ANSH1 = 0;    // Désactive l'analogique sur RH1
    // Pas de pull-up interne possible => résistance externe obligatoire
}

int boutonPressed(void)
{
    return (PORTHbits.RH1 == 0);  // 0 = bouton appuyé (car pull-up externe)
}