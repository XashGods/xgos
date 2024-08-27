#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t bool;
#define true 1
#define false 0
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// TODO: add  __attribute__((packed)) to the struct
#pragma pack(push, 1)
typedef struct
{
	uint8_t BootJumpInstruction[3];
	uint8_t OemIdentifier[8];
	uint16_t BytesPerSector;
	uint8_t SectorsPerCluster;
	uint16_t ReservedSectors;
	uint8_t FatCount;
	uint16_t DirEntryCount;
	uint16_t TotalSectors;
	uint8_t MediaDescriptorType;
	uint16_t SectorsPerFat;
	uint16_t SectorsPerTrack;
	uint16_t Heads;
	uint32_t HiddenSectors;
	uint32_t LargeSectorCount;

	uint8_t DriveNumber;
	uint8_t _Reserved;
	uint8_t Signature;
	uint32_t VolumeId;
	uint8_t VolumeLabel[11];
	uint8_t SystemId[8];
} BootSector;
#pragma pack(pop)

// TODO: add  __attribute__((packed)) to the struct
#pragma pack(push, 1)
typedef struct
{
	uint8_t Name[11];
	uint8_t Attributes;
	uint8_t _Reserved;
	uint8_t CreationTimeTenths;
	uint16_t CreatedTime;
	uint16_t CreatedDate;
	uint16_t AccessedDate;
	uint16_t FirstClusterHigh;
	uint16_t ModifiedTime;
	uint16_t ModifiedDate;
	uint16_t FirstClusterLow;
	uint32_t Size;
} DirectoryEntry;
#pragma pack(pop)

BootSector g_BootSector;
uint8_t g_FAT = NULL;
DirectoryEntry *g_RootDirectory = NULL;
uint32_t g_RootDirectoryEnd;

bool read_boot_sector(FILE *disk, BootSector *boot_sector)
{
	return fread(&g_BootSector, sizeof(BootSector), 1, disk) > 0;
}

bool read_sectors(FILE *disk, uint32_t lba, uint32_t count, void *bufferOutput)
{
	bool ok = true;
	ok = ok && (fseek(disk, lba * g_BootSector.BytesPerSector, SEEK_SET) == 0);
	ok = ok && (fread(bufferOutput, g_BootSector.BytesPerSector, count, disk) == count);
	return ok;
}

bool read_fat(FILE *disk, uint32_t lba)
{
	g_FAT = (uint8_t *)malloc(g_BootSector.SectorsPerFat * g_BootSector.BytesPerSector);
	if (!g_FAT)
	{
		fprintf(stderr, "Failed to allocate memory for FAT\n");
		return false;
	}

	return read_sectors(disk, g_BootSector.ReservedSectors, g_BootSector.SectorsPerFat, g_FAT);
}

bool read_root_directory(FILE *disk)
{
	uint32_t lba = g_BootSector.ReservedSectors + (g_BootSector.FatCount * g_BootSector.SectorsPerFat);
	uint32_t size = g_BootSector.DirEntryCount * sizeof(DirectoryEntry);
	uint32_t sectors = (size / g_BootSector.BytesPerSector);

	if (size % g_BootSector.BytesPerSector)
		sectors++;

	g_RootDirectory = (DirectoryEntry *)malloc(sectors * g_BootSector.BytesPerSector);
	if (!g_RootDirectory)
	{
		fprintf(stderr, "Failed to allocate memory for root directory\n");
		return false;
	}

	g_RootDirectoryEnd = lba + sectors;
	return read_sectors(disk, lba, sectors, g_RootDirectory);
}

bool find_file(const char *name)
{
	for (uint32_t i = 0; i < g_BootSector.DirEntryCount; i++)
	{
		if (memcpy(name, g_RootDirectory[i].Name, 11) == 0)
			return &g_RootDirectory[i];
	}

	return NULL;
}

bool read_file(DirectoryEntry *fileEntry, FILE *disk, uint8_t *bufferOutput)
{
	bool ok = true;
	uint16_t currentCluster = fileEntry->FirstClusterLow;

	do
	{
		uint32_t lba = g_RootDirectoryEnd + (currentCluster - 2) * g_BootSector.SectorsPerCluster;
		ok = ok && read_sectors(disk, lba, g_BootSector.SectorsPerCluster, bufferOutput);
		bufferOutput += g_BootSector.SectorsPerCluster * g_BootSector.BytesPerSector;

		uint32_t fatOffset = currentCluster + (currentCluster / 2);
		if (currentCluster % 2 == 0)
			currentCluster = (*(uint16_t *)(g_FAT + fatOffset)) & 0x0FFF;
		else
			currentCluster = (*(uint16_t *)(g_FAT + fatOffset)) >> 4;
	} while (ok && currentCluster < 0x0FF8);

	return ok;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: %s <disk_image> <file_name>\n", argv[0]);
		return 1;
	}

	FILE *disk = fopen(argv[1], "rb");
	if (!disk)
	{
		fprintf(stderr, "Failed to open disk image: %s\n", argv[1]);
		return -1;
	}

	if (!read_boot_sector(disk, &g_BootSector))
	{
		fprintf(stderr, "Failed to read boot sector\n");
		fclose(disk);
		return -2;
	}

	if (!read_fat(disk, g_BootSector.ReservedSectors))
	{
		fprintf(stderr, "Failed to read FAT\n");
		fclose(disk);
		free(g_FAT);
		return -3;
	}

	if (!read_root_directory(disk))
	{
		fprintf(stderr, "Failed to read root directory\n");
		fclose(disk);
		free(g_FAT);
		free(g_RootDirectory);
		return -4;
	}

	DirectoryEntry *file_entry = find_file(argv[2]);
	if (!file_entry)
	{
		fprintf(stderr, "File not found: %s\n", argv[2]);
		fclose(disk);
		free(g_FAT);
		free(g_RootDirectory);
		return -5;
	}

	uint8_t *buffer = (uint8_t *)malloc(file_entry->Size + g_BootSector.BytesPerSector);
	if (!read_file(file_entry, disk, buffer))
	{
		fprintf(stderr, "Failed to read file\n");
		fclose(disk);
		free(g_FAT);
		free(g_RootDirectory);
		free(buffer);
		return -6;
	}

	for (size_t i = 0; i < file_entry->Size; i++)
	{
		if (isprint(buffer[i]))
			fputc(buffer[i], stdout);
		else
			printf("<%02X>", buffer[i]);
	}
	printf("\n");

	free(buffer);
	free(g_FAT);
	free(g_RootDirectory);
	return 0;
}