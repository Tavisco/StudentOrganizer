#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/********************************************
 *
 *             GLOBAL VARIABLES
 *
 ********************************************/

Boolean visibleSelectors = false; // Handle if time selectors are shown
Int16 selectedDoW; // Selected Day of Week by the pushbuttons
ClassDB record; // Current database record
Int16 dowPushButtons[7] = {ManageClassSunPushButton, ManageClassMonPushButton, ManageClassTuesPushButton, ManageClassWedPushButton, ManageClassThursPushButton, ManageClassFriPushButton, ManageClassSatPushButton};

/*
 * FUNCTION: ManageClassFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     form item id
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
			AskTimeToUser(ManageClassStartSelectorTrigger); // make this return time
			// with the returned data, create function to parse it to inner object
			// with the inner object, correctly attribute it to the DB here.
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
		
		case ManageClassSunPushButton:
			selectedDoW = 0;
			LoadDoW();
			handled = true;
			break;
		
		case ManageClassMonPushButton:
			selectedDoW = 1;
			LoadDoW();
			handled = true;
			break;
		
		case ManageClassTuesPushButton:
			selectedDoW = 2;
			LoadDoW();
			handled = true;
			break;
			
		case ManageClassWedPushButton:
			selectedDoW = 3;
			LoadDoW();
			handled = true;
			break;
			
		case ManageClassThursPushButton:
			selectedDoW = 4;
			LoadDoW();
			handled = true;
			break;
			
		case ManageClassFriPushButton:
			selectedDoW = 5;
			LoadDoW();
			handled = true;
			break;
			
		case ManageClassSatPushButton:
			selectedDoW = 6;
			LoadDoW();
			handled = true;
			break;
			
		default:
			break;
	}

	return handled;
}


void LoadDoW() {

}

/*
 * FUNCTION: ToggleTimeSelectorTrigger
 *
 * DESCRIPTION: This routine shows or hides the time selectors and labels
 *   based on the state of the checkbox
 *
 * PARAMETERS: No parameters
 *
 */
void ToggleTimeSelectorTrigger() {
	FormType *formP = FrmGetActiveForm();
	UInt16 startLabelIndex = FrmGetObjectIndex(formP, ManageClassStartLabel);
	UInt16 startSelectorIndex = FrmGetObjectIndex(formP, ManageClassStartSelectorTrigger);
	UInt16 finishLabelIndex = FrmGetObjectIndex(formP, ManageClassFinishLabel);
	UInt16 finishSelectorIndex = FrmGetObjectIndex(formP, ManageClassFinishSelectorTrigger);
	
	
	if (visibleSelectors) {
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
	
	visibleSelectors = !visibleSelectors;
	record.classOcurrence[selectedDoW].active = visibleSelectors;
}


/*
 * FUNCTION: AskTimeToUser
 *
 * DESCRIPTION: This routine calls a system pop-up that asks the user
 *   to input time, in this case, the class start/finish time.
 *
 *   After a time has been choosen, it then converts it to ASCII and
 *   updates the selector field label to it.
 *
 *
 * PARAMETERS:
 *
 * field
 *     selector item id
 */
void AskTimeToUser(UInt16 field) {
	Boolean ok = false;
	DateTimeType now;
	Int16 hour, minute;
	char timeStr[timeStringLength];
	//ClassOccurrenceDB db;
	
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
	visibleSelectors = false;
	autoSelectCurrentDay();
	LoadDoW();
}


/*
 * FUNCTION: AutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void autoSelectCurrentDay() {
	DateTimeType now;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	selectedDoW = DayOfWeek(now.month, now.day, now.year);
	
	activateSelector(dowPushButtons[selectedDoW]);
}


/*
 * FUNCTION: ActivateSelector
 *
 * DESCRIPTION: This routine make the referenced PushButton active.
 *
 * PARAMETERS: 
 * 
 * field
 *     The ID of the selected day of week
 *
 */
void activateSelector(UInt16 field) {
	ControlType *ctl = GetObjectPtr(field);
	CtlSetValue(ctl, 1);
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