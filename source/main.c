// This game was commented and made for those who don't understand C or 3DS Homebrew
#include <string.h>
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

u64 cookies = 0;
int screen = 0;
int cursor = 0;

// Keep track of values

u64 buildingData[10] = {0};
bool autosave = true;
u16 saverate = 60;

//Framerate Check
int frames = 0; 
int longTimer = 0;
int autosavtimer = 0;

//Others
u64 cps = 0;

int main()
{
	buildingData[0] = 0; // Number of Clickers
	buildingData[1] = 0; // Number of Grandmas
	buildingData[2] = 0; // Number of Bakeries
	buildingData[3] = 10; // Clicker Cost
	buildingData[4] = 100; // Grandma Cost
	buildingData[5] = 1000; // Bakery Cost
	buildingData[6] = 1; //Cookie Multiplier
	buildingData[7] = 10000; //Cookie Multiplayer Cost
	buildingData[8] = 100000; //Clicker Upgrade Cost
	buildingData[9] = 1; //Clicker Upgrade Total
	
	
	gfxInitDefault();
	//Select Screens
	PrintConsole topScreen, bottomScreen;
	
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	//Choose which screen to print to (top)
	consoleSelect(&topScreen);
	
	/* Create "/3ds/data/CookieCollector/" directory.
	This is to prevent the game from crashing if it attempts to create "user.txt" while the directory doesn't exist. */
	
	mkdir("/3ds/", 0700);
	mkdir("/3ds/data/", 0700);
	mkdir("/3ds/data/CookieCollector", 0700);
	
	
	//Open file user.txt as "fp". If it exists, read it. If it doesn't, create a new file.
	FILE* fp = fopen("/3ds/data/cookiecollector/user.txt", "r+");
	
	if(!fp) { 
		fp = fopen("/3ds/data/cookiecollector/user.txt", "w+");
	}
	
	//Write data from file to variables.
    fread(&cookies, sizeof(u64), 1, fp);
	fread(buildingData, sizeof(u64), 10, fp);
	fread(&autosave, sizeof(bool), 1, fp);
	fread(&saverate, sizeof(u16), 1, fp);
	rewind(fp);
	
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		//Leave App
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; //Break in order to return to hbmenu
		
		
		if (screen == 0) { //Select Which Screen to Use

			consoleSelect(&topScreen);

			// Title Screen
			printf("\x1b[1;14HCookie Collector 3DS\e[K\n");
			printf("\x1b[5;15HYou have: %lld cookies \e[K\n", cookies);
			printf("\x1b[6;0H__________________________________________________");
			printf("\x1b[8;2HIt costs %llu cookies to build a clicker\e[K\n", buildingData[3]);
			printf("\x1b[9;2HIt costs %llu cookies to hire a grandma\e[K\n", buildingData[4]);
			printf("\x1b[10;2HIt costs %llu cookies to build a bakery\e[K\n", buildingData[5]);
			
		
	
			//Add Cookies	
			if (kDown & KEY_X){
				cookies = cookies + buildingData[6];
			}
						
			if (kDown & KEY_DOWN) {
				cursor++;
			} else if (kDown & KEY_UP) {
				cursor--;
			}
			
			if (cursor < 1) {
				cursor = 1;
			} else if (cursor > 2) {
				cursor = 2;
			}
			
			if (cursor == 1) {
				printf("\x1b[12;2H> Shop\e[K\n");
				printf("\x1b[13;2H  Options\e[K\n");
			} else if (cursor == 2) {
				printf("\x1b[12;2H  Shop\e[K\n");
				printf("\x1b[13;2H> Options\e[K\n");
			}
			
			if (kDown & KEY_A & (cursor == 1)) {
				screen = 1;
				cursor = 0;
				consoleClear();
			} else if (kDown & KEY_A & (cursor == 2)) {
				screen = 2;
				cursor = 0;
				consoleClear();
			}
		
		} else if (screen == 1) { //Select Shop
			
			consoleSelect(&topScreen);
			
			printf("\x1b[1;23HShop\e[K\n");
			printf("\x1b[5;15HYou have: %lld cookies \e[K\n", cookies);
			printf("\x1b[6;0H__________________________________________________\e[K\n");
			printf("\x1b[8;0HPress B to go back\e[K\n");
			
			if (kDown & KEY_DOWN) {
				cursor++;
			} else if (kDown & KEY_UP) {
				cursor--;
			}
			
			if ((cursor > 5) & (buildingData[0] >= 25)) {
				cursor = 5;
			} else if ((cursor > 4) & (buildingData[0] < 25)) {
				cursor = 4;
			} else if (cursor < 1) {
				cursor = 1;
			}
			
			// Simplified this using for loops and separating the options from the cursor
			printf("\x1b[10;3H Clicker:         %llu\e[K\n", buildingData[3]);
			printf("\x1b[11;3H Grandma:         %llu\e[K\n", buildingData[4]);
			printf("\x1b[12;3H Bakery:          %llu\e[K\n", buildingData[5]);
			printf("\x1b[14;3H Cookie Upgrade:  %llu\e[K\n", buildingData[7]);
			if (buildingData[0] >= 25) {
			printf("\x1b[15;3H Upgrade Clicker: %llu\e[K\n", buildingData[8]);
			}
			
			// Draw cursor
			if (cursor <= 3){
				printf("\x1b[%d;2H>", (9 + cursor));
				
			} else {
				printf("\x1b[%d;2H>", (10 + cursor));
			}
			
			// Refresh the top options (first three)
			for (int i = 1; i <= 3; i++) {
				if (i != cursor)
					printf("\x1b[%d;2H ", (9 + i));
			}
			
			// Refresh the bottom options (last two)
			for (int i = 4; i <= 5; i++) {
				if (i != cursor)
					printf("\x1b[%d;2H ", (10 + i));
			}
				
			
			if (kDown & KEY_B) {
				screen = 0;
				cursor = 0;
				consoleClear();
			}

			//Buy clicker	
			if (kDown & KEY_A & (cursor == 1)){
			
				if(cookies >= buildingData[3]){
					cookies-=buildingData[3];
					buildingData[3] = buildingData[3] * 1.2;
					buildingData[0]++;
				}
			}
		
			//Buy grandma
			if(kDown * KEY_A & (cursor == 2)){
				if(cookies >= buildingData[4]){
					cookies-=buildingData[4];
					buildingData[4] = buildingData[4] * 1.2;
					buildingData[1]++;
				}
			}
			
			//Buy bakery
			if(kDown & KEY_A & (cursor == 3)){
				if(cookies >= buildingData[5]){
					cookies-=buildingData[5];
					buildingData[5] = buildingData[5] * 1.2;
					buildingData[2]++;
				} 
			}
			
			if(kDown & KEY_A & (cursor == 4)) {
				if(cookies >= buildingData[7]) {
					cookies-=buildingData[7];
					buildingData[7] = buildingData[7] * 1.2;
					buildingData[6]++;
				}
			
			}
			
			if(kDown & KEY_A & (cursor == 5)) {
				if(cookies >= buildingData[8]) {
					cookies-= buildingData[8];
					buildingData[8] = buildingData[8] * 1.2;
					buildingData[9]++;
				}
			}
			//----------------------------------------------------
			//---------------------End Of-------------------------
			//---------------------Screen-------------------------
			//----------------------------------------------------
			
		} else if (screen == 2) {
			
			consoleSelect(&topScreen);
			
			printf("\x1b[1;23HConfig\e[K\n");
			printf("\x1b[5;15HYou have: %lld cookies \e[K\n", cookies);
			printf("\x1b[6;0H__________________________________________________\e[K\n");

			if (screen == 2){
				printf("\x1b[8;2HPress B to go back\e[K\n");
			}
			
			if (kDown & KEY_DOWN) {
				cursor++;
			} else if (kDown & KEY_UP) {
				cursor--;
			}

			if (cursor > 4) {
				cursor = 4;
			} else if (cursor < 1) {
				cursor = 1;
			}
			
			if (autosave)
				printf("\x1b[12;3H Autosave: Yes\e[K\n");
			else
				printf("\x1b[12;3H Autosave: No \e[K\n");
			printf("\x1b[13;3H Autosave rate (per sec.): %hu  \e[K\n", saverate);
			printf("\x1b[14;3H Save\e[K\n");
			printf("\x1b[15;3H Reset\e[K\n");
			
			printf("\x1b[%d;2H>", 11 + cursor);
			for (int i = 1; i <= 4; i++){
				if (i != cursor)
					printf("\x1b[%d;2H ", 11 + i);
			}
			
			if (kDown & KEY_B) {
				screen = 0;
				cursor = 0;
				consoleClear();
			}
			
			if ((cursor == 1) & (kDown & KEY_LEFT || kDown & KEY_RIGHT)) {
				autosave = !autosave;
			} else if (cursor == 2) {
				if (kDown & KEY_LEFT)
					saverate--;
				else if (kDown & KEY_RIGHT)
					saverate++;
				
				if (saverate > 120)
					saverate = 120;
				else if (saverate < 20)
					saverate = 20;
				
			} else if ((cursor == 3) & kDown & KEY_A) {
				fwrite(&cookies, sizeof(u64), 1, fp);
				fwrite(buildingData, sizeof(u64), 10, fp);
				fwrite(&autosave, sizeof(bool), 1, fp);
				fwrite(&saverate, sizeof(u16), 1, fp);
				fflush(fp);
				rewind(fp);
			} else if ((cursor == 4) & kDown & KEY_A) {
				cookies = 0;
				buildingData[0] = 0;
				buildingData[1] = 0;
				buildingData[2] = 0;
				buildingData[3] = 10;
				buildingData[4] = 100;
				buildingData[5] = 1000;
				buildingData[6] = 1;
				buildingData[7] = 10000;
				buildingData[8] = 100000;
				buildingData[9] = 1;
				autosave = true;
				saverate = 60;
				fwrite(&cookies, sizeof(u64), 1, fp);
				fwrite(buildingData, sizeof(u64), 10, fp);
				fwrite(&autosave, sizeof(bool), 1, fp);
				fwrite(&saverate, sizeof(u16), 1, fp);
				fflush(fp);
				rewind(fp);
			}
			
			
		}
		
			consoleSelect(&bottomScreen);
			
			cps = (buildingData[0] * buildingData[9]) + (buildingData[1] * 5) + (buildingData[2] * 15);
		
			//%llu is a replacement for a long long unsigned integer (or a 64-bit unsigned integer)
			printf("\x1b[1;1HPress X to gain %llu cookies!\e[K\n", buildingData[6]);
			printf("\x1b[2;1HCookies per second: %llu\e[K\n", cps);
			printf("\x1b[4;1HYou have %llu clickers so far!\e[K\n", buildingData[0]);
			printf("\x1b[5;1HYou have %llu grandmas hired so far!\e[K\n", buildingData[1]);
			printf("\x1b[6;1HYou have %llu bakeries built so far!\e[K\n", buildingData[2]);
			printf("\x1b[9;1HAuthor: Kaisogen");
			printf("\x1b[10;1HHelp: TheMachinumps");
			printf("\x1b[11;1HPress /\\ or \\/ to select");
			
			//Once it auto saves, it will display a little message saying so
			if (autosavtimer > 0)
				printf("\x1b[13;1HAutosaved!");
			else
				printf("\x1b[13;1H          ");
		
		
		// Flush and swap frame-buffers
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		
		frames++;
		longTimer++;
		if (autosavtimer > 0)
			autosavtimer--;
		
		if (frames % 60 == 0) {
			cookies = cookies + (buildingData[0] * buildingData[9]);
			cookies = cookies + buildingData[1] * 5;
			cookies = cookies + buildingData[2] * 15;
		}
		
		//Every user defined amount of seconds (n seconds * 60 frames (refresh rate)) it autosaves the game.
		if ((longTimer % (saverate * 60) == 0) & autosave) {
			fwrite(&cookies, sizeof(u64), 1, fp);
			fwrite(buildingData, sizeof(u64), 10, fp);
			fwrite(&autosave, sizeof(bool), 1, fp);
			fwrite(&saverate, sizeof(u16), 1, fp);
			fflush(fp);
			rewind(fp);
			autosavtimer = 300; // 5 seconds
		}
		
	}
	gfxExit();
	return 0; //Program exited properly
}
