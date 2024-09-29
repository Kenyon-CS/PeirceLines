# PeirceLines
C++ program that simulates students entering a Pierce cafeteria with multiple food lines. The program uses queues to manage the lines and simulates student arrivals using a Poisson distribution. The simulation runs for one hour, and you can customize parameters such as the number of food lines, time spent at the head of each line, and the total number of students.

1. Data Structures:
  - `Student` struct: Holds information about each student, such as arrival time, line joined, and timestamps related to the queue.
  - Vectors and queues: Used to manage lines (lines), service times, and student indices.
2. User Inputs:
  - The program prompts the user to input:
    - Number of food lines (N)
    - Time at the head of each line (T)
    - Total number of students (S)
    - Popularity scores for each line (Pi)
  - Default values are provided if the user presses Enter without input.
3. Random Generators:
  - Exponential Distribution (`expDist`): Generates inter-arrival times for students based on the arrival rate.
  - Discrete Distribution (`lineDistribution`): Determines which line a student chooses based on popularity scores.
4. Simulation Logic:
  - Arrival Generation:
    - Students' arrival times are generated using the exponential distribution until the simulation time is reached.
  - Simulation Loop:
    - Runs for each second (time) up to the simulation time.
    - Processing Arrivals:
      - Checks if any students arrive at the current time and adds them to their chosen lines.
    - Processing Lines:
      - For each line, the program checks if a student is being served.
      - If a student finishes being served, they leave, and the next student (if any) starts being served.
    - Per-Minute Output:
      - At the end of each minute, the program outputs:
        - Number of students who arrived during that minute.
        - Length of each line.
5. End-of-Simulation Statistics:
  - For each line, the program outputs:
    - Number of students left in the line.
    - Average wait time for students who were served.

## Notes:
  - **Randomness**: Each run may produce different results due to the random nature of student arrivals and line selection.
  - **Adjustable Parameters**: You can experiment by changing the number of lines, service time, number of students, and popularity scores to see how they affect the simulation.
  - **Accuracy**: The program accounts for students still in line at the end of the simulation when reporting the number left but only calculates average wait times for students who have been served.


