#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <iomanip>

struct Student {
    int arrivalTime;     // Time student enters the cafeteria
    int lineJoined;      // Line number the student joined
    int timeJoinedLine;  // Time student joined the line
    int timeServed;      // Time student reached the front and started being served
    int timeLeft;        // Time student leaves the line
};

int main() {
    int N = 5;           // Number of food lines (default)
    int T = 20;          // Time spent at the head of each line in seconds (default)
    int S = 500;         // Number of students entering during the simulation (default)
    int simulationTime = 3600; // Total simulation time in seconds (1 hour)

    // Ask user to input parameters or use defaults
    std::cout << "Enter number of food lines (default " << N << "): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) N = std::stoi(input);

    std::cout << "Enter time spent at the head of each line in seconds (default " << T << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) T = std::stoi(input);

    std::cout << "Enter number of students entering during the hour (default " << S << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) S = std::stoi(input);

    // Read popularity scores for each line
    std::vector<double> popularity(N);
    double totalPopularity = 0.0;
    std::cout << "Enter popularity scores for each line (should sum to 1):\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "Line " << i + 1 << " popularity (default " << (1.0 / N) << "): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            popularity[i] = std::stod(input);
        } else {
            popularity[i] = 1.0 / N;
        }
        totalPopularity += popularity[i];
    }
    if (totalPopularity != 1.0) {
        std::cout << "Total popularity scores do not sum to 1. Normalizing...\n";
        for (int i = 0; i < N; ++i) {
            popularity[i] /= totalPopularity;
        }
    }

    // Initialize queues for each line
    std::vector<std::queue<size_t>> lines(N);           // Queues for each line
    std::vector<int> serviceRemaining(N, 0);            // Service time remaining for each line
    std::vector<int> servingStudentIndex(N, -1);        // Index of student being served at each line
    std::vector<int> studentsServed(N, 0);              // Total students served per line
    std::vector<long long> totalWaitTime(N, 0);         // Total wait time per line

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());

    // Arrival rate per second
    double arrivalRate = static_cast<double>(S) / simulationTime;

    // Exponential distribution for inter-arrival times
    std::exponential_distribution<> expDist(arrivalRate);

    // Discrete distribution for line selection based on popularity
    std::discrete_distribution<> lineDistribution(popularity.begin(), popularity.end());

    // Generate arrival times for students
    std::vector<Student> allStudents;
    double currentTime = 0.0;
    int studentId = 0;
    while (currentTime < simulationTime && studentId < S) {
        double interArrivalTime = expDist(gen);
        currentTime += interArrivalTime;
        if (currentTime >= simulationTime) break;

        Student student;
        student.arrivalTime = static_cast<int>(currentTime);

        // Determine which line the student will join
        int lineChosen = lineDistribution(gen);
        student.lineJoined = lineChosen;

        allStudents.push_back(student);
        studentId++;
    }

    // Simulation loop
    size_t studentIndex = 0; // Index of the next student to arrive
    for (int time = 0; time < simulationTime; ++time) {
        // Process student arrivals at this time
        while (studentIndex < allStudents.size() && allStudents[studentIndex].arrivalTime == time) {
            Student& student = allStudents[studentIndex];
            student.timeJoinedLine = time;
            int lineChosen = student.lineJoined;
            lines[lineChosen].push(studentIndex); // Add student index to the line's queue
            studentIndex++;
        }

        // Process each line
        for (int i = 0; i < N; ++i) {
            if (servingStudentIndex[i] != -1) {
                // Student is being served
                serviceRemaining[i]--;
                if (serviceRemaining[i] == 0) {
                    // Student has finished being served
                    size_t studentIdx = servingStudentIndex[i];
                    allStudents[studentIdx].timeLeft = time;
                    studentsServed[i]++;
                    // Calculate wait time
                    int waitTime = allStudents[studentIdx].timeServed - allStudents[studentIdx].timeJoinedLine;
                    totalWaitTime[i] += waitTime;
                    servingStudentIndex[i] = -1;
                }
            }

            if (servingStudentIndex[i] == -1) {
                if (!lines[i].empty()) {
                    // Start serving next student
                    size_t studentIdx = lines[i].front();
                    lines[i].pop();
                    allStudents[studentIdx].timeServed = time;
                    servingStudentIndex[i] = static_cast<int>(studentIdx);
                    serviceRemaining[i] = T;
                }
            }
        }

        // Output per minute
        if (time % 60 == 59) {
            std::cout << "\nMinute " << (time + 1) / 60 << ":\n";
            int arrivalsThisMinute = 0;
            for (size_t idx = 0; idx < allStudents.size(); ++idx) {
                if (allStudents[idx].arrivalTime >= time - 59 && allStudents[idx].arrivalTime <= time) {
                    arrivalsThisMinute++;
                }
            }
            std::cout << "Arrivals this minute: " << arrivalsThisMinute << "\n";
            for (int i = 0; i < N; ++i) {
                int lineLength = static_cast<int>(lines[i].size());
                if (servingStudentIndex[i] != -1) {
                    lineLength += 1; // Include the student being served
                }
                std::cout << "Line " << i + 1 << " length: " << lineLength << "\n";
            }
        }
    }

    // Output end of simulation statistics
    std::cout << "\nEnd of simulation:\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "Line " << i + 1 << ":\n";
        int peopleLeftInLine = static_cast<int>(lines[i].size());
        if (servingStudentIndex[i] != -1) {
            peopleLeftInLine += 1; // Include the student being served
        }
        std::cout << "  Number of students left in line: " << peopleLeftInLine << "\n";
        double avgWaitTime = studentsServed[i] > 0 ? static_cast<double>(totalWaitTime[i]) / studentsServed[i] : 0;
        std::cout << "  Average wait time: " << std::fixed << std::setprecision(2) << avgWaitTime << " seconds\n";
    }

    return 0;
}
