#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/********************************************
 *
 *             GLOBAL VARIABLES
 ********************************************/

Boolean visible = false;

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
	FormType *formP = FrmGetActiveForm();
	UInt16 startLabelIndex = FrmGetObjectIndex(formP, ManageClassStartLabel);
	UInt16 startSelectorIndex = FrmGetObjectIndex(formP, ManageClassStartSelectorTrigger);
	UInt16 finishLabelIndex = FrmGetObjectIndex(formP, ManageClassFinishLabel);
	UInt16 finishSelectorIndex = FrmGetObjectIndex(formP, ManageClassFinishSelectorTrigger);
	
	
	if (visible) {
		FrmHideObject(formP, startLabelIndex);
		FrmHideObject(formP, startSelectorIndex);
		FrmHideObject(formP, finishSelectorIndex);
		FrmHideObject(formP, finishLabelIndex);
	} else {
		FrmShowObject(formP, startLabelIndex);
		FrmShowObject(formP, startSelectorIndex);
		FrmShowObject(formP, finishSelectorIndex);
		FrmShowObject(formP, finishLabelIndex);
	}
	
	visible = !visible;
	//CtlSetEnabled(startSelTrigger, !CtlEnabled(startSelTrigger));
	//CtlSetEnabled(finishSelTrigger, !CtlEnabled(finishSelTrigger));
}

void AskTimeToUser(UInt16 field) {
	Boolean ok = false;
	DateTimeType now;
	Int16 hour, minute;
	char timeStr[timeStringLength];
	
	TimSecondsToDateTime(TimGetSeconds(), &now);
	hour = now.hour;
	minute = now.minute;
			
	ok = SelectOneTime(&hour, &minute, "Select time");
	
	if (ok) {
		ControlType *fldP = GetObjectPtr(field);
		TimeToAscii(hour, minute, tfColon24h, timeStr);
		CtlSetLabel(fldP, timeStr);
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