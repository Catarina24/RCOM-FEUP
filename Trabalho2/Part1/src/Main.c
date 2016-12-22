#include <stdio.h>
#include <string.h>
#include "URL.h"
#include "FTP.h"

void printUsage(char* argv0) {
	printf("\nUsage with user and password: %s ftp://[<user>:<password>@]<host>/<url-path>\n",
			argv0);
	printf("Usage Anonymously: %s ftp://<host>/<url-path>\n\n", argv0);
}

int main(int argc, char** argv) {

		if (argc != 2) {
		printf("Invalid Number of Arguments.\n");
		printUsage(argv[0]);
		return 1;
	}

	/*URL*/

	url url;

	initURL(&url, argv[1]);

	if (parseURL(&url) == -1){
		printf("parseURL failed!\n");
		return -1;
	}
		

	if (getIpByHost(&url)) {
		printf("Did not find IP to hostname %s.\n", url.host);
		return -1;
	}

	printf("\nThe IP received to %s was %s\n", url.host, url.ip);

	/*FTP*/

	ftp ftp;
	
	ftpConnect(&ftp, url.ip, url.port);

	// Verifying username
	const char* user = strlen(url.user) ? url.user : "anonymous";

	// Verifying password
	char* password;
	if(strcmp(user,"anonymous")==0){
		printf("Anonymous Mode.\n");
		password = NULL;
	}
	else if (strlen(url.password)) {
		password = url.password;
	} else {
		char buf[100];
		printf("Please insert your college email as password: ");
		while (strlen(fgets(buf, 100, stdin)) < 14)
			printf("\nIncorrect input, please try again: ");
		password = (char*) malloc(strlen(buf));
		strncat(password, buf, strlen(buf) - 1);
	}

	// Sending credentials to server
	if (ftpLogin(&ftp, user, password)) {
		printf("Did Not Login User: %s\n", user);
		return -1;
	}

	// Changing directory
	if (ftpChangeDir(&ftp, url.path)) {
		printf("Did Not Change Directory of %s\n",
				url.filename);
		return -1;
	}

	// Entry in passive mode
	if (ftpPassive(&ftp)) {
		printf("Passive Mode Not Active!\n");
		return -1;
	}

	// Begins transmission of a file from the remote host
	ftpRetrieve(&ftp, url.filename);

	// Starting file transfer
	ftpDownload(&ftp, url.filename);

	// Disconnecting from server
	ftpDisconnect(&ftp);

	return 0;
}
