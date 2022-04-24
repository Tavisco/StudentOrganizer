#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"


Boolean MngHmwrkFormDoCommand(UInt16 command)
{
	Boolean handled = false;

	switch (command)
	{
	case CancelMngHmwrkButton:
	{
		FrmGotoForm(MainForm);
		handled = true;
		break;
	}
	}

	return handled;
}


void MngHmwrkFormInit(FormType *frmP)
{
	
}

Boolean MngHmwrkFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;

	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID);
	
	case menuEvent:
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
		MngHmwrkFormInit(frmP);
		handled = true;
		break;
	}

	return handled;
}