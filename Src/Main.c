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
	SetDueHomework();
	MemPtrFree(CurrClass);
}


void SetDueHomework()
{
	UInt32 pstInt;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	UInt16 i, dueCount, numRecs;
	dueCount = 0;

	// The number of choices is equal to the number os classes
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
			// TODO: Save the index on a global to fetch later
			// in order to avoid reescaning the whole DB on
			// every freaking line draw

		}
	}

}

void SetDueCount(UInt16 dueCount)
{
	DrawDueHomeworkIcon(TaskAttemptFamily);
}

void SetNoDue()
{
	DrawDueHomeworkIcon(TaskCompleteFamily);
}

void DrawDueHomeworkIcon(DmResID bmpFamilyID)
{
	MemHandle resH;
	BitmapType *bitmap;
	
	resH = DmGetResource(bitmapRsc, bmpFamilyID); 
	bitmap = MemHandleLock (resH); 
	WinPaintBitmap(bitmap, 79, 133); 
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
		MemHandleUnlock(recH);

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
		MemHandleUnlock(recH);

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
	char timeStr[timeStringLength];

	TimSecondsToDateTime(TimGetSeconds(), &now);
	TimeToAscii(now.hour, now.minute, tfColon24h, timeStr);

	FrmCopyLabel(frmP, MainTimeLabel, timeStr);
}

void ShowCurrentWeekday(FormType *frmP)
{
	DateTimeType now;
	Char dowNameStr[dowDateStringLength];

	TimSecondsToDateTime(TimGetSeconds(), &now);
	DateTemplateToAscii("^1l", now.month, now.day, now.year, dowNameStr, sizeof(dowNameStr));
	FrmCopyLabel(frmP, MainWeekdayLabel, dowNameStr);
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
		MainFormInit(frmP);
		FrmDrawForm(frmP);
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