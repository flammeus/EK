#ifndef _API_H
#define _API_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic.h"
#include "node.h"
#include "queue.h"
/*Appellido , Nombre Mail
Granier , Pierre-Yves    flammeus8@gmail.com
Queralt , Ignacio        nachoqueralt@gmail.com
Lusso ,   Jonathan       jonilusso@gmail.com
Drazile , Eduardo andres edudraz@gmail.com
Leberle , Maico          maico.leberle@gmail.com
*/
DragonP NuevoDragon(void);
int DestruirDragon(DragonP N);
int CargarUnLado(DragonP N, u32 x, u32 y, u32 c);
int LlenarDragon(DragonP N);
int ECAML(DragonP N);
int DondeEstamosParados(DragonP N);
u32 AumentarFlujo(DragonP N);
u32 AumentarFlujoYtambienImprimirCamino(DragonP N);
void ImprimirFlujo(DragonP N);
void ImprimirValorFlujo(DragonP N);
void ImprimirCorte(DragonP N);
u64 Sumar64(u64 a, u32 b);

#endif
