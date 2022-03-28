#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: ClassesFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     menu item id
 */
Boolean ClassesFormDoCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case ClassesDoneButton:
			FrmGotoForm (MainForm);
			handled = true;
			break;
		case ClassesNewButton:
			FrmGotoForm (ManageClassForm);
			handled = true;
			break;
		
		default:
			break;
	}

	return handled;
}

/*
 * FUNCTION: ClassesFormInit
 *
 * DESCRIPTION: This routine initializes the MainForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */
void ClassesFormInit(FormType *frmP) {
	ClassesAutoSelectCurrentDay();
}

/*
 * FUNCTION: ClassesAutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void ClassesAutoSelectCurrentDay() {
	Int16 dayOfWeekInt;
	DateTimeType now;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	dayOfWeekInt = DayOfWeek(now.month, now.day, now.year);
	
	switch (dayOfWeekInt) {
		case 0 :
			activateSelector(ClassesSunPushButton);
			break;
		case 1 :
			activateSelector(ClassesMonPushButton);
			break;
		case 2 :
			activateSelector(ClassesTuesPushButton);
			break;
		case 3 :
			activateSelector(ClassesWedPushButton);
			break;
		case 4 :
			activateSelector(ClassesThursPushButton);
			break;
		case 5 :
			activateSelector(ClassesFriPushButton);
			break;
		case 6 :
			activateSelector(ClassesSatPushButton);
			break;
	}
}


/*
 * FUNCTION: ClassesActivateSelector
 *
 * DESCRIPTION: This routine make the referenced PushButton active.
 *
 * PARAMETERS: 
 * 
 * field
 *     The ID of the selected day of week
 *
 */
void ClassesActivateSelector(UInt16 field) {
	ControlType *ctl = GetObjectPtr(field);
	CtlSetValue(ctl, 1);
}


/*
 * FUNCTION: ClassesFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "ClassesForm" of this 
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
Boolean ClassesFormHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormPtr frmP;

	switch (eventP->eType) 
	{
		case frmOpenEvent: 
		{
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);
			ClassesFormInit(frmP);
			handled = true;
			break;
        }    
			
		case ctlSelectEvent:
		{
			return ClassesFormDoCommand(eventP->data.ctlSelect.controlID);
		}
		
		default:
		{
			break;
		}
	}
    
	return handled;
}