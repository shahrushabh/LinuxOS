/* fat12ls.c 
* 
*  Displays the files in the root sector of an MSDOS floppy disk
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define SIZE 32      /* size of the read buffer */
#define ROOTSIZE 256 /* max size of the root directory */
//define PRINT_HEX   // un-comment this to print the values in hex for debugging

struct BootSector
{
    unsigned char  sName[9];            // The name of the volume
    unsigned short iBytesSector;        // Bytes per Sector
    
    unsigned char  iSectorsCluster;     // Sectors per Cluster
    unsigned short iReservedSectors;    // Reserved sectors
    unsigned char  iNumberFATs;         // Number of FATs
    
    unsigned short iRootEntries;        // Number of Root Directory entries
    unsigned short iLogicalSectors;     // Number of logical sectors
    unsigned char  xMediumDescriptor;   // Medium descriptor
    
    unsigned short iSectorsFAT;         // Sectors per FAT
    unsigned short iSectorsTrack;       // Sectors per Track
    unsigned short iHeads;              // Number of heads
    
    unsigned short iHiddenSectors;      // Number of hidden sectors
};

void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[]);
//  Pre: Calculated directory offset and number of directory entries
// Post: Prints the filename, time, date, attributes and size of each entry

unsigned short endianSwap(unsigned char one, unsigned char two);
//  Pre: Two initialized characters
// Post: The characters are swapped (two, one) and returned as a short

void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[]);
//  Pre: An initialized BootSector struct and a pointer to an array
//       of characters read from a BootSector
// Post: The BootSector struct is filled out from the buffer data

char * toDOSName(char string[], unsigned char buffer[], int offset);
//  Pre: String is initialized with at least 12 characters, buffer contains
//       the directory array, offset points to the location of the filename
// Post: fills and returns a string containing the filename in 8.3 format

char * parseAttributes(char string[], unsigned char key, unsigned char buffer[]);
//  Pre: String is initialized with at least five characters, key contains
//       the byte containing the attribue from the directory buffer
// Post: fills the string with the attributes

char * parseTime(char string[], unsigned short usTime, unsigned char buffer[]);
//  Pre: string is initialzied for at least 9 characters, usTime contains
//       the 16 bits used to store time
// Post: string contains the formatted time

char * parseDate(char string[], unsigned short usDate, unsigned char buffer[]);
//  Pre: string is initialized for at least 13 characters, usDate contains
//       the 16 bits used to store the date
// Post: string contains the formatted date

int roundup512(int number);
// Pre: initialized integer
// Post: number rounded up to next increment of 512

char * size(char string[], unsigned char buffer[], int offset);
//

// reads the boot sector and root directory
int main(int argc, char * argv[])
{
    int pBootSector = 0;
    unsigned char buffer[SIZE];
    unsigned char rootBuffer[ROOTSIZE * 32];
    struct BootSector sector;
    int iRDOffset = 0;
    
    // Check for argument
    if (argc < 2) {
    	printf("Specify boot sector\n");
    	exit(1);
    }
    
    // Open the file and read the boot sector
    pBootSector = open(argv[1], O_RDONLY);
    read(pBootSector, buffer, SIZE);
    
    // Decode the boot Sector
    decodeBootSector(&sector, buffer);
    
    // Calculate the location of the root directory
    iRDOffset = (1 + (sector.iSectorsFAT * sector.iNumberFATs) )
                 * sector.iBytesSector;
    
    // Read the root directory into buffer
    lseek(pBootSector, iRDOffset, SEEK_SET);
    read(pBootSector, rootBuffer, ROOTSIZE);
    close(pBootSector);
    
    // Parse the root directory
    parseDirectory(iRDOffset, sector.iRootEntries, rootBuffer);
    
} // end main


// Converts two characters to an unsigned short with two, one
unsigned short endianSwap(unsigned char one, unsigned char two)
{
    short temp = two << 8;
    temp = temp | one;
    return temp;
//    This is stub code.
//    return 0x0000;
}


// Fills out the BootSector Struct from the buffer
void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[])
{
    int i = 3;  // Skip the first 3 bytes
    char one, two;

    // Pull the name and put it in the struct (remember to null-terminate)
    strncpy(pBootS->sName, &(buffer[i]), 8);
    pBootS->sName[9] = '\0';

    // Read bytes/sector and convert to big endian
    i = 0x0B;
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iBytesSector = endianSwap(one, two);

    // Read sectors/cluster, Reserved sectors and Number of Fats
    one = buffer[i];
    i += 1;
    pBootS->iSectorsCluster = endianSwap(one, 0);

    // Read root entries, logicical sectors and medium descriptor
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iReservedSectors = endianSwap(one, two);

    // Read and covert sectors/fat, sectors/track, and number of heads
    one = buffer[i];
    i += 1;
    pBootS->iNumberFATs = endianSwap(one, 0);
    
    // Root Directory entries.
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iRootEntries = endianSwap(one, two);

    // Logical sectors.
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iLogicalSectors = endianSwap(one, two);
    
    // Medium Descriptor.
    one = buffer[i];
    i += 1;
    pBootS->xMediumDescriptor = endianSwap(one, 0);
    
    // Sectors per FAT.
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iSectorsFAT = endianSwap(one, two);
    
    // Sectors per Track.
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iSectorsTrack = endianSwap(one, two);
    
    // Number of Heads.
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iHeads = endianSwap(one, two);
    
    // Read hidden sectors
    one = buffer[i];
    i += 1;
    two = buffer[i];
    i += 1;
    pBootS->iHiddenSectors = endianSwap(one, two);
    
    return;
}


// iterates through the directory to display filename, time, date,
// attributes and size of each directory entry to the console
void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[])
{
    int i = 0;
    char string[13];
    
    // Display table header with labels
    printf("Filename\tAttrib\tTime\t\tDate\t\tSize\n");
    
    // loop through directory entries to print information for each
    for(i = 0; i < (iEntries); i = i + 32)   {
    	if ((buffer[i] != 0x00) && (buffer[i] != 0xe5)) {
    		// Display filename
    		printf("%s\t", toDOSName(string, buffer, i));
    		// Display Attributes
    		printf("%s\t", parseAttributes(string, (i+0x0b), buffer));
    		// Display Time
    		printf("%s\t", parseTime(string, (i+0x16), buffer));
    		// Display Date
    		printf("%s\t", parseDate(string, (i+0x18), buffer));
    		// Display Size
    		printf("%s\n", size(string, buffer, (i+0x1c)));
    	}
    }
    
    // Display key
    printf("(R)ead Only (H)idden (S)ystem (A)rchive\n");
} // end parseDirectory()


// Parses the attributes bits of a file
char * parseAttributes(char string[], unsigned char key, unsigned char buffer[])
{
    // This is stub code!
    // Check for the set bit for position 0, 1, 2, and 5.
    int set_index = 0;
    if(buffer[key] & 0x01) {
    	string[set_index] = 'R';
    	set_index += 1;
    }
    if(buffer[key] & 0x02) {
    	string[set_index] = 'H';
    	set_index += 1;
    }
    if(buffer[key] & 0x04) {
    	string[set_index] = 'S';
    	set_index += 1;
    }
    if(buffer[key] & 0x20) {
    	string[set_index] = 'A';
    	set_index += 1;
    }
    string[set_index] = '\0';
    return string;
} // end parseAttributes()


// Decodes the bits assigned to the time of each file
char * parseTime(char string[], unsigned short usTime, unsigned char buffer[])
{
    unsigned char hour = 0x00, min = 0x00, sec = 0x00;
    
    sec = 2 * (buffer[usTime] & 0x1f);
    hour = (buffer[usTime+1] >> 3);
    min = (buffer[usTime+1] & 0x07) << 3;
    min = min | ((buffer[usTime] & 0xd0) >> 5);
    
    sprintf(string, "%02i:%02i:%02i", hour, min, sec);
    
    return string;
    
    
} // end parseTime()


// Decodes the bits assigned to the date of each file
char * parseDate(char string[], unsigned short usDate, unsigned char buffer[])
{
    unsigned char month = 0x00, day = 0x00;
    unsigned short year = 0x0000;
    
    day = (buffer[usDate] & 0x1f);

    // decimal 1980 = 0x7bc.
    year = 0x7bc + ((buffer[usDate+1] & 0xfd) >> 1);
    month = (buffer[usDate+1] & 0x01) << 3;
    month = month | ((buffer[usDate] & 0xe0) >> 5);
    sprintf(string, "%d/%d/%d", year, month, day);
    
    return string;
    
} // end parseDate()


// Formats a filename string as DOS (adds the dot to 8-dot-3)
char * toDOSName(char string[], unsigned char buffer[], int offset)
{
    int i=0;
    int j=0;
    for(i=0; i<8; i++) {
        if(buffer[i+offset] && (buffer[i+offset] != ' ')) {
            string[i] = buffer[i+offset];
        } else {
            break;
        }
    }
    string[i] = '.';
    j = i+1;
    for(i=8; i<11; i++) {
        string[j] = buffer[i+offset];
        j++;
    }
    string[j] = '\0';
    sprintf(string, "%s", string);
    return string;
} // end toDosNameRead-Only Bit

char * size(char string[], unsigned char buffer[], int offset) {
    unsigned int size;
    unsigned short first = endianSwap(buffer[offset+2],buffer[offset+3]);
    unsigned short second = endianSwap(buffer[offset],buffer[offset+1]);
    size = first << 16;
    size = size | second;
    sprintf(string, "%d Bytes.", size);
    return string;
}