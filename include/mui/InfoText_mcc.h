/*
**
** $VER: InfoText_mcc.h V15.3
** Copyright © 1997 Benny Kjær Nielsen. All rights reserved.
**
*/

/*** Include stuff ***/

#ifndef INFOTEXT_MCC_H
#define INFOTEXT_MCC_H

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef BKN_SERIAL
#define BKN_SERIAL 0xfcf70000
#endif

/*** MUI Defines ***/

#define MUIC_InfoText "InfoText.mcc"
#define InfoTextObject MUI_NewObject(MUIC_InfoText

/*** Methods ***/
#define MUIM_InfoText_TimeOut          (BKN_SERIAL | 0x101 )

/*** Attributes ***/
#define MUIA_InfoText_Contents         (BKN_SERIAL | 0x110 )
#define MUIA_InfoText_ExpirationPeriod (BKN_SERIAL | 0x111 )
#define MUIA_InfoText_FallBackText     (BKN_SERIAL | 0x112 )


#endif /* INFOTEXT_MCC_H */
