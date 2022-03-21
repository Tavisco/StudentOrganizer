#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: ManageClassFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     menu item id
 */
Boolean ManageClassFormDoCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case ManageClassDoneButton:
			FrmGotoForm (ClassesForm);
			handled = true;
			break;
			
		case ManageClassCancelButton:
			FrmGotoForm (ClassesForm);
			handled = true;
			break;
			
		case ManageClassStartSelectorTrigger:
			AskTimeToUser(ManageClassStartSelectorTrigger);
			
			handled = true;
			break;
			
		case ManageClassFinishSelectorTrigger:
			AskTimeToUser(ManageClassFinishSelectorTrigger);
			handled = true;
			break;
		case ManageClassHasClassCheckbox:
			ToggleTimeSelectorTrigger();
			handled = true;
			break;
			
		default:
			break;
	}

	return handled;
}

void ToggleTimeSelectorTrigger() {
	ControlType *startSelTrigger = GetObjectPtr(ManageClassStartSelectorTrigger);
	ControlType *finishSelTrigger = GetObjectPtr(ManageClassFinishSelectorTrigger);
	
	CtlSetEnabled(startSelTrigger, !CtlEnabled(startSelTrigger));
	CtlSetEnabled(finishSelTrigger, !CtlEnabled(finishSelTrigger));
}

void AskTimeToUser(UInt16 field) {
	Boolean ok = false;
	DateTimeType dateTime;
	Int16 hour, minute;
	MemHandle hourH, minuteH;
	
	TimSecondsToDateTime(TimGetSeconds(), &dateTime);
	hour = dateTime.hour;
	minute = dateTime.minute;
			
	ok = SelectOneTime(&hour, &minute, "Select time");
	
	if (ok) {
		ControlType *fldP = GetObjectPtr(field);
		Char *cHour, *cMinute;
		
		hourH = MemHandleNew(64);		minuteH = MemHandleNew(64);
		
		cHour = MemHandleLock(hourH);		cMinute = MemHandleLock(minuteH);
		
		StrIToA(cHour, hour);
		StrIToA(cMinute, minute);
		StrCat(cHour, ":");
		StrCat(cHour, cMinute);
		
		CtlSetLabel(fldP, cHour);
		MemPtrFree(cHour);
		MemPtrFree(cMinute);
	}
}

/*
 * FUNCTION: ManageClassFormInit
 *
 * DESCRIPTION: This routine initializes the ManageClassForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the ManageClass form.
 */
void ManageClassFormInit(FormType *frmP) {

}

/*
 * FUNCTION: ManageClassFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "ManageClassForm" of this 
 * application.
 *
 * PARAMETERS:
 *
 * eventP
 *     a pointer to an EventType structure
 *
 * RETURNED:
 *     true if the event was handled and should not be passed to
 *     FrmHandleEvent
 */
Boolean ManageClassFormHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormPtr frmP;

	switch (eventP->eType) 
	{
		case frmOpenEvent: 
		{
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);
			ManageClassFormInit(frmP);
			handled = true;
			break;
        }    
			
		case ctlSelectEvent:
		{
			return ManageClassFormDoCommand(eventP->data.ctlSelect.controlID);
		}
		
		default:
		{
			break;
		}
	}
    
	return handled;
}