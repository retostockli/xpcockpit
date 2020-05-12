#ifndef _XPCAircraft_h_
#define _XPCAircraft_h_

#include "XPLMMultiplayer.h"

class	XPCAircraft {
public:
	
									XPCAircraft(
										const char *			inICAOCode,
										const char *			inLivery);
	virtual							~XPCAircraft();
	
	virtual	XPLMPlaneCallbackResult	GetPlanePosition(
										XPLMPlanePosition_t *	outPosition)=0;
										
	virtual	XPLMPlaneCallbackResult	GetPlaneSurfaces(
										XPLMPlaneSurfaces_t *	outSurfaces)=0;

	virtual	XPLMPlaneCallbackResult	GetPlaneRadar(
										XPLMPlaneRadar_t *	outRadar)=0;	
protected:

		XPLMPlaneID			mPlane;

	static	XPLMPlaneCallbackResult	AircraftCB(
										XPLMPlaneID			inPlane,
										XPLMPlaneDataType	inDataType,
										void *				ioData,
										void *				inRefcon);

};	

#endif
