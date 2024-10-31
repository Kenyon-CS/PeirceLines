#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <iomanip>
#include <string>

// Class representing a student in the cafeteria
class Student {
public:
    int arrivalTime;     // Time student enters the cafeteria (in seconds)
    int lineJoined;      // Line number the student joined
    int timeJoinedLine;  // Time student joined the line
    int timeServed;      // Time student started being served
    int timeLeft;        // Time student leaves the line

    // Constructor
    Student(int arrival, int line)
        : arrivalTime(arrival), lineJoined(line),
          timeJoinedLine(0), timeServed(0), timeLeft(0) {}
};

// Class representing a food line in the cafeteria
class FoodLine {
private:
    std::queue<size_t> lineQueue; // Queue to manage students (stores indices of students)
    int serviceRemaining;         // Time remaining to serve the current student
    size_t servingStudentIndex;   // Index of the student being served (-1 if none)
    int studentsServed;           // Total number of students served
    long long totalWaitTime;      // Cumulative wait time for all served students

public:
    // Constructor
    FoodLine()
        : serviceRemaining(0), servingStudentIndex(-1),
          studentsServed(0), totalWaitTime(0) {}

    // Adds a student to the line
    void addStudent(size_t studentIdx) {
        lineQueue.push(studentIdx);
    }

    // Starts serving the next student in line
    void serveNextStudent(std::vector<Student>& students, int currentTime, int serviceTime) {
        if (!lineQueue.empty() && servingStudentIndex == (size_t)-1) {
            servingStudentIndex = lineQueue.front();
            lineQueue.pop();
            students[servingStudentIndex].timeServed = currentTime;
            serviceRemaining = serviceTime;
        }
    }

    // Processes the current serving student
    void processServingStudent(std::vector<Student>& students, int currentTime) {
        if (servingStudentIndex != (size_t)-1) {
            serviceRemaining--;
            if (serviceRemaining <= 0) {
                students[servingStudentIndex].timeLeft = currentTime;
                int waitTime = students[servingStudentIndex].timeServed - students[servingStudentIndex].timeJoinedLine;
                totalWaitTime += waitTime;
                studentsServed++;
                servingStudentIndex = (size_t)-1;
            }
        }
    }

    // Returns the number of students currently in line (including the one being served)
    int getLineLength() const {
        return lineQueue.size() + (servingStudentIndex != (size_t)-1 ? 1 : 0);
    }

    // Retrieves statistics for the line
    void getStatistics(int& served, double& avgWait) const {
        served = studentsServed;
        avgWait = served > 0 ? static_cast<double>(totalWaitTime) / served : 0.0;
    }

    // Checks if the line is currently serving a student
    bool isServing() const {
        return servingStudentIndex != (size_t)-1;
    }
};

// Class managing the entire cafeteria simulation
class CafeteriaSimulation {
private:
    int numLines;                        // Number of food lines
    int serviceTime;                     // Time spent at the head of each line (in seconds)
    int numStudents;                     // Number of students entering during the simulation
    int simulationDuration;              // Total simulation time (in seconds)
    std::vector<double> popularity;      // Popularity scores for each line
    std::vector<FoodLine> foodLines;     // Vector of food lines
    std::vector<Student> students;       // Vector of all students

public:
    // Constructor
    CafeteriaSimulation(int N, int T, int S, int duration, const std::vector<double>& pop)
        : numLines(N), serviceTime(T), numStudents(S), simulationDuration(duration), popularity(pop),
          foodLines(N), students() {}

    // Runs the simulation
    void runSimulation() {
        // Initialize random number generators
        std::random_device rd;
        std::mt19937 gen(rd());

        // Calculate total popularity to ensure it sums to 1
        double totalPopularity = 0.0;
        for (double p : popularity) {
            totalPopularity += p;
        }
        if (totalPopularity != 1.0) {
            std::cout << "Total popularity scores do not sum to 1. Normalizing...\n";
            for (double& p : popularity) {
                p /= totalPopularity;
            }
        }

        // Create a discrete distribution based on popularity for line selection
        std::discrete_distribution<> lineDist(popularity.begin(), popularity.end());

        // Calculate arrival rate per second
        double arrivalRate = static_cast<double>(numStudents) / simulationDuration;

        // Exponential distribution for inter-arrival times
        std::exponential_distribution<> expDist(arrivalRate);

        // Generate arrival times for students
        double currentTime = 0.0;
        size_t studentIdx = 0;
        while (currentTime < simulationDuration && studentIdx < static_cast<size_t>(numStudents)) {
            double interArrivalTime = expDist(gen);
            currentTime += interArrivalTime;
            if (currentTime >= simulationDuration) break;

            int arrival = static_cast<int>(currentTime);
            int lineChosen = lineDist(gen);
            students.emplace_back(arrival, lineChosen);
            studentIdx++;
        }

        // Sort students by arrival time
        std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
            return a.arrivalTime < b.arrivalTime;
        });

        // Simulation loop
        size_t nextStudent = 0;
        for (int time = 0; time < simulationDuration; time++) {
            // Add arriving students to their respective lines
            while (nextStudent < students.size() && students[nextStudent].arrivalTime == time) {
                int line = students[nextStudent].lineJoined;
                foodLines[line].addStudent(nextStudent);
                students[nextStudent].timeJoinedLine = time;
                std::cout << "Time " << time << ": Student " << nextStudent + 1
                          << " arrived and joined line " << line + 1 << ".\n";
                nextStudent++;
            }

            // Process each food line
            for (int i = 0; i < numLines; i++) {
                // Serve next student if not currently serving
                if (!foodLines[i].isServing()) {
                    foodLines[i].serveNextStudent(students, time, serviceTime);
                }

                // Process serving student
                foodLines[i].processServingStudent(students, time);
            }

            // Optional: Output status at specific intervals (e.g., every 60 seconds)
            if ((time + 1) % 60 == 0) {
                int minute = (time + 1) / 60;
                std::cout << "\n--- Minute " << minute << " ---\n";
                for (int i = 0; i < numLines; i++) {
                    std::cout << "Line " << i + 1 << " has " << foodLines[i].getLineLength() << " student(s) in line.\n";
                }
            }
        }

        // End of simulation statistics
        std::cout << "\n=== Simulation End ===\n";
        for (int i = 0; i < numLines; i++) {
            int served;
            double avgWait;
            foodLines[i].getStatistics(served, avgWait);
            std::cout << "Line " << i + 1 << " served " << served << " student(s).\n";
            std::cout << "Average wait time: " << std::fixed << std::setprecision(2) << avgWait << " seconds.\n";
        }
    }
};

int main() {
    // Default simulation parameters
    int N = 5;               // Number of food lines
    int T = 20;              // Time spent at the head of each line in seconds
    int S = 500;             // Number of students entering during the simulation
    int simulationTime = 3600; // Total simulation time in seconds (1 hour)

    // User input for simulation parameters
    std::string input;

    std::cout << "Enter number of food lines (default " << N << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) N = std::stoi(input);

    std::cout << "Enter time spent at the head of each line in seconds (default " << T << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) T = std::stoi(input);

    std::cout << "Enter number of students entering during the simulation (default " << S << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) S = std::stoi(input);

    // Read popularity scores for each line
    std::vector<double> popularity(N, 1.0 / N); // Initialize with default equal popularity
    double totalPopularity = 0.0;

    std::cout << "Enter popularity scores for each line (should sum to 1):\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "Line " << i + 1 << " popularity (default " << (1.0 / N) << "): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            popularity[i] = std::stod(input);
        }
        totalPopularity += popularity[i];
    }

    // Normalize popularity scores if they do not sum to 1
    if (std::abs(totalPopularity - 1.0) > 1e-6) { // Allowing a small epsilon for floating point
        std::cout << "Total popularity scores do not sum to 1. Normalizing...\n";
        for (double& p : popularity) {
            p /= totalPopularity;
        }
    }

    // Initialize and run the simulation
    CafeteriaSimulation simulation(N, T, S, simulationTime, popularity);
    simulation.runSimulation();

    return 0;
}
