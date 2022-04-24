#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"


Boolean HomeworksFormDoCommand(UInt16 command)
{
	Boolean handled = false;

	switch (command)
	{
	case HomeworksDoneButton:
	{
		FrmGotoForm(MainForm);
		handled = true;
		break;
	}
    case HomeworksNewButton:
	{
		FrmGotoForm(ManageHomeworkForm);
		handled = true;
		break;
	}
	}

	return handled;
}


void HomeworksFormInit(FormType *frmP)
{
	
}

Boolean HomeworksFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;

	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return HomeworksFormDoCommand(eventP->data.menu.itemID);
	
	case menuEvent:
		return HomeworksFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
		HomeworksFormInit(frmP);
		handled = true;
		break;
	}

	return handled;
}