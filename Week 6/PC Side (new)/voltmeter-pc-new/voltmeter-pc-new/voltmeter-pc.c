#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

/* ===== SERIAL FUNCTIONS ===== */
HANDLE initCom(int comPortNumber) {
	DCB dcb;
	HANDLE hCom;
	BOOL fSuccess;
	char comPortName[20];
	sprintf(comPortName, "\\\\.\\COM%d", comPortNumber);
	hCom = CreateFileA(comPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		MessageBoxA(0, "Fout: De seriële poort kan niet geopend worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
		exit(-1);
	}
	fSuccess = GetCommState(hCom, &dcb);
	if (!fSuccess) {
		MessageBoxA(0, "Fout: De instellingen van de seriële poort kunnen niet gelezen worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
		exit(-1);
	}
	dcb.fParity = TRUE;
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 7;
	dcb.Parity = EVENPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess) {
		MessageBoxA(0, "Fout: De instellingen van de seriële poort kunnen niet aangepast worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
		exit(-1);
	}
	return hCom;
}

char readCom(HANDLE hCom) {
	char inBuffer[2] = " ";
	unsigned long nBytesRead;
	while (ReadFile(hCom, &inBuffer, 1, &nBytesRead, NULL) && nBytesRead == 0);
	if (nBytesRead != 1) {
		MessageBoxA(0, "Fout: De seriële poort kan niet uitgelezen worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
		exit(-1);
	}
	return inBuffer[0];
}

void writeCom(HANDLE hCom, char c) {
	char outBuffer[2] = " ";
	unsigned long nBytesWritten;
	outBuffer[0] = c;
	WriteFile(hCom, &outBuffer, 1, &nBytesWritten, NULL);
	if (nBytesWritten != 1) {
		MessageBoxA(0, "Fout: De seriële poort kan niet beschreven worden!", "Fout", MB_OK | MB_ICONEXCLAMATION);
		exit(-1);
	}
}

void closeCom(HANDLE hCom) {
	CloseHandle(hCom);
}



int main(void) {
	HANDLE hCom;
	char in;
	hCom = initCom(1);
	time_t current_time;
	time_t last_start;
	int read_AVR_serial = 0;
	char volts[10];
	FILE* AVR_data;
	int i;

	time(&current_time);
	last_start = current_time;
	AVR_data = fopen("AVR_data.txt", "w");

	while (1) {
		time(&current_time);
		if (current_time == -1) {
			printf("Current time is not available. Program will be terminated. Press enter to continue.\n");
			break;
		}

		if ((current_time - last_start) >= 5) {
			writeCom(hCom, 'S');
			printf("'S' was sent to AVR.\n");
			last_start = current_time;
			read_AVR_serial = 1;
		}

		if (read_AVR_serial == 1) {
			i = 0;
			do {
				in = readCom(hCom);
				if (in != 'Q') {
					volts[i] = in;
				}
				else {
					volts[i] = 0;
				}
				i++;
			} while (in != 'Q');
			printf("%s%s\n\n", ctime(&current_time), volts);
			
			if (AVR_data == NULL) {
				printf("File AVR_data.txt kan niet aangemaakt worden.\n");
			}
			else {
				fprintf(AVR_data, "%s%s\n", ctime(&current_time), volts);
				
			}

			read_AVR_serial = 0;
		}
	}
	fclose(AVR_data);
	getchar();

	return 0;
}
