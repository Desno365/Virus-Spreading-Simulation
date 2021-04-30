# A Simple Model for Virus Spreading


## Assignment

Note: for more details on the assignment see [Projects 2021.pdf](https://github.com/Desno365/Virus-Spreading-Simulation/blob/master/Projects%202021.pdf)

#### Description of the project

Implement a program that simulates how a virus spreads over time in a population of individuals.
The program considers N individuals that move in a rectangular area with linear motion and velocity v (each individual following a different direction).
Some individuals are initially infected.
If an individual remains close to (at least one) infected individual for more than 10 minutes, it becomes infected.
After 10 days, an infected individual recovers and becomes immune.
Immune individuals do not become infected and do not infect others.
An immune individual becomes susceptible again (i.e., it can be infected) after 3 months.

The overall area is split into smaller rectangular sub-areas representing countries.
The program outputs, at the end of each simulated day, the overall number of susceptible, infected, and immune individuals in each country.
An individual belongs to a country if it is in that country at the end of the day.

#### Assumptions and Guidelines

* The program takes in input the following parameters
  * N = number of individuals;
  * I = number of individuals that are initially infected;
  * W, L = width and length of the rectangular area where individuals move (in meters);
  * w, l = width and length of each country (in meters);
  * v = moving speed for an individual;
  * d = maximum spreading distance (in meters): a susceptible individual that remains closer than d to at least one infected individual becomes infected;
  * t = time step (in seconds): the simulation recomputes the position and status (susceptible, infected, immune) of each individual with a temporal granularity of t (simulated) seconds;
* You can make any assumptions on the behavior of individuals when they reach the boundaries of the area (for instance, they can change direction to guarantee that they remain in the area)


## Usage

To run the program on a single machine install Open MPI first, then use the tasks defined in the Makefile to execute the program.

Instead, to run the program on multiple machines see [INSTRUCTIONS.md](https://github.com/Desno365/Virus-Spreading-Simulation/blob/master/INSTRUCTIONS.md).


## Grade

This project was made for the course: "Middleware Technologies for Distributed Systems" at "Politecnico di Milano", Academic Year 2020/2021.<br>
Grade: 30/30.


## Developers

[Accordi Gianmarco](https://github.com/gianfi12)

[Buratti Roberto](https://github.com/Furcanzo)

[Motta Dennis](https://github.com/Desno365)
