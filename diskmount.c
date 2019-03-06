//This program mounts and unmount a specific block device using udisksctl. Runs as the same permissions as the user who runs it, that is, no sudo-ing involved.

#include <stdio.h>
#include <string.h> //string manipulation
#include <stdlib.h> //includes prototypes for exit()
#include <unistd.h> //exit functionality

//Prototypes
char getchoice(); //gets choice from stdin and returns to user
void getdevice(char string[10]); //Gets specific block device from stdin.
void call_lsblk(); // calls lsblk to list block devices
void runMounter(int choice, char device[10]); //method to run the mounter
void getByMountname(char *argv[], int i, char *device);
void removeNewlines(char text[]);
void showHelp();

int main(int argc, char *argv[]) //argc is 1 when no arguments given (argv[0] is ./filename)
{
	int ready1=0;
	char choice = '\0'; //initialise choice char with null terminator
	char device[15];
	memset(device,0,sizeof(device)); //nullifies string (zeroes it out)

	int i;
	
	for (i=1;i<argc;i++)
	{
		if (strcmp("-h", argv[i]) == 0)
			showHelp();

		if (strcmp("-f", argv[i]) == 0)
		{
			getByMountname(argv,i+1, device);
			ready1 = 1;
		}
		if (strcmp("-c", argv[i]) == 0)
		{
			choice = argv[i+1][0]; //grabs first char of the next argument
		}
	}
	//at this point, device has been initialised...maybe
	if (choice == '\0')
		choice = getchoice(); //initialises choice if its not given to user
	if (!(choice == 'm' || choice == 'u'))
	{
		printf("Invalid choice. Exiting.");
		exit(1);
	}
	while (device[0] == '\0')
	{
		ready1 = 0; //acting as boolean
		getdevice(device);
		if (device[0] == '\0')
			printf("\n\n\tInvalid device. Try again.\n\n");
		else
			ready1 = 1;
	}
	if (ready1)
	{	
		if (choice == 'm')
			runMounter(1, device);
		else if (choice == 'u')
			runMounter(0, device);
		else
		{
			printf("invalid choice, exiting");
			exit(1);
		}
	}

}

void getByMountname(char *argv[], int i, char *device) //i has been incremented when passing here - so ith index contains the argument of the dev mounted name
{
	FILE *deviceRetrieve;
	char devName[20];
	char devArg[20];
	char cmd[100];
	char tempDevLine[100];
	memset(cmd,0,sizeof(cmd)); //nullifies string (zeroes it out)
	memset(tempDevLine,0,sizeof(tempDevLine)); //nullifies string (zeroes it out)
	memset(devName,0,sizeof(devName)); //nullifies string (zeroes it out)
	memset(devArg,0,sizeof(devArg)); //nullifies string (zeroes it out)
	strcpy(devArg, argv[i]);
	sprintf(tempDevLine, "lsblk | grep \"%s\"",devArg);
	sprintf(cmd, "lsblk | grep \"%s\" | cut -d \" \" -f 1 | sed 's/^..//'", devArg);
	deviceRetrieve = popen(cmd,"r");
	if(deviceRetrieve==NULL)
	{
		printf("Error creating file pointer deviceRetrieve. Exiting!");
		exit(1);
	}

	while ( fgets(devName, sizeof(devName), deviceRetrieve) != NULL)
	{
		//nothing happens here - this is just a buffer for fgets
	}

	pclose(deviceRetrieve);
	removeNewlines(devName);
	system(tempDevLine);
	printf("Is %s your device? (y/n)", devName);
	char choice = getchar();
	if (choice == 'y')
	{
		strcpy(device,devName);
		return;
	}
	else if (choice == 'n')
	{
		printf("exiting.");
		exit(0);
		
	}
	else
	{
		printf("invalid choice, exiting");
		exit(0);
	}

}

char getchoice()
{
    printf("No choice argument specified. Enter (m) to mount or (u) to unmount:\t");
    return getchar(); //runs getchar to get a character and returns straight into its called function
}

void getdevice(char string[10]) //gets block device from stdin, after calling lsblk to list it to user
{
    call_lsblk(); //calls lsblk, nothing more
    printf("\nBlock device? /dev/XXXX:\t"); //prompts for block device without /dev/ to make it easy
    scanf("%s", string);
    printf("\n"); //skips line
    return;
}

void runMounter(int choice, char device[10])
{
    char cmd[40];
    if (device[0] == '\0') //if device string is empty, then hasnt been pre-provided in an arg.
    {
        getdevice(device); //passes string raw - its passed as a pointer anyway.
    }
    char mountchoice[10] = "unmount"; //by default unmounts the block device - when if statement runs, it will overwrite the mountchoice string
    if (choice)
    {
        memset(mountchoice,0,sizeof(mountchoice)); //erases mountchoice string
        sprintf(mountchoice, "mount"); //replaces all of mountchoice with mount
    }

    sprintf(cmd, "udisksctl %s -b /dev/%s", mountchoice, device); //puts the mountchoice and device name into the udisksctl command. device string goes into pre-prepared /dev/ placeholder.
    exit(system(cmd)); //calls the command and exits with the same error code.

}

void call_lsblk() //simply calls lsblk
{
    char cmd[20] = "lsblk";
    system(cmd); //since its system, it calls it using the same permissions as the user calling it.
    return;
}

void showHelp()
{
	printf("\tWelcome to diskmount.\n\n\tRun diskmount with the -c switch, followed by either 'm' or 'u' for mount and unmount, respectively.\n\tUse the -f switch to search for a string in the lsblk buffer - this uses grep and cut by design.\n\tUse -h to display this help message.\n\tThank you.\n\n");
	exit(0); //no error.
}

void removeNewlines(char text[])
{
	int i;
	for (i=0;i<strlen(text);i++)
	{
		if (text[i] == '\n')
			text[i] = '\0'; //removes newlines and replaces with null terminator
	}
}
