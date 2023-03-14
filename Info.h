#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Solution.h"

int getParticipant(struct Participant* participant, int id, char* error);

void loopThroughParticipants(int indAmount, struct Participant* participant, char *dummy, int *participantCount, int *solutionCount);

int checkFileExists(FILE* indexTable, FILE* database, char* error) {
	if (indexTable == NULL || database == NULL) {
		strcpy(error, "DB files do not exits");
		return 0;
	}
	return 1;
}

int checkIndexExists(FILE* indexTable, char* error, int id) {
	fseek(indexTable, 0, SEEK_END);
	long indexTableSize = ftell(indexTable);
	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize) {
		strcpy(error, "no such ID in table");
		return 0;
	}
	return 1;
}

int checkRecordExists(struct Indexer indexer, char* error) {
	if (!indexer.exists) {
		strcpy(error, "the record has been deleted");
		return 0;
	}
	return 1;
}

int checkKeyPairUnique(struct Participant participant, int solutionId) {
	FILE* solutionDb = fopen(SOLUTION_DATA, "a+b");
	struct Solution solution;
	fseek(solutionDb, participant.firstSolutionAddress, SEEK_SET);
	for (int i = 0; i < participant.solutionCount; i++) {
		fread(&solution, SOLUTION_SIZE, 1, solutionDb);
		fclose(solutionDb);
		if (solution.solutionId == solutionId) {
			return 0;
		}
        solutionDb = fopen(SOLUTION_DATA, "r+b");
		fseek(solutionDb, solution.nextAddress, SEEK_SET);
	}
	fclose(solutionDb);
	return 1;
}

void info() {
	FILE* indexTable = fopen("participant.ind", "rb");
	if (indexTable == NULL) {
		printf("Error: database files do not exist\n");
		return;
	}
	int participantCount = 0;
	int solutionCount = 0;
	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);
	struct Participant participant;
	char dummy[51];
    loopThroughParticipants(indAmount, &participant, dummy, &participantCount, &solutionCount);
    fclose(indexTable);
	printf("Total participants: %d\n", participantCount);
	printf("Total solutions: %d\n", solutionCount);
}

void loopThroughParticipants(int indAmount, struct Participant* participant, char* dummy, int* participantCount, int* solutionCount) {
    for (int i = 1; i <= indAmount; i++) {
        if (getParticipant(participant, i, dummy)) {
            (*participantCount)++;
            (*solutionCount) += (*participant).solutionCount;
            printf("Participant #%d has %d solutions\n", i, (*participant).solutionCount);
        }
    }
}
