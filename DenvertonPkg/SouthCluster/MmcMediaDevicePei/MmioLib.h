/*
 * MmioLib.h
 *
 *  Created on: Mar 4, 2015
 *      Author: kadavis2
 */

#ifndef DENVERTONPKG_SOUTHCLUSTER_MMCMEDIADEVICEPEI_MMIOLIB_H_
#define DENVERTONPKG_SOUTHCLUSTER_MMCMEDIADEVICEPEI_MMIOLIB_H_

#include <Library/MmPciLib.h>
#include <Library/IoLib.h>

#ifndef VOLATILE
#define VOLATILE volatile
#endif
#define MmPci32(Seg, Bus, Dev, Func, Off) (*(VOLATILE UINT32*)(MmPciBase((Bus), (Dev), (Func)) + (Off)))
#define MmPci16(Seg, Bus, Dev, Func, Off) (*(VOLATILE UINT16*)(MmPciBase((Bus), (Dev), (Func)) + (Off)))
#define MmPci8(Seg, Bus, Dev, Func, Off)  (*(VOLATILE UINT8*)(MmPciBase((Bus), (Dev), (Func)) + (Off)))
#define MmPci8AndThenOr(Seg, Bus, Dev, Func, Off, AndMsk, OrMsk) {\
	UINT8 d =  MmPci8((Seg), (Bus), (Dev), (Func), (Off)) & ((UINT8)(AndMsk));\
	 MmPci8((Seg), (Bus), (Dev), (Func), (Off)) = d | ((UINT8)OrMsk);\
}

#endif /* DENVERTONPKG_SOUTHCLUSTER_MMCMEDIADEVICEPEI_MMIOLIB_H_ */
