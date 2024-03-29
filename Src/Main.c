#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: MainFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     menu item id
 */

Boolean MainFormDoCommand(UInt16 command)
{
	Boolean handled = false;

	switch (command)
	{
	case OptionsAboutStudentOrganizer:
	{
		FormType *frmP;

		/* Clear the menu status from the display */
		MenuEraseStatus(0);

		/* Display the About Box. */
		frmP = FrmInitForm(AboutForm);
		FrmDoDialog(frmP);
		FrmDeleteForm(frmP);

		handled = true;
		break;
	}

	case MainManageClassesButton:
	{
		FrmGotoForm(ClassesForm);
		handled = true;
		break;
	}

	case MainAddHomeworkButton:
	{
		FrmGotoForm(ManageHomeworkForm);
		handled = true;
		break;
	}

	case MainManageHomeworkButton:
	{
		FrmGotoForm(HomeworksForm);
		handled = true;
		break;
	}

	case MainMemoButton:
	{
		UInt16 		cardNo;
		LocalID 		dbID;
		DmSearchStateType searchInfo;
		UInt32		*gotoInfoP;

		// get the Memo application's card number an dbID
		DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, sysFileCMemo, true, &cardNo, &dbID);

		// Set the pointers following the documentation
		gotoInfoP = (UInt32*)MemPtrNew (sizeof(UInt32));
		ErrFatalDisplayIf ((!gotoInfoP), "Out of memory");
		MemPtrSetOwner(gotoInfoP, 0);

		// Switches the app
		SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdNormalLaunch, gotoInfoP);
		handled = true;
		break;
	}
	}

	return handled;
}

/*
 * FUNCTION: MainFormInit
 *
 * DESCRIPTION: This routine initializes the MainForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */
void MainFormInit(FormType *frmP)
{
	Char *CurrClass;

	CurrClass = (Char *)MemPtrNew(sizeof(Char[19]));
	if ((UInt32)CurrClass == 0)
		return;
	MemSet(CurrClass, sizeof(Char[19]), 0);

	ShowCurrentTime(frmP);
	ShowCurrentWeekday(frmP);
	SetCurrentClass(frmP, CurrClass);
	SetNextClass(frmP, CurrClass);
	SetDueHomework(frmP);
	MemPtrFree(CurrClass);
}


void SetDueHomework(FormType *frmP)
{
	UInt32 pstInt, tomorrowSec, nowSec, classDueSec;
	DateTimeType now, classDue;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	UInt16 i, dueCount, numRecs;
	dueCount = 0;

	nowSec = TimGetSeconds();
	TimSecondsToDateTime(nowSec, &now);
	now.hour = 23;
	now.minute = 59;
	nowSec = TimDateTimeToSeconds(&now);
	tomorrowSec = nowSec + 86400; // Seconds in one day
	
	
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->completedDate.year == 0)
		{
			classDue.year = rec->dueYear;
			classDue.month = rec->dueMonth;
			classDue.day = rec->dueDay;
			classDue.weekDay = DayOfWeek(rec->dueMonth, rec->dueDay, rec->dueYear);
			classDue.hour = 23;
			classDue.minute = 58;
			classDue.second = 1;

			classDueSec = TimDateTimeToSeconds(&classDue);

			if (classDueSec > nowSec)
			{
				if (classDueSec < tomorrowSec)
				{
					dueCount++;
				}
			}
		}
	}

	if (dueCount == 0) {
		SetNoDue(frmP);
	} else {
		SetDueCount(frmP, dueCount);
	}

}

void SetDueCount(FormType *frmP, UInt16 dueCount)
{
	Char *label;
	Char *dueCountStr;
	
	// Allocate memory in dynamic heap for the labels
	label = (Char *)MemPtrNew(22);
	ErrFatalDisplayIf ((!label), "Out of memory");
	MemSet(label, 22, 0);
	
	dueCountStr = (Char *)MemPtrNew(3);
	ErrFatalDisplayIf ((!dueCountStr), "Out of memory");
	MemSet(dueCountStr, 3, 0);
	
	// Manipulate the labels
	StrCat(label, "Found ");
	StrIToA(dueCountStr, dueCount);
	StrCat(label, dueCountStr);
	StrCat(label,  " due\rtomorrow");
	// Copy text from memory to the label structure
	// and redraw the label
	FrmCopyLabel(frmP, MainDueLabel, label);
	
	// As text was COPIED, we can free it in the heap
	MemPtrFree(label);
	MemPtrFree(dueCountStr);
	
	DrawDueHomeworkIcon(TaskAttemptFamily);
}

void SetNoDue(FormType *frmP)
{
	FrmCopyLabel(frmP, MainDueLabel, "Nothing due\rtomorrow");
	DrawDueHomeworkIcon(TaskCompleteFamily);
}

void DrawDueHomeworkIcon(DmResID bmpFamilyID)
{
	MemHandle resH;
	BitmapType *bitmap;
	
	resH = DmGetResource(bitmapRsc, bmpFamilyID); 
	bitmap = MemHandleLock (resH);
	WinPaintBitmap(bitmap, 79, 133);
	MemHandleUnlock(resH);
}

/**
 * @brief Set the Current Class label.
 *
 * @param frmP
 */
void SetCurrentClass(FormType *frmP, Char *className)
{
	DateTimeType now, start, finish;
	UInt32 pstInt, nowSec, startSec, finishSec;
	DmOpenRef gDB;
	UInt16 numRecs, i;
	ClassDB *rec;
	MemHandle recH;
	Boolean set = false;

	nowSec = TimGetSeconds();
	TimSecondsToDateTime(nowSec, &now);
	start = now;
	finish = now;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		
		if (rec->classOcurrence[now.weekDay].active)
		{
			start.hour = rec->classOcurrence[now.weekDay].sHour;
			start.minute = rec->classOcurrence[now.weekDay].sMinute;
			startSec = TimDateTimeToSeconds(&start);

			finish.hour = rec->classOcurrence[now.weekDay].fHour;
			finish.minute = rec->classOcurrence[now.weekDay].fMinute;
			finishSec = TimDateTimeToSeconds(&finish);

			if (nowSec >= startSec && nowSec <= finishSec)
			{
				FrmCopyLabel(frmP, MainCurrentClassLabel, rec->className);
				StrCopy(className, rec->className); // Set className variable to be used on the next method
				set = true;
			}
		}
		
		MemHandleUnlock(recH);
	}

	if (!set)
	{
		FrmCopyLabel(frmP, MainCurrentClassLabel, "None. Enjoy!");
	}
}

/**
 * @brief Set the Next Class label.
 *
 * @param frmP
 */
void SetNextClass(FormType *frmP, Char *currentClass)
{
	DateTimeType now, start, finish;
	UInt32 pstInt, nowSec, startSec, finishSec;
	DmOpenRef gDB;
	UInt16 numRecs, i, x;
	ClassDB *rec;
	MemHandle recH;
	Boolean set = false;

	nowSec = TimGetSeconds();
	TimSecondsToDateTime(nowSec, &now);
	start = now;
	finish = now;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		nowSec = TimGetSeconds();
		TimSecondsToDateTime(nowSec, &now);

		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);

		if (rec->classOcurrence[now.weekDay].active)
		{
			start.hour = rec->classOcurrence[now.weekDay].sHour;
			start.minute = rec->classOcurrence[now.weekDay].sMinute;
			startSec = TimDateTimeToSeconds(&start);

			finish.hour = rec->classOcurrence[now.weekDay].fHour;
			finish.minute = rec->classOcurrence[now.weekDay].fMinute;
			finishSec = TimDateTimeToSeconds(&finish);

			now.minute = 0;
			nowSec = TimDateTimeToSeconds(&now);

			for (x = now.hour; x <= 23; x++)
			{
				if (nowSec >= startSec && nowSec <= finishSec)
				{
					if (StrCompare(rec->className, currentClass) != 0)
					{
						FrmCopyLabel(frmP, MainNextClassLabel, rec->className);
						set = true;
						break;
					}
				}
				nowSec += 3600; // Advance 1 hour. May tweak this to 15 minutes.
				TimSecondsToDateTime(nowSec, &now);
			}
		}
		
		MemHandleUnlock(recH);

		if (set) {
			break;
		}
	}

	if (!set)
	{
		FrmCopyLabel(frmP, MainNextClassLabel, "None. Enjoy!");
	}
}

/*
 * FUNCTION: ShowCurrentTime
 *
 * DESCRIPTION: Update the label on the main form with the current time.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */

void ShowCurrentTime(FormType *frmP)
{
	DateTimeType now;
	char *timeStr;
	
	// Allocate memory in dynamic heap for the label
	timeStr = (Char *)MemPtrNew(timeStringLength);
	ErrFatalDisplayIf ((!timeStr), "Out of memory");
	MemSet(timeStr, timeStringLength, 0);

	TimSecondsToDateTime(TimGetSeconds(), &now);
	TimeToAscii(now.hour, now.minute, tfColon24h, timeStr);

	FrmCopyLabel(frmP, MainTimeLabel, timeStr);
	
	MemPtrFree(timeStr);
}

void ShowCurrentWeekday(FormType *frmP)
{
	DateTimeType now;
	Char *dowNameStr;
	
	// Allocate memory in dynamic heap for the label
	dowNameStr = (Char *)MemPtrNew(dowDateStringLength);
	ErrFatalDisplayIf ((!dowNameStr), "Out of memory");
	MemSet(dowNameStr, dowDateStringLength, 0);

	TimSecondsToDateTime(TimGetSeconds(), &now);
	DateTemplateToAscii("^1l", now.month, now.day, now.year, dowNameStr, dowDateStringLength);
	FrmCopyLabel(frmP, MainWeekdayLabel, dowNameStr);
	
	MemPtrFree(dowNameStr);
}
/*
 * FUNCTION: MainFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "MainForm" of this
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

Boolean MainFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;

	switch (eventP->eType)
	{
	case menuEvent:
		return MainFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
		MainFormInit(frmP);
		handled = true;
		break;

	case frmUpdateEvent:
		/*
		 * To do any custom drawing here, first call
		 * FrmDrawForm(), then do your drawing, and
		 * then set handled to true.
		 */
		break;

	case ctlSelectEvent:
	{
		return MainFormDoCommand(eventP->data.menu.itemID);
	}
	}

	return handled;
}