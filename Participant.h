#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Info.h"
#include "Solution.h"

#define PARTICIPANT_IND "participant.ind"
#define PARTICIPANT_DATA "participant.fl"
#define PARTICIPANT_GARBAGE "participant_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define PARTICIPANT_SIZE sizeof(struct Participant)

void noteDeletedParticipant(int id) {
	FILE* garbageZone = fopen(PARTICIPANT_GARBAGE, "rb");
	int garbageAmount;
	fscanf(garbageZone, "%d", &garbageAmount);
	int* deleteIds = (int *)malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(PARTICIPANT_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	fprintf(garbageZone, " %d", id);
	free(deleteIds);
	fclose(garbageZone);
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Participant* record) {
	int* deleteIds = (int*)malloc(garbageCount * sizeof(int));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}
	record->id = deleteIds[0];

	fclose(garbageZone);
	fopen(PARTICIPANT_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	free(deleteIds);
	fclose(garbageZone);
}

int insertParticipant(struct Participant record) {
	FILE* indexTable = fopen(PARTICIPANT_IND, "a+b");
	FILE* database = fopen(PARTICIPANT_DATA, "a+b");
	FILE* garbageZone = fopen(PARTICIPANT_GARBAGE, "a+b");
	struct Indexer indexer;
	int garbageCount = 0;
	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageId(garbageCount, garbageZone, &record);
		fclose(indexTable);
		fclose(database);
		indexTable = fopen(PARTICIPANT_IND, "r+b");
		database = fopen(PARTICIPANT_DATA, "r+b");
		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);
	} else {
		long indexerSize = INDEXER_SIZE;
		fseek(indexTable, 0, SEEK_END);
		if (ftell(indexTable)) {
			fseek(indexTable, -indexerSize, SEEK_END);
			fread(&indexer, INDEXER_SIZE, 1, indexTable);
			record.id = indexer.id + 1;
		} else {
			record.id = 1;
		}
	}
	record.firstSolutionAddress = -1;
	record.solutionCount = 0;
	fwrite(&record, PARTICIPANT_SIZE, 1, database);

	indexer.id = record.id;
	indexer.address = (record.id - 1) * PARTICIPANT_SIZE;
	indexer.exists = 1;
	printf("Participant id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int getParticipant(struct Participant* participant, int id, char* error) {
	FILE* indexTable = fopen(PARTICIPANT_IND, "rb");
	FILE* database = fopen(PARTICIPANT_DATA, "rb");
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	struct Indexer indexer;
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fread(participant, sizeof(struct Participant), 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int updateParticipant(struct Participant participant, char* error) {
	FILE* indexTable = fopen(PARTICIPANT_IND, "r+b");
	FILE* database = fopen(PARTICIPANT_DATA, "r+b");
    struct Indexer indexer;
    int id = participant.id;
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fwrite(&participant, PARTICIPANT_SIZE, 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int deleteParticipant(int id, char* error) {
	FILE* indexTable = fopen(PARTICIPANT_IND, "r+b");
    struct Participant participant;
    struct Indexer indexer;
    if (indexTable == NULL) {
		strcpy(error, "database files are not created yet");
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
    getParticipant(&participant, id, error);
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	indexer.exists = 0;
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
    noteDeletedParticipant(id);

	if (participant.solutionCount) {
		FILE* solutionDb = fopen(SOLUTION_DATA, "r+b");
		struct Solution solution;
		fseek(solutionDb, participant.firstSolutionAddress, SEEK_SET);
		for (int i = 0; i < participant.solutionCount; i++) {
			fread(&solution, SOLUTION_SIZE, 1, solutionDb);
			fclose(solutionDb);
            deleteSolution(participant, solution, solution.solutionId, error);
            solutionDb = fopen(SOLUTION_DATA, "r+b");
			fseek(solutionDb, solution.nextAddress, SEEK_SET);
		}
		fclose(solutionDb);
	}
	return 1;
}
