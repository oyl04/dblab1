#include <stdio.h>
#include "Structures.h"
#include "Participant.h"
#include "Solution.h"
#include "consolestream.h"

int main() {
	struct Participant participant;
	struct Solution solution;
	while (1) {
		int choice;
		int id;
		char error[51];
		printf("Enter the command:\n");
        printf("0 exit\n");
        printf("1 insert participant\n");
        printf("2 get participant\n");
        printf("3 update participant\n");
        printf("4 delete participant\n");
        printf("5 insert solution\n");
        printf("6 get solution\n");
        printf("7 update solution\n");
        printf("8 delete solution\n");
        printf("9 all information\n");
		printf("Enter the number to continue\n");
		scanf("%d", &choice);
		switch (choice) {
		case 0:
			return 0;
		case 1:
			inputParticipant(&participant);
			insertParticipant(participant);
			break;
		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			getParticipant(&participant, id, error) ? printParticipant(participant) : printf("Error: %s\n", error);
			break;
		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			participant.id = id;
			inputParticipant(&participant);
			updateParticipant(participant, error) ? printf("Updated successfully\n") : printf("Error: % s\n", error);
			break;
		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteParticipant(id, error) ? printf("Deleted successfully\n") : printf("Error: % s\n", error);
			break;
		case 5:
			printf("Enter participant ID: ");
			scanf("%d", &id);
			if (getParticipant(&participant, id, error)) {
				solution.participantId = id;
				printf("Enter solution ID: ");
				scanf("%d", &id);
				if (checkKeyPairUnique(participant, id)) {
					solution.solutionId = id;
					inputSolution(&solution);
					insertSolution(participant, solution, error);
					printf("Updated successfully\n");
				}
				else {
					printf("Error! Non-unique key\n");
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 6:
			printf("Enter participant ID: ");
			scanf("%d", &id);
			if (getParticipant(&participant, id, error)) {
				printf("Enter solution ID: ");
				scanf("%d", &id);
				if (getSolution(participant, &solution, id, error)) printSolution(solution, participant);
				else printf("Error: %s\n", error);
			}
			else {
                printf("Error: %s\n", error);
			}
			break;
		case 7:
			printf("Enter participant ID: ");
			scanf("%d", &id);
			if (getParticipant(&participant, id, error)) {
				printf("Enter solution ID: ");
				scanf("%d", &id);
				if (getSolution(participant, &solution, id, error)) {
					inputSolution(&solution);
					updateSolution(solution, id);
					printf("Updated successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 8:
			printf("Enter participant ID: ");
			scanf("%d", &id);
			if (getParticipant(&participant, id, error)) {
				printf("Enter solution ID: ");
				scanf("%d", &id);
				if (getSolution(participant, &solution, id, error)) {
					deleteSolution(participant, solution, id, error);
					printf("Deleted successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 9:
			info();
			break;
		default:
			printf("Invalid input\n");
		}
		printf("\n");
	}
}
