
/*
 *
 * Simulation of Single Server Queueing System
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA,
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include "simlib.h"

/*******************************************************************************/

/*
 * Simulation Parameters
 */

#define RANDOM_SEED  6769420 //6254440 5259140 6769420
#define NUMBER_TO_SERVE 10e6


#define SERVICE_TIME 18 //double that of 9 (i.e 1+8)
// #define ARRIVAL_RATE 0.1

#define BLIP_RATE 10000

/*******************************************************************************/

/*
 * main() uses various simulation parameters and creates a clock variable to
 * simulate real time. A loop repeatedly determines if the next event to occur
 * is a customer arrival or customer departure. In either case the state of the
 * system is updated and statistics are collected before the next
 * iteration. When it finally reaches NUMBER_TO_SERVE customers, the program
 * outputs some statistics such as mean delay.
 */

int main()
{
  double clock = 0; /* Clock keeps track of simulation time. */
  int queue_caps[5] = {10, 50, 100, 500, 1000};



  for(int q = 0; q < 5; q++) {
    int MAX_QUEUE_SIZE = queue_caps[q];
    /* System state variables. */


    printf("MAX_QUEUE_SIZE,ARRIVAL_RATE,Utilization,Fraction served,Mean number in system,Mean delay,rejection ratio\n");
    //printf("ARRIVAL_RATE,Mean delay\n");
      /* Set the seed of the random number generator. */

    random_generator_initialize(RANDOM_SEED);

    //because we have a queue now, we don't have to stay below p<1, have to find arrival rates for appropriate critical response arrives
    for (double ARRIVAL_RATE = 0.01; ARRIVAL_RATE <= 0.51; ARRIVAL_RATE += 0.01) {
      
      int number_in_system = 0;
      double next_arrival_time = 0;
      double next_departure_time = 0;

    /* Data collection variables. */
      long int total_served = 0;
      long int total_arrived = 0;

      double total_busy_time = 0;
      double integral_of_n = 0;
      double last_event_time = 0;
      
      int customers_rejected = 0;
      double rejection_ratio = 0;
      double service_time = 0;



      /* Process customers until we are finished. */
      while (total_served < NUMBER_TO_SERVE) {
        

        /* Test if the next event is a customer arrival or departure. */
        if(number_in_system == 0 || next_arrival_time < next_departure_time) {

          /*
            * A new arrival is occurring.
            */

          clock = next_arrival_time;
          next_arrival_time = clock + exponential_generator((double) 1/ARRIVAL_RATE);

          /* Update our statistics. */
          integral_of_n += number_in_system * (clock - last_event_time);
          last_event_time = clock;

          // number_in_system++;
          total_arrived++;

          if(number_in_system < MAX_QUEUE_SIZE+1) {
            number_in_system++;
          }
          else {
            customers_rejected++;
          }

          /* If this customer has arrived to an empty system, start its
        service right away. */
          if(number_in_system == 1) {
          service_time = exponential_generator((double)SERVICE_TIME); 
          next_departure_time = clock + service_time;
          total_busy_time += service_time;
        }
        } else {

          /*
            * A customer departure is occuring. 
            */

          clock = next_departure_time;

          /* Update our statistics. */
          integral_of_n += number_in_system * (clock - last_event_time);
          last_event_time = clock;

          number_in_system--;
          total_served++;
          // total_busy_time += service_time; 
          /* 
            * If there are other customers waiting, start one in service
            * right away.
            */
          
          //we do this cuz departure and busy time need to use the same value
          //exponential_generator creates a new value everytime it is run. kindof like RNG
          if(number_in_system > 0) {
            service_time = exponential_generator((double)SERVICE_TIME); 
            next_departure_time = clock + service_time;
            total_busy_time += service_time;
          }

          /* 
            * Every so often, print an activity message to show we are active. 
            */

          //if (total_served % BLIP_RATE == 0)
            //printf("Customers served = %ld (Total arrived = %ld)\r", total_served, total_arrived);
        }
        
      }
      //need to add max_queue_size, rejection ratio,  
      // neec to modify mean delay calc
      //printf("ARRIVAL_RATE,Utilization,Fraction served,Mean number in system,Mean delay, \n");

      rejection_ratio = (double) customers_rejected / total_arrived;
      //printf("\nARRIVAL_RATE= %f\n", ARRIVAL_RATE);
      /* Output final results. */
      printf("%f,%f,%f,%f,%f,%f,%f\n", (double) MAX_QUEUE_SIZE, ARRIVAL_RATE, total_busy_time/clock, (double) total_served/total_arrived, integral_of_n/clock, integral_of_n/total_served, rejection_ratio);
      //printf("%f,%f\n", ARRIVAL_RATE, integral_of_n/total_served);
      /*
      printf("\nUtilization = %f\n", total_busy_time/clock);
      printf("Fraction served = %f\n", (double) total_served/total_arrived);
      printf("Mean number in system = %f\n", integral_of_n/clock);
      printf("Mean delay = %f\n", integral_of_n/total_served);
      */

      /* Halt the program before exiting. */
      // printf("Hit Enter to finish ... \n");
      // getchar(); 


    }
  } 

  return 0;

}






