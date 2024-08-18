#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SPEED 88
#define MIN_YEAR 1950
#define MAX_YEAR 2024

uint32_t compare(const void *a, const void *b) {
  return (*(uint32_t *)a - *(uint32_t *)b);
}

typedef struct TimePeriod {
  uint32_t year;
  char description[50];
  struct TimePeriod *next;
} TimePeriod;

typedef struct {
  TimePeriod *head;
} TimeMachine;

void initialize_time_machine(TimeMachine *time_machine) {
  time_machine->head = NULL;
}

TimePeriod *create_time_period(uint32_t year, const char *description) {
  TimePeriod *new_period = (TimePeriod *)malloc(sizeof(TimePeriod));

  if (new_period == NULL) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }

  new_period->year = year;
  strcpy(new_period->description, description);
  new_period->next = NULL;

  return new_period;
}

void add_time_period(TimeMachine *time_machine, uint32_t year,
                     const char *description) {
  TimePeriod *new_period = create_time_period(year, description);

  if (time_machine->head == NULL) {
    time_machine->head = new_period;
  } else {
    TimePeriod *current = time_machine->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_period;
  }
}

void set_time_periods(TimeMachine *time_machine, uint32_t years) {
  char *descriptions[] = {"Past",        "Present",        "Future",
                          "Far Past",    "Future",         "Near Past",
                          "Near Future", "Distant Future", "Ancient Times",
                          "Far Future"};

  for (size_t i = 0; i < years; ++i) {
    uint32_t year = MIN_YEAR + rand() % (MAX_YEAR - MIN_YEAR + 1);
    add_time_period(time_machine, year, descriptions[i % sizeof(descriptions)]);
  }
}

void travel_to_period(TimePeriod *period) {
  printf("Traveling to year %d: %s\n", period->year, period->description);
}

void travel_through_time(TimeMachine *time_machine) {
  TimePeriod *current = time_machine->head;
  while (current != NULL) {
    travel_to_period(current);
    current = current->next;
  }
  travel_to_period(current);
}

void flux_capacitor(TimeMachine *time_machine) {
  printf("Flux capacitor activated! Reaching %d MPH...\n", SPEED);
  travel_through_time(time_machine);
}

void *parallel_time_travel(void *arg) {
  TimeMachine *params = (TimeMachine *)arg;
  printf("Initiating parallel time travel...\n");
  travel_through_time(params);
  return NULL;
}

void travel_to_year(uint32_t year) {
  printf("Initiating travel to year %d...\n", year);
  TimePeriod destination;
  destination.year = year;
  sprintf(destination.description, "Target Year - %d", year);
  travel_to_period(&destination);
}

void return_to_present() {
  printf("Returning to the present...\n");
  TimePeriod present;
  present.year = 1985;
  strcpy(present.description, "Present");
  travel_to_period(&present);
}

void free_time_machine(TimeMachine *time_machine) {
  TimePeriod *current = time_machine->head;
  while (current != NULL) {
    TimePeriod *temp = current;
    current = current->next;
    free(temp);
  }
  time_machine->head = NULL;  // Reset head pointer
}

int main(void) {
  srand(time(NULL));

  uint32_t years = 0;

  printf("Starting DeLorean...\n");
  printf("How many years do you want to try? ");
  scanf("%d", &years);
  printf("\n");

  TimeMachine time_machine;
  initialize_time_machine(&time_machine);
  set_time_periods(&time_machine, years);

  flux_capacitor(&time_machine);

  return_to_present();

  printf("Back to the future!\n");

  free_time_machine(&time_machine);

  return 0;
}