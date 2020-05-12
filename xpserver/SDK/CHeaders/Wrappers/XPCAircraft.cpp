#include "XPCAircraft.h"
	
XPCAircraft::XPCAircraft(
					const char *			inICAOCode,
					const char *			inLivery)
{
	mPlane = XPLMCreatePlane(inICAOCode, inLivery, AircraftCB,
					reinterpret_cast<void *>(this));
}
					
XPCAircraft::~XPCAircraft()
{
	XPLMDestroyPlane(mPlane);
}

XPLMPlaneCallbackResult	XPCAircraft::AircraftCB(
										XPLMPlaneID			inPlane,
										XPLMPlaneDataType	inDataType,
										void *				ioData,
										void *				inRefcon)
{
	XPCAircraft * me = reinterpret_cast<XPCAircraft *>(inRefcon);
	switch(inDataType) {
	case xplmDataType_Position:
		return me->GetPlanePosition((XPLMPlanePosition_t *) ioData);
	case xplmDataType_Surfaces:
		return me->GetPlaneSurfaces((XPLMPlaneSurfaces_t *) ioData);
	case xplmDataType_Radar:
		return me->GetPlaneRadar((XPLMPlaneRadar_t *) ioData);
	default:
		return xplmData_Unavailable;
	}
}
