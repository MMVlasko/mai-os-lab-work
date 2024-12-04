#include <tools.h>

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_set>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <config.json> <max_parallel_jobs>" << std::endl;
        return 1;
    }

    const std::string configFile = argv[1];
    const int maxParallelJobs = std::stoi(argv[2]);

    if(argc == 4) {
        mustBreak = argv[3];
    }

    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << configFile << std::endl;
        return 1;
    }

    nlohmann::json config;
    file >> config;
    file.close();

    std::unordered_set<int> ids;
    for (const auto& job : config["jobs"]) {
        int id = job["id"];

        if (ids.find(id) != ids.end()) {
            std::cerr << "Error: DAG contains duplicated ids" << std::endl;
            return 1;
        }

        ids.insert(id);
        std::string name = job["name"];
        jobs[id] = {name, job["dependencies"], job.value("semaphore", ""),
            job.value("semaphore_limit", 1), job.value("time", 2)};
        graph[id] = job["dependencies"].get<std::vector<int>>();

        if (job.contains("semaphore")) {
            auto semExists = false;

            for (auto& [fst, _] : semaphores) {
                if (fst == job["semaphore"]) {
                    semExists = true;
                    break;
                }
            }

            if (!semExists) {
                const std::string sem_name = job["semaphore"];
                int semLimit = job.value("semaphore_limit", 1);
                sem_t semaphore;
                sem_init(&semaphore, 0, semLimit);
                semaphores.emplace(sem_name, semaphore);
            }
        }
    }

    if (!ValidateDAG(graph)) {
        return 1;
    }

    for (const auto& [job, deps] : graph) {
        inDegree[job] = deps.size();
        if (deps.empty()) {
            readyJobs.push(job);
        }
    }

    std::vector<pthread_t> workers;

    for (int i = 0; i < maxParallelJobs; ++i) {
        pthread_t thread;
        pthread_create(&thread, nullptr, ThreadProcess, nullptr);
        workers.emplace_back(thread);
    }

    pthread_cond_broadcast(&queueCV);

    for (auto& worker : workers) {
        pthread_join(worker, nullptr);
    }

    std::cout << (errorFlag ? "Execution failed" : "Execution completed successfully") << std::endl;

    return 0;
}